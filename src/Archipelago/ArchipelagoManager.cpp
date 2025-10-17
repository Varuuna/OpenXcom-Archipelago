/*
 * Copyright 2010-2016 OpenXcom Developers.
 *
 * This file is part of OpenXcom.
 *
 * OpenXcom is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * OpenXcom is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with OpenXcom.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "ArchipelagoManager.h"
#include "../Engine/Game.h"
#include "../Geoscape/GeoscapeState.h"
#include "../Mod/Mod.h"
#include "../Mod/RuleResearch.h"
#include "../Savegame/Base.h"
#include "../Savegame/ResearchProject.h"
#include "../Savegame/SavedGame.h"
#include "ItemMappings.h"
#include <iostream>

namespace OpenXcom
{

// Static instance
ArchipelagoManager *ArchipelagoManager::_instance = nullptr;

/**
 * Private constructor for singleton
 */
ArchipelagoManager::ArchipelagoManager() : _game(nullptr), _geoscapeState(nullptr), _connected(false), _gameStarted(false), _locationsScouted(false)
{
	_client = std::make_unique<ArchipelagoClient>();
}

/**
 * Private destructor
 */
ArchipelagoManager::~ArchipelagoManager()
{
	shutdown();
}

/**
 * Get the singleton instance
 * @return singleton instance
 */
ArchipelagoManager *ArchipelagoManager::getInstance()
{
	if (!_instance)
	{
		_instance = new ArchipelagoManager();
	}
	return _instance;
}

/**
 * Initialize the manager with game reference
 * @param game Game instance
 */
void ArchipelagoManager::initialize(Game *game)
{
	_game = game;
	initializeItemMappings();

	// Set up callbacks
	_client->setItemClearCallback([this]()
								  { onItemsClear(); });
	_client->setItemRecvCallback([this](int64_t itemId, bool notify)
								 { onItemReceived(itemId, notify); });
	_client->setLocationCheckedCallback([this](int64_t locationId)
										{ onLocationChecked(locationId); });
	_client->setLocationInfoCallback([this](std::vector<AP_NetworkItem> items)
									 { onLocationInfoReceived(items); });
}

/**
 * Cleanup and shutdown
 */
void ArchipelagoManager::shutdown()
{
	if (_client)
	{
		_client->shutdown();
	}
	_connected = false;
	_gameStarted = false;
}

/**
 * Connect to Archipelago server
 * @param connectionInfo Connection information
 * @return true if connection successful
 */
bool ArchipelagoManager::connect(const APConnectionInfo &connectionInfo)
{
	if (!_client->initialize(connectionInfo))
	{
		return false;
	}

	bool result = _client->connect();
	if (result)
	{
		_connected = true;

		// Initialize location mappings for this connection
		initializeLocationMappings();
	}

	return result;
}

/**
 * Disconnect from server
 */
void ArchipelagoManager::disconnect()
{
	if (_client)
	{
		_client->disconnect();
	}
	_connected = false;
	_locationsScouted = false; // Reset so we can scout again on reconnection
}

/**
 * Check if connected to Archipelago
 * @return true if connected
 */
bool ArchipelagoManager::isConnected() const
{
	return _connected && _client && _client->getConnectionStatus() == APConnectionStatus::Authenticated;
}

/**
 * Get connection status
 * @return connection status
 */
APConnectionStatus ArchipelagoManager::getConnectionStatus() const
{
	return _client ? _client->getConnectionStatus() : APConnectionStatus::Disconnected;
}

/**
 * Get connection info
 * @return connection info
 */
const APConnectionInfo &ArchipelagoManager::getConnectionInfo() const
{
	static APConnectionInfo empty;
	return _client ? _client->getConnectionInfo() : empty;
}

/**
 * Start a new game with Archipelago integration
 */
