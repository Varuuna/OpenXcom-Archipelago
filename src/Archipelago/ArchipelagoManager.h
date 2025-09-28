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
#include "ArchipelagoClient.h"
#include <memory>
#include <vector>
#include <map>

namespace OpenXcom
{

class SavedGame;
class Game;

/**
 * High-level manager for Archipelago integration
 * Coordinates between game systems and the Archipelago client
 * Manages connection state, item/location synchronization, and save integration
 */
class ArchipelagoManager
{
private:
    static ArchipelagoManager* _instance;
    
    std::unique_ptr<ArchipelagoClient> _client;
    Game* _game;
    
    // State tracking
    std::vector<APResearchItem> _receivedItems;
    std::vector<APResearchLocation> _checkedLocations;
    bool _connected;
    bool _gameStarted;
    
    // Research mappings
    std::map<std::string, int64_t> _researchToLocationMap;
    std::map<int64_t, std::string> _itemToResearchMap;
    
public:
    /// Get the singleton instance
    static ArchipelagoManager* getInstance();
    
    /// Initialize the manager with game reference
    void initialize(Game* game);
    
    /// Cleanup and shutdown
    void shutdown();
    
    /// Connect to Archipelago server
    bool connect(const APConnectionInfo& connectionInfo);
    
    /// Disconnect from server
    void disconnect();
    
    /// Check if connected to Archipelago
    bool isConnected() const;
    
    /// Get connection status
    APConnectionStatus getConnectionStatus() const;
    
    /// Get connection info
    const APConnectionInfo& getConnectionInfo() const;
    
    /// Start a new game with Archipelago integration
    void startNewGame();
    
    /// Load game with Archipelago state
    void loadGame(SavedGame* save);
    
    /// Save Archipelago state to save game
    void saveGame(SavedGame* save);
    
    /// Handle research completion - send location check
    void onResearchCompleted(const std::string& researchName);
    
    /// Check if research is unlocked by received items
    bool isResearchUnlocked(const std::string& researchName) const;
    
    /// Get list of received research items
    const std::vector<APResearchItem>& getReceivedItems() const;
    
    /// Get list of checked locations
    const std::vector<APResearchLocation>& getCheckedLocations() const;
    
    /// Update the manager (should be called regularly)
    void update();
    
    /// Force autosave after AP events
    void triggerAutosave();
    
private:
    /// Private constructor for singleton
    ArchipelagoManager();
    
    /// Private destructor
    ~ArchipelagoManager();
    
    /// Initialize research mappings
    void initializeResearchMappings();
    
    /// Initialize location mappings for current connection
    void initializeLocationMappings();
    
    /// Callback for when items are cleared
    void onItemsClear();
    
    /// Callback for when item is received
    void onItemReceived(int64_t itemId, bool notify);
    
    /// Callback for when location is checked
    void onLocationChecked(int64_t locationId);
    
    /// Unlock research immediately when item is received
    void unlockResearch(const std::string& researchName);
    
    /// Get research name from item ID
    std::string getResearchFromItemId(int64_t itemId) const;
    
    /// Get location ID from research name
    int64_t getLocationFromResearch(const std::string& researchName) const;
    
    /// Validate that we're connected before operations
    bool validateConnection() const;
};

}