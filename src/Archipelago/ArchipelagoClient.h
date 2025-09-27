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
#include <functional>
#include <vector>
#include <set>

namespace OpenXcom
{

/**
 * Wrapper class around APCpp library providing game-specific functionality
 * Handles connection management, callbacks, and network communication
 */
class ArchipelagoClient
{
private:
    APConnectionInfo _connectionInfo;
    APConnectionStatus _connectionStatus;
    bool _initialized;
    
    // Callback function pointers
    std::function<void()> _itemClearCallback;
    std::function<void(int64_t, bool)> _itemRecvCallback;
    std::function<void(int64_t)> _locationCheckedCallback;
    
public:
    /// Creates the Archipelago client
    ArchipelagoClient();
    
    /// Cleans up the Archipelago client
    ~ArchipelagoClient();
    
    /// Initialize the APCpp library with connection details
    bool initialize(const APConnectionInfo& connectionInfo);
    
    /// Start the connection to Archipelago server
    bool connect();
    
    /// Disconnect from Archipelago server
    void disconnect();
    
    /// Shutdown and reset the library state
    void shutdown();
    
    /// Check if the client is initialized
    bool isInitialized() const;
    
    /// Get current connection status
    APConnectionStatus getConnectionStatus() const;
    
    /// Get connection information
    const APConnectionInfo& getConnectionInfo() const;
    
    /// Get player ID from server
    int getPlayerId() const;
    
    /// Send location check to server
    void sendLocationCheck(int64_t locationId);
    
    /// Send multiple location checks to server
    void sendLocationChecks(const std::set<int64_t>& locationIds);
    
    /// Send story completion status
    void sendStoryComplete();
    
    /// Set callback for when items need to be cleared
    void setItemClearCallback(std::function<void()> callback);
    
    /// Set callback for when items are received
    void setItemRecvCallback(std::function<void(int64_t, bool)> callback);
    
    /// Set callback for when locations are checked
    void setLocationCheckedCallback(std::function<void(int64_t)> callback);
    
    /// Check if there are pending messages
    bool isMessagePending() const;
    
    /// Get the latest message
    AP_Message* getLatestMessage();
    
    /// Clear the latest message
    void clearLatestMessage();
    
    /// Send a chat message
    void sendMessage(const std::string& message);
    
    /// Update the client (should be called regularly)
    void update();
    
    /// Get error message for connection errors
    static std::string getErrorMessage(APConnectionError error);
    
private:
    /// Internal callback handlers for APCpp
    void onItemClear();
    void onItemReceived(int64_t itemId, bool notify);
    void onLocationChecked(int64_t locationId);
    
    /// Validate connection parameters
    APConnectionError validateConnection(const APConnectionInfo& info) const;
    
    /// Parse server URL and extract host/port
    bool parseServerUrl(const std::string& url, std::string& host, int& port) const;
};

}