void ArchipelagoManager::startNewGame()
{
	std::cout << "[AP] startNewGame() called" << std::endl;
	std::cout << "[AP] Connection status: " << (int)getConnectionStatus() << std::endl;
	std::cout << "[AP] Connected: " << (_connected ? "true" : "false") << std::endl;
	std::cout << "[AP] Client exists: " << (_client ? "true" : "false") << std::endl;

	if (!validateConnection())
	{
		std::cout << "[AP] validateConnection() failed, will wait for authentication" << std::endl;
		_gameStarted = true;
		_receivedItems.clear();
		_checkedLocations.clear();
		return;
	}

	std::cout << "[AP] Connection validated, starting new game" << std::endl;
	_gameStarted = true;
	_receivedItems.clear();
	_checkedLocations.clear();
}

/**
 * Load game with Archipelago state
 * @param save Save game
 */
void ArchipelagoManager::loadGame(SavedGame *save)
{
	if (!save)
		return;

	_gameStarted = true;

	// TODO: Load AP state from save game YAML
	// For now, clear state and let AP sync
	_receivedItems.clear();
	_checkedLocations.clear();

	// Initialize location mappings
	initializeLocationMappings();
}

/**
 * Handle research completion - send location check
 * @param researchName Name of completed research
 */
void ArchipelagoManager::onResearchCompleted(const std::string &researchName)
{
	if (!validateConnection())
		return;

	int64_t locationId = getLocationFromResearch(researchName);
	if (locationId > 0)
	{
		// Always send the location check to the AP server - let the server handle duplicates
		_client->sendLocationCheck(locationId);

		// Mark location as checked locally
		bool found = false;
		for (auto &location : _checkedLocations)
		{
			if (location.locationId == locationId)
			{
				location.checked = true;
				found = true;
				break;
			}
		}

		// Add location if not found
		if (!found)
		{
			APResearchLocation location(locationId, researchName + " Location", researchName);
			location.checked = true;
			_checkedLocations.push_back(location);
		}

		std::cout << "[AP] Notification: Sent item for " << researchName << std::endl;

		triggerAutosave();
	}
}

/**
 * Check if research is unlocked by received items
 * @param researchName Research name to check
 * @return true if unlocked
 */
bool ArchipelagoManager::isResearchUnlocked(const std::string &researchName) const
{
	for (const auto &item : _receivedItems)
	{
		if (item.received && getResearchFromItemId(item.itemId) == researchName)
		{
			return true;
		}
	}
	return false;
}

/**
 * Get list of received research items
 * @return vector of received items
 */
const std::vector<APItem> &ArchipelagoManager::getReceivedItems() const
{
	return _receivedItems;
}

/**
 * Get list of checked locations
 * @return vector of checked locations
 */
const std::vector<APResearchLocation> &ArchipelagoManager::getCheckedLocations() const
{
	return _checkedLocations;
}

/**
 * Update the manager (should be called regularly)
 */
void ArchipelagoManager::update()
{
	if (_client)
	{
		_client->update();

		// Server messages are processed through callbacks, no need for separate processing

		// Scout locations once we're authenticated and haven't done it yet
		if (!_locationsScouted && isConnected())
		{
			std::cout << "[AP] Game started: " << (_gameStarted ? "true" : "false") << std::endl;

			// If game was started but we weren't authenticated yet, mark it as started now
			if (!_gameStarted)
			{
				std::cout << "[AP] Setting game started to true since we're now authenticated" << std::endl;
				_gameStarted = true;
				_receivedItems.clear();
				_checkedLocations.clear();
			}

			createDynamicResearchProjects();
			_locationsScouted = true;
		}
	}
	else
	{
		static int logCount = 0;
		if (logCount++ < 5) // Only log first few times to avoid spam
		{
			std::cout << "[AP] update() called but no client exists" << std::endl;
		}
	}
}

/**
 * Force autosave after AP events
 */
void ArchipelagoManager::triggerAutosave()
{
	if (_game && _gameStarted)
	{
		SavedGame *save = _game->getSavedGame();
		if (save)
		{
			try
			{
				// Use the geoscape autosave filename
				std::string filename = SavedGame::AUTOSAVE_GEOSCAPE;
				save->save(filename);
				std::cout << "[AP] Autosave triggered: " << filename << std::endl;
			}
			catch (const std::exception &e)
			{
				std::cout << "[AP] Autosave failed: " << e.what() << std::endl;
			}
		}
	}
}

/**
 * Initialize item mappings
 */
