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

#include "ArchipelagoClient.h"
#include "../APCpp/Archipelago.h"
#include <iostream>
#include <chrono>

namespace OpenXcom
{

/**
 * Creates the Archipelago client
 */
ArchipelagoClient::ArchipelagoClient() : _connectionStatus(APConnectionStatus::Disconnected), _initialized(false)
{
}

/**
 * Cleans up the Archipelago client
 */
ArchipelagoClient::~ArchipelagoClient()
{
    if (_initialized)
    {
        shutdown();
    }
}

/**
 * Initialize the APCpp library with connection details
 * @param connectionInfo Connection information
 * @return true if initialization successful
 */
bool ArchipelagoClient::initialize(const APConnectionInfo& connectionInfo)
{
    std::cout << "[AP] Initializing connection..." << std::endl;
    std::cout << "[AP] Server URL: " << connectionInfo.serverUrl << std::endl;
    std::cout << "[AP] Slot Name: " << connectionInfo.slotName << std::endl;
    
    APConnectionError error = validateConnection(connectionInfo);
    if (error != AP_ERROR_NONE)
    {
        std::cout << "[AP] Validation failed: " << getErrorMessage(error) << std::endl;
        return false;
    }
    
    _connectionInfo = connectionInfo;
    
    // Parse server URL to get host and port
    std::string host;
    int port;
    if (!parseServerUrl(connectionInfo.serverUrl, host, port))
    {
        std::cout << "[AP] Failed to parse server URL" << std::endl;
        return false;
    }
    
    std::cout << "[AP] Parsed host: " << host << ", port: " << port << std::endl;
    
    try {
        // Initialize APCpp library
        std::string portStr = std::to_string(port);
        // APCpp expects the server address in format "host:port", not separate parameters
        std::string serverAddress = host + ":" + portStr;
        std::cout << "[AP] Calling AP_Init with:" << std::endl;
        std::cout << "[AP]   Server: '" << serverAddress << "'" << std::endl;
        std::cout << "[AP]   Game: 'OpenXcom'" << std::endl;
        std::cout << "[AP]   Slot: '" << connectionInfo.slotName << "'" << std::endl;
        std::cout << "[AP]   Password: '" << (connectionInfo.password.empty() ? "(empty)" : "(provided)") << "'" << std::endl;
        
        // Set client version to latest (0.6.4)
        AP_NetworkVersion version = {0, 6, 4};
        AP_SetClientVersion(&version);
        std::cout << "[AP] Set client version to 0.6.4" << std::endl;
        
        AP_Init(serverAddress.c_str(), "OpenXcom", connectionInfo.slotName.c_str(), connectionInfo.password.c_str());
        std::cout << "[AP] AP_Init() completed" << std::endl;
        
        // Set up callbacks before starting
        std::cout << "[AP] Setting up callbacks..." << std::endl;
        AP_SetItemClearCallback([this]() { onItemClear(); });
        AP_SetItemRecvCallback([this](int64_t itemId, bool notify) { onItemReceived(itemId, notify); });
        AP_SetLocationCheckedCallback([this](int64_t locationId) { onLocationChecked(locationId); });
        AP_SetLocationInfoCallback([this](std::vector<AP_NetworkItem> items) {
            if (_locationInfoCallback) _locationInfoCallback(items);
        });
        std::cout << "[AP] Callbacks set up successfully" << std::endl;
        
        // Enable message queuing for item send/receive notifications
        std::cout << "[AP] Enabling message queuing..." << std::endl;
        AP_EnableQueueItemRecvMsgs(true);
        
        // AP_Init only sets up parameters, we need to call AP_Start to actually initialize
        std::cout << "[AP] Calling AP_Start()..." << std::endl;
        AP_Start();
        std::cout << "[AP] AP_Start() completed" << std::endl;
        
        // Now check if initialization was successful
        _initialized = AP_IsInit();
        std::cout << "[AP] AP_IsInit() returned: " << (_initialized ? "true" : "false") << std::endl;
        
        // Check initial connection status
        AP_ConnectionStatus status = AP_GetConnectionStatus();
        std::cout << "[AP] Initial connection status: " << static_cast<int>(status) << " (";
        switch(status) {
            case AP_ConnectionStatus::Disconnected: std::cout << "Disconnected"; break;
            case AP_ConnectionStatus::Connected: std::cout << "Connected"; break;
            case AP_ConnectionStatus::Authenticated: std::cout << "Authenticated"; break;
            case AP_ConnectionStatus::ConnectionRefused: std::cout << "ConnectionRefused"; break;
        }
        std::cout << ")" << std::endl;
        
        return _initialized;
    }
    catch (const std::exception& e) {
        std::cout << "[AP] Exception during initialization: " << e.what() << std::endl;
        return false;
    }
    catch (...) {
        std::cout << "[AP] Unknown exception during initialization" << std::endl;
        return false;
    }
}

/**
 * Start the connection to Archipelago server
 * @return true if connection started successfully
 */
bool ArchipelagoClient::connect()
{
    std::cout << "[AP] Starting connection..." << std::endl;
    
    if (!_initialized)
    {
        std::cout << "[AP] Not initialized, cannot connect" << std::endl;
        return false;
    }
    
    try {
        // Start the connection
        AP_Start();
        std::cout << "[AP] AP_Start() called" << std::endl;
        
        // Wait briefly for connection to establish (websocket is asynchronous)
        std::cout << "[AP] Waiting for connection to establish..." << std::endl;
        for (int i = 0; i < 50; i++) { // Wait up to 5 seconds (50 * 100ms)
            _connectionStatus = AP_GetConnectionStatus();
            if (_connectionStatus != APConnectionStatus::Disconnected) {
                break;
            }
            // Simple sleep equivalent - busy wait for 100ms
            auto start = std::chrono::high_resolution_clock::now();
            while (std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::high_resolution_clock::now() - start).count() < 100) {
                // Busy wait
            }
        }
        
        // Update connection status
        std::cout << "[AP] Final connection status: " << static_cast<int>(_connectionStatus) << " (";
        switch(_connectionStatus) {
            case APConnectionStatus::Disconnected: std::cout << "Disconnected"; break;
            case APConnectionStatus::Connected: std::cout << "Connected"; break;
            case APConnectionStatus::Authenticated: std::cout << "Authenticated"; break;
            case APConnectionStatus::ConnectionRefused: std::cout << "ConnectionRefused"; break;
        }
        std::cout << ")" << std::endl;
        
        return _connectionStatus != APConnectionStatus::Disconnected;
    }
    catch (const std::exception& e) {
        std::cout << "[AP] Exception during connection: " << e.what() << std::endl;
        return false;
    }
    catch (...) {
        std::cout << "[AP] Unknown exception during connection" << std::endl;
        return false;
    }
}

