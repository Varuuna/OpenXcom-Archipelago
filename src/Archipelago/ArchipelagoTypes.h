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

#include <string>
#include <cstdint>
#include "../APCpp/Archipelago.h"

namespace OpenXcom
{

// Use the APCpp connection status directly
using APConnectionStatus = AP_ConnectionStatus;
// Available states: Disconnected, Connected, Authenticated, ConnectionRefused

/**
 * Structure containing connection information for Archipelago server
 */
struct APConnectionInfo {
    std::string serverUrl;
    std::string slotName;
    std::string password;
    int playerId;
    
    APConnectionInfo() : playerId(-1) {}
};

/**
 * Structure representing a research item received from Archipelago
 */
struct APResearchItem {
    int64_t itemId;
    std::string itemName;
    bool received;
    
    APResearchItem() : itemId(0), received(false) {}
    APResearchItem(int64_t id, const std::string& name) 
        : itemId(id), itemName(name), received(false) {}
};

/**
 * Structure representing a research location that can be checked in Archipelago
 */
struct APResearchLocation {
    int64_t locationId;
    std::string locationName;
    std::string researchName;
    bool checked;
    
    APResearchLocation() : locationId(0), checked(false) {}
    APResearchLocation(int64_t id, const std::string& locName, const std::string& resName)
        : locationId(id), locationName(locName), researchName(resName), checked(false) {}
};

/**
 * Enum for Archipelago connection errors
 */
enum APConnectionError {
    AP_ERROR_NONE = 0,
    AP_ERROR_NO_SERVER,
    AP_ERROR_NO_SLOT,
    AP_ERROR_INVALID_PORT,
    AP_ERROR_CONNECTION_FAILED,
    AP_ERROR_AUTHENTICATION_FAILED,
    AP_ERROR_CONNECTION_REFUSED,
    AP_ERROR_NETWORK_ERROR
};

/**
 * Structure for Archipelago world configuration
 * Based on OpenXcomAPWorld items and locations
 */
struct APWorldConfig {
    // Item IDs from OpenXcomAPWorld
    static const int64_t ITEM_LASER_WEAPONS = 1;
    static const int64_t ITEM_MEDI_KIT = 2;
    static const int64_t ITEM_MOTION_SCANNER = 3;
    
    // Location IDs from OpenXcomAPWorld
    static const int64_t LOCATION_LASER_WEAPONS = 1;
    static const int64_t LOCATION_MEDI_KIT = 2;
    static const int64_t LOCATION_MOTION_SCANNER = 3;
    
    // Research name mappings
    static const char* RESEARCH_LASER_WEAPONS;
    static const char* RESEARCH_MEDI_KIT;
    static const char* RESEARCH_MOTION_SCANNER;
};

}