void ArchipelagoManager::initializeItemMappings()
{
	// Load mappings from the ItemMappings class
	ItemMappings::getItemToLocationMap(_itemToLocationMap);
	ItemMappings::getReceivedItemMap(_receivedItemMap);
}

/**
 * Initialize location mappings for the current connection
 */
void ArchipelagoManager::initializeLocationMappings()
{
	// Create location objects for tracking all locations
	_checkedLocations.clear();

	// Initialize all locations from the item to location map
	for (const auto &pair : _itemToLocationMap)
	{
		const std::string &itemName = pair.first;
		int64_t locationId = pair.second;

		APResearchLocation location(locationId, itemName + " Location", itemName);
		_checkedLocations.push_back(location);
	}

	std::cout << "[AP] Initialized " << _checkedLocations.size() << " location mappings" << std::endl;
}

/**
 * Callback for when items are cleared
 */
void ArchipelagoManager::onItemsClear()
{
	_receivedItems.clear();
}

/**
 * Callback for when item is received
 * @param itemId Item ID
 * @param notify Whether to notify player
 */
void ArchipelagoManager::onItemReceived(int64_t itemId, bool notify)
{
	std::cout << "[AP] Item received - ID: " << itemId << std::endl;

	const APItemInfo *apItem = getItemById(itemId);
	if (apItem)
	{
		// Add to received items
		APItem item(itemId, apItem->name);
		item.received = true;
		_receivedItems.push_back(item);

		std::cout << "[AP] Research item received: " << item.itemName << std::endl;

		if (apItem->type == APItemType::Research)
		{
			// Unlock research immediately
			auto research = _game->getMod()->getResearch(item.itemName);
			_game->getSavedGame()->addFinishedResearchSimple(research);
		}
	}

	_client->clearLatestMessage();
}

/**
 * Callback for when location is checked
 * @param locationId Location ID
 */
void ArchipelagoManager::onLocationChecked(int64_t locationId)
{
	// Mark location as checked
	for (auto &location : _checkedLocations)
	{
		if (location.locationId == locationId)
		{
			location.checked = true;
			break;
		}
	}
}

/**
 * Unlock research immediately when item is received
 * @param researchName Research to unlock
 */
void ArchipelagoManager::unlockResearch(const std::string &researchName)
{
	if (!_game || !_game->getSavedGame())
		return;

	SavedGame *save = _game->getSavedGame();
	Mod *mod = _game->getMod();

	// Get the research rule
	const RuleResearch *research = mod->getResearch(researchName, false);
	if (!research)
	{
		std::cout << "[AP] Warning: Research rule not found for: " << researchName << std::endl;
		return;
	}

	// Check if already researched
	if (save->isResearched(researchName, false))
	{
		std::cout << "[AP] Research already completed: " << researchName << std::endl;
		return;
	}

	std::cout << "[AP] Unlocking research: " << researchName << std::endl;

	// Remove from completed-but-not-unlocked tracking
	_completedButNotUnlockedResearch.erase(researchName);

	// Add the research as completed to all bases
	for (auto *base : *save->getBases())
	{
		save->addFinishedResearch(research, mod, base);
	}

	// Also handle lookup research if it exists
	if (!research->getLookup().empty())
	{
		const RuleResearch *lookupResearch = mod->getResearch(research->getLookup(), false);
		if (lookupResearch)
		{
			for (auto *base : *save->getBases())
			{
				save->addFinishedResearch(lookupResearch, mod, base);
			}
		}
	}

	std::cout << "[AP] Research unlocked successfully: " << researchName << std::endl;
}

/**
 * Get research name from item ID
 * @param itemId Item ID
 * @return research name
 */
std::string ArchipelagoManager::getResearchFromItemId(int64_t itemId) const
{
	auto it = _receivedItemMap.find(itemId);
	if (it != _receivedItemMap.end() && it->second.type == APItemType::Research)
	{
		return it->second.name;
	}
	return "";
}

/**
 * Get item info from item ID
 * @param itemId Item ID
 * @return pointer to item info or nullptr if not found
 */
const APItemInfo *ArchipelagoManager::getItemById(int64_t itemId) const
{
	auto it = _receivedItemMap.find(itemId);
	if (it != _receivedItemMap.end())
	{
		return &it->second;
	}
	return nullptr;
}

