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
#include "../Savegame/SavedGame.h"

namespace OpenXcom
{

// Static instance
ArchipelagoManager* ArchipelagoManager::_instance = nullptr;

/**
 * Private constructor for singleton
 */
ArchipelagoManager::ArchipelagoManager() : _game(nullptr), _connected(false), _gameStarted(false)
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
ArchipelagoManager* ArchipelagoManager::getInstance()
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
void ArchipelagoManager::initialize(Game* game)
{
    _game = game;
    initializeResearchMappings();
    
    // Set up callbacks
    _client->setItemClearCallback([this]() { onItemsClear(); });
    _client->setItemRecvCallback([this](int64_t itemId, bool notify) { onItemReceived(itemId, notify); });
    _client->setLocationCheckedCallback([this](int64_t locationId) { onLocationChecked(locationId); });
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
bool ArchipelagoManager::connect(const APConnectionInfo& connectionInfo)
{
    if (!_client->initialize(connectionInfo))
    {
        return false;
    }
    
    _connected = _client->connect();
    return _connected;
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
const APConnectionInfo& ArchipelagoManager::getConnectionInfo() const
{
    static APConnectionInfo empty;
    return _client ? _client->getConnectionInfo() : empty;
}

/**
 * Start a new game with Archipelago integration
 */
void ArchipelagoManager::startNewGame()
{
    if (!validateConnection())
        return;
    
    _gameStarted = true;
    _receivedItems.clear();
    _checkedLocations.clear();
}

/**
 * Load game with Archipelago state
 * @param save Save game
 */
void ArchipelagoManager::loadGame(SavedGame* save)
{
    // TODO: Load AP state from save game
    _gameStarted = true;
}

/**
 * Save Archipelago state to save game
 * @param save Save game
 */
void ArchipelagoManager::saveGame(SavedGame* save)
{
    // TODO: Save AP state to save game
}

/**
 * Handle research completion - send location check
 * @param researchName Name of completed research
 */
void ArchipelagoManager::onResearchCompleted(const std::string& researchName)
{
    if (!validateConnection())
        return;
    
    int64_t locationId = getLocationFromResearch(researchName);
    if (locationId > 0)
    {
        _client->sendLocationCheck(locationId);
        
        // Mark location as checked locally
        for (auto& location : _checkedLocations)
        {
            if (location.locationId == locationId)
            {
                location.checked = true;
                break;
            }
        }
        
        triggerAutosave();
    }
}

/**
 * Check if research is unlocked by received items
 * @param researchName Research name to check
 * @return true if unlocked
 */
bool ArchipelagoManager::isResearchUnlocked(const std::string& researchName) const
{
    for (const auto& item : _receivedItems)
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
const std::vector<APResearchItem>& ArchipelagoManager::getReceivedItems() const
{
    return _receivedItems;
}

/**
 * Get list of checked locations
 * @return vector of checked locations
 */
const std::vector<APResearchLocation>& ArchipelagoManager::getCheckedLocations() const
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
    }
}

/**
 * Force autosave after AP events
 */
void ArchipelagoManager::triggerAutosave()
{
    // TODO: Trigger game autosave
}

/**
 * Initialize research mappings
 */
void ArchipelagoManager::initializeResearchMappings()
{
    // Map research names to location IDs
    _researchToLocationMap[APWorldConfig::RESEARCH_LASER_WEAPONS] = APWorldConfig::LOCATION_LASER_WEAPONS;
    _researchToLocationMap[APWorldConfig::RESEARCH_MEDI_KIT] = APWorldConfig::LOCATION_MEDI_KIT;
    _researchToLocationMap[APWorldConfig::RESEARCH_MOTION_SCANNER] = APWorldConfig::LOCATION_MOTION_SCANNER;
    
    // Map item IDs to research names
    _itemToResearchMap[APWorldConfig::ITEM_LASER_WEAPONS] = APWorldConfig::RESEARCH_LASER_WEAPONS;
    _itemToResearchMap[APWorldConfig::ITEM_MEDI_KIT] = APWorldConfig::RESEARCH_MEDI_KIT;
    _itemToResearchMap[APWorldConfig::ITEM_MOTION_SCANNER] = APWorldConfig::RESEARCH_MOTION_SCANNER;
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
    std::string researchName = getResearchFromItemId(itemId);
    if (!researchName.empty())
    {
        // Add to received items
        APResearchItem item(itemId, researchName);
        item.received = true;
        _receivedItems.push_back(item);
        
        // Unlock research immediately
        unlockResearch(researchName);
        
        triggerAutosave();
    }
}

/**
 * Callback for when location is checked
 * @param locationId Location ID
 */
void ArchipelagoManager::onLocationChecked(int64_t locationId)
{
    // Mark location as checked
    for (auto& location : _checkedLocations)
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
void ArchipelagoManager::unlockResearch(const std::string& researchName)
{
    // TODO: Implement immediate research unlocking
    // This should bypass normal research time/cost requirements
}

/**
 * Get research name from item ID
 * @param itemId Item ID
 * @return research name
 */
std::string ArchipelagoManager::getResearchFromItemId(int64_t itemId) const
{
    auto it = _itemToResearchMap.find(itemId);
    return (it != _itemToResearchMap.end()) ? it->second : "";
}

/**
 * Get location ID from research name
 * @param researchName Research name
 * @return location ID
 */
int64_t ArchipelagoManager::getLocationFromResearch(const std::string& researchName) const
{
    auto it = _researchToLocationMap.find(researchName);
    return (it != _researchToLocationMap.end()) ? it->second : 0;
}

/**
 * Validate that we're connected before operations
 * @return true if connected and ready
 */
bool ArchipelagoManager::validateConnection() const
{
    return _connected && _client && _client->getConnectionStatus() == APConnectionStatus::Authenticated;
}

}