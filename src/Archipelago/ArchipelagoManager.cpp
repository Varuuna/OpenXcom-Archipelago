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
#include "ResearchLocationMapper.h"
#include "../Engine/Game.h"
#include "../Engine/Logger.h"
#include "../Mod/Mod.h"
#include "../Mod/RuleResearch.h"
#include "../Savegame/SavedGame.h"
#include <sstream>

namespace OpenXcom
{

// Static instance
ArchipelagoManager* ArchipelagoManager::_instance = nullptr;

/**
 * Private constructor for singleton.
 */
ArchipelagoManager::ArchipelagoManager() :
	_game(nullptr),
	_enabled(false),
	_initialized(false),
	_connecting(false)
{
	Log(LOG_INFO) << "ArchipelagoManager: Created (using APCpp)";
}

/**
 * Destructor.
 */
ArchipelagoManager::~ArchipelagoManager()
{
	shutdown();
	Log(LOG_INFO) << "ArchipelagoManager: Destroyed";
}

/**
 * Gets the singleton instance.
 * @return Singleton instance
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
 * Destroys the singleton instance.
 */
void ArchipelagoManager::destroy()
{
	if (_instance)
	{
		delete _instance;
		_instance = nullptr;
	}
}

/**
 * Initializes the manager with game reference.
 * @param game Game instance
 */
void ArchipelagoManager::initialize(Game* game)
{
	if (_initialized)
		return;
	
	_game = game;
	
	// Create research location mapper
	_mapper = std::make_unique<ResearchLocationMapper>();
	
	// Initialize mapper with mod data
	if (_game && _game->getMod())
	{
		_mapper->initialize(_game->getMod());
	}
	
	// Set up APCpp callbacks using std::function
	AP_SetItemClearCallback(std::function<void()>(onItemClear));
	AP_SetItemRecvCallback(std::function<void(int64_t, bool)>(onItemReceived));
	AP_SetLocationCheckedCallback(std::function<void(int64_t)>(onLocationChecked));
	
	// Set client version
	AP_NetworkVersion version = {_config.version_major, _config.version_minor, _config.version_build};
	AP_SetClientVersion(&version);
	
	_initialized = true;
	Log(LOG_INFO) << "ArchipelagoManager: Initialized with APCpp";
}

/**
 * Shuts down the manager.
 */
void ArchipelagoManager::shutdown()
{
	if (!_initialized)
		return;
	
	disconnect();
	
	// Shutdown APCpp if it was initialized
	if (AP_IsInit())
	{
		AP_Shutdown();
	}
	
	_mapper.reset();
	_game = nullptr;
	_initialized = false;
	
	Log(LOG_INFO) << "ArchipelagoManager: Shut down";
}

/**
 * Checks if Archipelago is enabled.
 * @return True if enabled
 */
bool ArchipelagoManager::isEnabled() const
{
	return _enabled;
}

/**
 * Enables or disables Archipelago.
 * @param enabled Enable state
 */
void ArchipelagoManager::setEnabled(bool enabled)
{
	if (_enabled == enabled)
		return;
	
	_enabled = enabled;
	
	if (!_enabled && isConnected())
	{
		disconnect();
	}
	
	Log(LOG_INFO) << "ArchipelagoManager: " << (enabled ? "Enabled" : "Disabled");
}

/**
 * Checks if connected to Archipelago server.
 * @return True if connected
 */
bool ArchipelagoManager::isConnected() const
{
	if (!_enabled || !_initialized)
		return false;
	
	AP_ConnectionStatus status = AP_GetConnectionStatus();
	return status == AP_ConnectionStatus::Connected || status == AP_ConnectionStatus::Authenticated;
}

/**
 * Checks if authenticated with Archipelago server.
 * @return True if authenticated
 */
bool ArchipelagoManager::isAuthenticated() const
{
	if (!_enabled || !_initialized)
		return false;
	
	return AP_GetConnectionStatus() == AP_ConnectionStatus::Authenticated;
}

/**
 * Gets the current connection state.
 * @return Connection state
 */
ArchipelagoConnectionState ArchipelagoManager::getConnectionState() const
{
	if (!_enabled || !_initialized)
		return AP_DISCONNECTED;
	
	// If we're in the process of connecting but APCpp hasn't established connection yet,
	// we need to simulate the CONNECTING state since APCpp doesn't have one
	if (_connecting && !AP_IsInit())
		return AP_CONNECTING;
	
	if (!AP_IsInit())
		return AP_DISCONNECTED;
	
	AP_ConnectionStatus status = AP_GetConnectionStatus();
	switch (status)
	{
		case AP_ConnectionStatus::Disconnected:
			// If we initiated a connection but APCpp shows disconnected, we might be connecting
			return _connecting ? AP_CONNECTING : AP_DISCONNECTED;
		case AP_ConnectionStatus::Connected:
			return AP_CONNECTED;
		case AP_ConnectionStatus::Authenticated:
			return AP_AUTHENTICATED;
		case AP_ConnectionStatus::ConnectionRefused:
			return AP_ERROR;
		default:
			return AP_DISCONNECTED;
	}
}

/**
 * Sets the configuration.
 * @param config Archipelago configuration
 */
void ArchipelagoManager::setConfig(const ArchipelagoConfig& config)
{
	_config = config;
	Log(LOG_INFO) << "ArchipelagoManager: Configuration updated";
}

/**
 * Gets the current configuration.
 * @return Current configuration
 */
const ArchipelagoConfig& ArchipelagoManager::getConfig() const
{
	return _config;
}

/**
 * Connects to the Archipelago server.
 * @return True if connection initiated successfully
 */
bool ArchipelagoManager::connect()
{
	if (!_enabled || !_initialized)
	{
		Log(LOG_WARNING) << "ArchipelagoManager: Cannot connect - not enabled or initialized";
		return false;
	}
	
	if (isConnected())
	{
		Log(LOG_WARNING) << "ArchipelagoManager: Already connected";
		return true;
	}
	
	// Build server address string
	std::string serverAddress = _config.hostname + ":" + std::to_string(_config.port);
	
	Log(LOG_INFO) << "ArchipelagoManager: Connecting to " << serverAddress;
	
	try
	{
		// Set connecting state before initiating connection
		_connecting = true;
		
		// Initialize APCpp with connection parameters
		AP_Init(serverAddress.c_str(), _config.game.c_str(), _config.slot_name.c_str(), _config.password.c_str());
		
		// Start the connection
		AP_Start();
		
		Log(LOG_INFO) << "ArchipelagoManager: APCpp connection initiated";
		return true;
	}
	catch (const std::exception& e)
	{
		_connecting = false;
		Log(LOG_ERROR) << "ArchipelagoManager: Failed to connect: " << e.what();
		return false;
	}
	catch (...)
	{
		_connecting = false;
		Log(LOG_ERROR) << "ArchipelagoManager: Failed to connect: Unknown error";
		return false;
	}
}

/**
 * Disconnects from the server.
 */
void ArchipelagoManager::disconnect()
{
	_connecting = false;
	
	if (AP_IsInit())
	{
		AP_Shutdown();
	}
	
	Log(LOG_INFO) << "ArchipelagoManager: Disconnected";
}

/**
 * Updates the manager (call regularly from main game loop).
 */
void ArchipelagoManager::update()
{
	if (!_enabled || !_initialized)
		return;
	
	// Check if we were connecting and now have a definitive state
	if (_connecting && AP_IsInit())
	{
		AP_ConnectionStatus status = AP_GetConnectionStatus();
		if (status == AP_ConnectionStatus::Authenticated ||
		    status == AP_ConnectionStatus::ConnectionRefused)
		{
			_connecting = false;
			Log(LOG_INFO) << "ArchipelagoManager: Connection process completed, status: " << static_cast<int>(status);
		}
	}
	
	// APCpp handles updates internally, but we can process messages here if needed
	// For now, just let APCpp handle everything
}

/**
 * Notifies that a research project has been completed.
 * @param researchName Name of completed research
 */
void ArchipelagoManager::onResearchCompleted(const std::string& researchName)
{
	if (!_enabled || !isAuthenticated() || !_mapper)
		return;
	
	int64_t locationId = _mapper->getLocationId(researchName);
	if (locationId == 0)
	{
		Log(LOG_DEBUG) << "ArchipelagoManager: Research " << researchName << " not mapped to Archipelago location";
		return;
	}
	
	Log(LOG_INFO) << "ArchipelagoManager: Sending location check for research: " << researchName << " (ID: " << locationId << ")";
	
	// Send location check using APCpp
	AP_SendItem(locationId);
}

/**
 * Grants a research project from Archipelago.
 * @param researchName Name of research to grant
 */
void ArchipelagoManager::grantResearch(const std::string& researchName)
{
	if (!_game || !_game->getSavedGame() || !_game->getMod())
	{
		Log(LOG_ERROR) << "ArchipelagoManager: Cannot grant research - no game or saved game";
		return;
	}
	
	const RuleResearch* research = _game->getMod()->getResearch(researchName);
	if (!research)
	{
		Log(LOG_ERROR) << "ArchipelagoManager: Cannot grant research - research not found: " << researchName;
		return;
	}
	
	// Check if already researched
	if (_game->getSavedGame()->isResearched(researchName, false))
	{
		Log(LOG_DEBUG) << "ArchipelagoManager: Research already completed: " << researchName;
		return;
	}
	
	Log(LOG_INFO) << "ArchipelagoManager: Granting research from Archipelago: " << researchName;
	
	// Grant the research without triggering our own location check
	_game->getSavedGame()->addFinishedResearchSimple(research);
	
	Log(LOG_INFO) << "ArchipelagoManager: Successfully granted research: " << researchName;
}

/**
 * Gets the research location mapper.
 * @return Research location mapper
 */
ResearchLocationMapper* ArchipelagoManager::getMapper()
{
	return _mapper.get();
}

/**
 * Gets connection status string for UI.
 * @return Status string
 */
std::string ArchipelagoManager::getConnectionStatusString() const
{
	if (!_enabled)
		return "Disabled";
	
	ArchipelagoConnectionState state = getConnectionState();
	switch (state)
	{
		case AP_DISCONNECTED:
			return "Disconnected";
		case AP_CONNECTING:
			return "Connecting...";
		case AP_CONNECTED:
			return "Connected";
		case AP_AUTHENTICATED:
			return "Authenticated";
		case AP_ERROR:
			return "Error";
		default:
			return "Unknown";
	}
}

/**
 * Gets player info string for UI.
 * @return Player info string
 */
std::string ArchipelagoManager::getPlayerInfoString() const
{
	if (!isAuthenticated())
		return "";
	
	std::ostringstream info;
	info << "Player ID: " << AP_GetPlayerID();
	info << " | UUID: " << AP_GetUUID();
	
	return info.str();
}

// Static convenience methods

/**
 * Checks if the instance is connected (static).
 * @return True if connected
 */
bool ArchipelagoManager::isInstanceConnected()
{
	if (!_instance)
		return false;
	
	return _instance->isConnected();
}

/**
 * Notifies research completion (static).
 * @param researchName Name of completed research
 */
void ArchipelagoManager::notifyResearchCompleted(const std::string& researchName)
{
	if (!_instance)
		return;
	
	_instance->onResearchCompleted(researchName);
}

/**
 * Updates the instance (static).
 */
void ArchipelagoManager::updateInstance()
{
	if (!_instance)
		return;
	
	_instance->update();
}

// APCpp callback implementations

/**
 * APCpp callback: Clear all items (reset game state).
 */
void ArchipelagoManager::onItemClear()
{
	if (!_instance || !_instance->_game)
		return;
	
	Log(LOG_INFO) << "ArchipelagoManager: APCpp requested item clear";
	
	// Reset research state - this would need to be implemented based on game requirements
	// For now, just log the event
}

/**
 * APCpp callback: Item received from Archipelago.
 * @param itemId Item ID received
 * @param notify Whether to notify the player
 */
void ArchipelagoManager::onItemReceived(int64_t itemId, bool notify)
{
	if (!_instance || !_instance->_mapper)
		return;
	
	std::string researchName = _instance->_mapper->getResearchFromItem(itemId);
	if (researchName.empty())
	{
		Log(LOG_WARNING) << "ArchipelagoManager: Received unmapped item: " << itemId;
		return;
	}
	
	Log(LOG_INFO) << "ArchipelagoManager: Received item from Archipelago: " << researchName << " (ID: " << itemId << ")";
	
	// Grant the research
	_instance->grantResearch(researchName);
}

/**
 * APCpp callback: Location was checked.
 * @param locationId Location ID that was checked
 */
void ArchipelagoManager::onLocationChecked(int64_t locationId)
{
	if (!_instance || !_instance->_mapper)
		return;
	
	std::string researchName = _instance->_mapper->getResearchName(locationId);
	if (researchName.empty())
	{
		Log(LOG_WARNING) << "ArchipelagoManager: Location checked for unmapped location: " << locationId;
		return;
	}
	
	Log(LOG_INFO) << "ArchipelagoManager: Location checked: " << researchName << " (ID: " << locationId << ")";
}

}