/**
 * Disconnect from Archipelago server
 */
void ArchipelagoClient::disconnect()
{
    if (_initialized)
    {
        // APCpp doesn't have a specific disconnect function
        // We need to shutdown and reinitialize if we want to reconnect
        _connectionStatus = APConnectionStatus::Disconnected;
    }
}

/**
 * Shutdown and reset the library state
 */
void ArchipelagoClient::shutdown()
{
    if (_initialized)
    {
        AP_Shutdown();
        _initialized = false;
        _connectionStatus = APConnectionStatus::Disconnected;
    }
}

/**
 * Check if the client is initialized
 * @return true if initialized
 */
bool ArchipelagoClient::isInitialized() const
{
    return _initialized;
}

/**
 * Get current connection status
 * @return connection status
 */
APConnectionStatus ArchipelagoClient::getConnectionStatus() const
{
    return _connectionStatus;
}

/**
 * Get connection information
 * @return connection info
 */
const APConnectionInfo& ArchipelagoClient::getConnectionInfo() const
{
    return _connectionInfo;
}

/**
 * Get player ID from server
 * @return player ID
 */
int ArchipelagoClient::getPlayerId() const
{
    if (_initialized && _connectionStatus == APConnectionStatus::Authenticated)
    {
        return AP_GetPlayerID();
    }
    return _connectionInfo.playerId;
}

/**
 * Send location check to server
 * @param locationId Location ID to check
 */
void ArchipelagoClient::sendLocationCheck(int64_t locationId)
{
    if (_initialized && _connectionStatus == APConnectionStatus::Authenticated)
    {
        AP_SendItem(locationId);
    }
}

/**
 * Send multiple location checks to server
 * @param locationIds Set of location IDs to check
 */
void ArchipelagoClient::sendLocationChecks(const std::set<int64_t>& locationIds)
{
    if (_initialized && _connectionStatus == APConnectionStatus::Authenticated)
    {
        AP_SendItem(locationIds);
    }
}

/**
 * Set callback for when items need to be cleared
 * @param callback Callback function
 */
void ArchipelagoClient::setItemClearCallback(std::function<void()> callback)
{
    _itemClearCallback = callback;
}