/**
 * Get location ID from research name
 * @param researchName Research name
 * @return location ID
 */
int64_t ArchipelagoManager::getLocationFromResearch(const std::string &itemName) const
{
	auto it = _itemToLocationMap.find(itemName);
	return (it != _itemToLocationMap.end()) ? it->second : 0;
}

/**
 * Validate that we're connected before operations
 * @return true if connected and ready
 */
bool ArchipelagoManager::validateConnection() const
{
	return _connected && _client && _client->getConnectionStatus() == APConnectionStatus::Authenticated;
}

/**
 * Get the AP item name that will be sent for this research
 * @param researchName Original research name
 * @return AP item name (e.g., "Flute (Player1)")
 */
std::string ArchipelagoManager::getAPItemNameForResearch(const std::string &researchName) const
{
	auto it = _apResearchNames.find(researchName);
	return (it != _apResearchNames.end()) ? it->second : researchName;
}

/**
 * Check if this research should use AP naming
 * @param researchName Research name to check
 * @return true if this is an AP research
 */
bool ArchipelagoManager::isAPResearch(const std::string &researchName) const
{
	return _apResearchNames.find(researchName) != _apResearchNames.end();
}

/**
 * Check if research should skip automatic unlocking (for AP-mapped research)
 * @param researchName Research name to check
 * @return true if automatic unlocking should be skipped
 */
bool ArchipelagoManager::shouldSkipResearchUnlock(const std::string &researchName) const
{
	// Skip automatic unlocking if this item is mapped to an AP location
	return isConnected() && (_itemToLocationMap.find(researchName) != _itemToLocationMap.end());
}

/**
 * Check if research is completed but not yet unlocked (for AP research filtering)
 * @param researchName Research name to check
 * @return true if research is completed but not unlocked
 */
bool ArchipelagoManager::isResearchCompletedButNotUnlocked(const std::string &researchName) const
{
	return _completedButNotUnlockedResearch.find(researchName) != _completedButNotUnlockedResearch.end();
}

/**
 * Mark research as completed but not unlocked (for AP research tracking)
 * @param researchName Research name to mark
 */
void ArchipelagoManager::markResearchCompletedButNotUnlocked(const std::string &researchName)
{
	_completedButNotUnlockedResearch.insert(researchName);
}

/**
 * Create dynamic research projects from AP locations
 */
void ArchipelagoManager::createDynamicResearchProjects()
{
	if (!validateConnection())
	{
		std::cout << "[AP] Cannot create dynamic research projects - not connected" << std::endl;
		return;
	}

	std::cout << "[AP] Dynamic research projects initialized - using original item names" << std::endl;
	std::cout << "[AP] Item to location map size: " << _itemToLocationMap.size() << std::endl;

	// Clear any existing AP research names - we're using original names
	_apResearchNames.clear();

	std::cout << "[AP] Research projects will use original OpenXcom names" << std::endl;
}

/**
 * Callback for when location info is received
 * @param items Vector of location items from server
 */
