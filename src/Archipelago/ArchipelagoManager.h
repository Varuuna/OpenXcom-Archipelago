#pragma once
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

#include "ArchipelagoTypes.h"
#include "../APCpp/Archipelago.h"
#include <string>
#include <memory>

namespace OpenXcom
{

class ResearchLocationMapper;
class Game;
class Mod;
class RuleResearch;

/**
 * Main manager class for Archipelago integration using APCpp library.
 * This class provides the high-level interface for OpenXcom to interact with Archipelago
 * using the APCpp C++ library instead of a custom implementation.
 */
class ArchipelagoManager
{
private:
	// Game references
	Game* _game;
	
	// Configuration and state
	ArchipelagoConfig _config;
	bool _enabled;
	bool _initialized;
	bool _connecting;
	
	// Research location mapper (keep existing functionality)
	std::unique_ptr<ResearchLocationMapper> _mapper;
	
	// Singleton instance
	static ArchipelagoManager* _instance;
	
	// Private constructor for singleton
	ArchipelagoManager();
	
	// APCpp callback functions (static C-style callbacks)
	static void onItemClear();
	static void onItemReceived(int64_t itemId, bool notify);
	static void onLocationChecked(int64_t locationId);
	
public:
	/// Destructor
	~ArchipelagoManager();
	
	/// Gets the singleton instance
	static ArchipelagoManager* getInstance();
	/// Destroys the singleton instance
	static void destroy();
	
	/// Initializes the manager with game reference
	void initialize(Game* game);
	/// Shuts down the manager
	void shutdown();
	
	/// Checks if Archipelago is enabled
	bool isEnabled() const;
	/// Enables or disables Archipelago
	void setEnabled(bool enabled);
	
	/// Checks if connected to Archipelago server
	bool isConnected() const;
	/// Checks if authenticated with Archipelago server
	bool isAuthenticated() const;
	/// Gets the current connection state
	ArchipelagoConnectionState getConnectionState() const;
	
	/// Sets the configuration
	void setConfig(const ArchipelagoConfig& config);
	/// Gets the current configuration
	const ArchipelagoConfig& getConfig() const;
	
	/// Connects to the Archipelago server
	bool connect();
	/// Disconnects from the server
	void disconnect();
	
	/// Updates the manager (call regularly from main game loop)
	void update();
	
	/// Notifies that a research project has been completed
	void onResearchCompleted(const std::string& researchName);
	/// Grants a research project from Archipelago
	void grantResearch(const std::string& researchName);
	
	/// Gets the research location mapper
	ResearchLocationMapper* getMapper();
	
	/// Gets connection status string for UI
	std::string getConnectionStatusString() const;
	/// Gets player info string for UI
	std::string getPlayerInfoString() const;
	
	// Static convenience methods for global access
	static bool isInstanceConnected();
	static void notifyResearchCompleted(const std::string& researchName);
	static void updateInstance();
};

}