/**
 * Set callback for when items are received
 * @param callback Callback function
 */
void ArchipelagoClient::setItemRecvCallback(std::function<void(int64_t, bool)> callback)
{
    _itemRecvCallback = callback;
}

/**
 * Set callback for when locations are checked
 * @param callback Callback function
 */
void ArchipelagoClient::setLocationCheckedCallback(std::function<void(int64_t)> callback)
{
    _locationCheckedCallback = callback;
}

/**
 * Set callback for when location info is received
 * @param callback Callback function
 */
void ArchipelagoClient::setLocationInfoCallback(std::function<void(std::vector<AP_NetworkItem>)> callback)
{
    _locationInfoCallback = callback;
}

/**
 * Check if there are pending messages
 * @return true if messages pending
 */
bool ArchipelagoClient::isMessagePending() const
{
    if (_initialized)
    {
        return AP_IsMessagePending();
    }
    return false;
}

/**
 * Get the latest message
 * @return pointer to latest message
 */
AP_Message* ArchipelagoClient::getLatestMessage()
{
    if (_initialized)
    {
        return AP_GetLatestMessage();
    }
    return nullptr;
}

/**
 * Clear the latest message
 */
void ArchipelagoClient::clearLatestMessage()
{
	if (_initialized)
	{
		AP_ClearLatestMessage();
	}
}

/**
 * Update the client (should be called regularly)
 */
void ArchipelagoClient::update()
{
    if (_initialized)
    {
        // Update connection status
        _connectionStatus = AP_GetConnectionStatus();
        
        // Update player ID if authenticated
        if (_connectionStatus == APConnectionStatus::Authenticated)
        {
            _connectionInfo.playerId = AP_GetPlayerID();
        }
    }
}

/**
 * Get error message for connection errors
 * @param error Error code
 * @return error message string
 */
std::string ArchipelagoClient::getErrorMessage(APConnectionError error)
{
    switch (error)
    {
        case AP_ERROR_NONE:
            return "No error";
        case AP_ERROR_NO_SERVER:
            return "Please enter a server URL";
        case AP_ERROR_NO_SLOT:
            return "Please enter a slot name";
        case AP_ERROR_INVALID_PORT:
            return "Invalid port number";
        case AP_ERROR_CONNECTION_FAILED:
            return "Connection failed";
        case AP_ERROR_AUTHENTICATION_FAILED:
            return "Authentication failed";
        case AP_ERROR_CONNECTION_REFUSED:
            return "Connection refused";
        case AP_ERROR_NETWORK_ERROR:
            return "Network error";
        default:
            return "Unknown error";
    }
}

/**
 * Internal callback handlers for APCpp
 */
void ArchipelagoClient::onItemClear()
{
    if (_itemClearCallback)
        _itemClearCallback();
}

void ArchipelagoClient::onItemReceived(int64_t itemId, bool notify)
{
    if (_itemRecvCallback)
        _itemRecvCallback(itemId, notify);
}

void ArchipelagoClient::onLocationChecked(int64_t locationId)
{
    if (_locationCheckedCallback)
        _locationCheckedCallback(locationId);
}

/**
 * Validate connection parameters
 * @param info Connection info to validate
 * @return error code
 */
APConnectionError ArchipelagoClient::validateConnection(const APConnectionInfo& info) const
{
    if (info.serverUrl.empty())
        return AP_ERROR_NO_SERVER;
    
    if (info.slotName.empty())
        return AP_ERROR_NO_SLOT;
    
    return AP_ERROR_NONE;
}

/**
 * Parse server URL and extract host/port
 * @param url Server URL
 * @param host Output host
 * @param port Output port
 * @return true if parsing successful
 */
bool ArchipelagoClient::parseServerUrl(const std::string& url, std::string& host, int& port) const
{
    size_t colonPos = url.find(':');
    if (colonPos != std::string::npos)
    {
        host = url.substr(0, colonPos);
        std::string portStr = url.substr(colonPos + 1);
        try
        {
            port = std::stoi(portStr);
            std::cout << "[AP] URL parsing - Host: '" << host << "', Port: " << port << std::endl;
            return port > 0 && port <= 65535;
        }
        catch (...)
        {
            std::cout << "[AP] Failed to parse port from: " << portStr << std::endl;
            return false;
        }
    }
    else
    {
        host = url;
        port = 38281; // Default Archipelago port
        std::cout << "[AP] No port specified, using default - Host: '" << host << "', Port: " << port << std::endl;
        return true;
    }
}

}