void ArchipelagoManager::onLocationInfoReceived(const std::vector<AP_NetworkItem> &items)
{
	std::cout << "[AP] Received location info for " << items.size() << " items" << std::endl;

	for (size_t i = 0; i < items.size(); ++i)
	{
		const auto &item = items[i];
		std::cout << "[AP] Item " << (i + 1) << ":" << std::endl;
		std::cout << "[AP]   Location ID: " << item.location << std::endl;
		std::cout << "[AP]   Item ID: " << item.item << std::endl;
		std::cout << "[AP]   Item Name: '" << item.itemName << "'" << std::endl;
		std::cout << "[AP]   Location Name: '" << item.locationName << "'" << std::endl;
		std::cout << "[AP]   Player ID: " << item.player << std::endl;
		std::cout << "[AP]   Player Name: '" << item.playerName << "'" << std::endl;
		std::cout << "[AP]   Flags: " << item.flags << std::endl;

		_locationData[item.location] = item;

		// Find the item name that corresponds to this location
		std::string itemName;
		for (const auto &pair : _itemToLocationMap)
		{
			if (pair.second == item.location)
			{
				itemName = pair.first;
				break;
			}
		}

		if (!itemName.empty())
		{
			// Create AP display name: "ItemName (PlayerName)"
			std::string apDisplayName = item.itemName + " (" + item.playerName + ")";
			_apResearchNames[itemName] = apDisplayName;

			std::cout << "[AP] Mapped item '" << itemName << "' -> '" << apDisplayName << "'" << std::endl;
		}
		else
		{
			std::cout << "[AP] No item mapping found for location " << item.location << std::endl;
		}
	}

	std::cout << "[AP] Dynamic research mapping complete with " << _apResearchNames.size() << " mappings!" << std::endl;

	// Now that we have the AP item names, add research projects to the first base
	if (_gameStarted && _game && _game->getSavedGame() && !_game->getSavedGame()->getBases()->empty())
	{
		Base *firstBase = _game->getSavedGame()->getBases()->front();
		addAPResearchProjectsToBase(firstBase);
		std::cout << "[AP] Research projects added with proper AP names!" << std::endl;
	}
	else
	{
		std::cout << "[AP] Cannot add research projects - game not started or no bases available" << std::endl;
		std::cout << "[AP]   Game started: " << (_gameStarted ? "true" : "false") << std::endl;
		std::cout << "[AP]   Game exists: " << (_game ? "true" : "false") << std::endl;
		if (_game && _game->getSavedGame())
		{
			std::cout << "[AP]   Bases count: " << _game->getSavedGame()->getBases()->size() << std::endl;
		}
	}
}

/**
 * Add AP research projects to a base
 * @param base Base to add research projects to
 */
void ArchipelagoManager::addAPResearchProjectsToBase(Base *base)
{
	if (!base || !_game || !_game->getMod())
	{
		std::cout << "[AP] Cannot add research projects - missing base, game, or mod" << std::endl;
		return;
	}

	std::cout << "[AP] Adding AP research projects to base..." << std::endl;
	std::cout << "[AP] Current AP research names map size: " << _apResearchNames.size() << std::endl;

	// Debug: Print all AP research names
	for (const auto &pair : _apResearchNames)
	{
		std::cout << "[AP] AP Name mapping: " << pair.first << " -> " << pair.second << std::endl;
	}

	Mod *mod = _game->getMod();

	// Check if base already has research projects
	std::cout << "[AP] Base currently has " << base->getResearch().size() << " research projects" << std::endl;

	// Add research projects for each AP location
	for (const auto &pair : _itemToLocationMap)
	{
		const std::string &researchName = pair.first;

		// Check if this research already exists in the base
		bool alreadyExists = false;
		for (const auto *existingProject : base->getResearch())
		{
			if (existingProject->getRules()->getName() == researchName)
			{
				alreadyExists = true;
				std::cout << "[AP] Research project already exists: " << researchName << std::endl;
				break;
			}
		}

		if (alreadyExists)
			continue;

		// Get the research rule
		const RuleResearch *research = mod->getResearch(researchName, false);
		if (!research)
		{
			std::cout << "[AP] Warning: Research rule not found for: " << researchName << std::endl;
			continue;
		}

		// Create a new research project
		ResearchProject *project = new ResearchProject(const_cast<RuleResearch *>(research), research->getCost());

		// Add the project to the base
		base->addResearch(project);

		std::cout << "[AP] Added research project: " << researchName << std::endl;

		// Check if we have an AP name for this research
		auto apNameIt = _apResearchNames.find(researchName);
		if (apNameIt != _apResearchNames.end())
		{
			std::cout << "[AP] This research has AP name: " << apNameIt->second << std::endl;
		}
		else
		{
			std::cout << "[AP] No AP name found for research: " << researchName << std::endl;
		}
	}

	std::cout << "[AP] AP research projects added successfully!" << std::endl;
	std::cout << "[AP] Base now has " << base->getResearch().size() << " research projects" << std::endl;
}

/**
 * Set the current Geoscape state for notifications
 * @param geoscape Pointer to the current GeoscapeState
 */
void ArchipelagoManager::setGeoscapeState(GeoscapeState *geoscape)
{
	_geoscapeState = geoscape;
}

}