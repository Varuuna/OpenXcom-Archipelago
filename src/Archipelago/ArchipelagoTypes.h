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
    static const int64_t ITEM_AC_AP = 1;
    static const int64_t ITEM_AC_HE = 2;
    static const int64_t ITEM_AC_I = 3;
    static const int64_t ITEM_ACCESS_LIFT = 4;
    static const int64_t ITEM_ALIEN_CONTAINMENT = 5;
    static const int64_t ITEM_ALIEN_GRENADE = 6;
    static const int64_t ITEM_AUTO_CANNON = 7;
    static const int64_t ITEM_AVALANCHE_LAUNCHER = 8;
    static const int64_t ITEM_AVALANCHE_MISSILE = 9;
    static const int64_t ITEM_AVENGER = 10;
    static const int64_t ITEM_BLASTER_BOMB = 11;
    static const int64_t ITEM_BLASTER_LAUNCHER = 12;
    static const int64_t ITEM_CANNON = 13;
    static const int64_t ITEM_ELECTRO_FLARE = 14;
    static const int64_t ITEM_ELERIUM_115 = 15;
    static const int64_t ITEM_FIRESTORM = 16;
    static const int64_t ITEM_FLYING_SUIT = 17;
    static const int64_t ITEM_FUSION_BALL = 18;
    static const int64_t ITEM_FUSION_BALL_DEFENCES = 19;
    static const int64_t ITEM_FUSION_BALL_LAUNCHER = 20;
    static const int64_t ITEM_GENERAL_STORES = 21;
    static const int64_t ITEM_GRAV_SHIELD = 22;
    static const int64_t ITEM_GRENADE = 23;
    static const int64_t ITEM_HANGAR = 24;
    static const int64_t ITEM_HC_AP = 25;
    static const int64_t ITEM_HC_HE = 26;
    static const int64_t ITEM_HC_I = 27;
    static const int64_t ITEM_HEAVY_CANNON = 28;
    static const int64_t ITEM_HEAVY_LASER = 29;
    static const int64_t ITEM_HEAVY_PLASMA = 30;
    static const int64_t ITEM_HEAVY_PLASMA_CLIP = 31;
    static const int64_t ITEM_HIGH_EXPLOSIVES = 32;
    static const int64_t ITEM_HOVERTANK_LAUNCHER = 33;
    static const int64_t ITEM_HOVERTANK_PLASMA = 34;
    static const int64_t ITEM_HYPER_WAVE_DECODER = 35;
    static const int64_t ITEM_INCENDIARY_ROCKET = 36;
    static const int64_t ITEM_INTERCEPTOR = 37;
    static const int64_t ITEM_LABORATORY = 38;
    static const int64_t ITEM_LARGE_RADAR_SYSTEM = 39;
    static const int64_t ITEM_LARGE_ROCKET = 40;
    static const int64_t ITEM_LASER_CANNON = 41;
    static const int64_t ITEM_LASER_DEFENCES = 42;
    static const int64_t ITEM_LASER_PISTOL = 43;
    static const int64_t ITEM_LASER_RIFLE = 44;
    static const int64_t ITEM_LIGHTNING = 45;
    static const int64_t ITEM_LIVING_QUARTERS = 46;
    static const int64_t ITEM_MEDI_KIT = 47;
    static const int64_t ITEM_MIND_PROBE = 48;
    static const int64_t ITEM_MIND_SHIELD = 49;
    static const int64_t ITEM_MISSILE_DEFENCES = 50;
    static const int64_t ITEM_MOTION_SCANNER = 51;
    static const int64_t ITEM_PERSONAL_ARMOR = 52;
    static const int64_t ITEM_PISTOL = 53;
    static const int64_t ITEM_PISTOL_CLIP = 54;
    static const int64_t ITEM_PLASMA_BEAM = 55;
    static const int64_t ITEM_PLASMA_DEFENCES = 56;
    static const int64_t ITEM_PLASMA_PISTOL = 57;
    static const int64_t ITEM_PLASMA_PISTOL_CLIP = 58;
    static const int64_t ITEM_PLASMA_RIFLE = 59;
    static const int64_t ITEM_PLASMA_RIFLE_CLIP = 60;
    static const int64_t ITEM_POWER_SUIT = 61;
    static const int64_t ITEM_PROXIMITY_GRENADE = 62;
    static const int64_t ITEM_PSI_AMP = 63;
    static const int64_t ITEM_RIFLE = 64;
    static const int64_t ITEM_RIFLE_CLIP = 65;
    static const int64_t ITEM_ROCKET_LAUNCHER = 66;
    static const int64_t ITEM_SKYRANGER = 67;
    static const int64_t ITEM_SMALL_LAUNCHER = 68;
    static const int64_t ITEM_SMALL_RADAR_SYSTEM = 69;
    static const int64_t ITEM_SMALL_ROCKET = 70;
    
    // Location IDs from OpenXcomAPWorld
    static const int64_t LOCATION_ALIEN_ALLOYS = 1;
    static const int64_t LOCATION_ALIEN_ENTERTAINMENT = 2;
    static const int64_t LOCATION_ALIEN_FOOD = 3;
    static const int64_t LOCATION_ALIEN_GRENADE = 4;
    static const int64_t LOCATION_ALIEN_ORIGINS = 5;
    static const int64_t LOCATION_ALIEN_SURGERY = 6;
    static const int64_t LOCATION_BLASTER_BOMB = 7;
    static const int64_t LOCATION_BLASTER_LAUNCHER = 8;
    static const int64_t LOCATION_CELATID_CORPSE = 9;
    static const int64_t LOCATION_CELATID_TERRORIST = 10;
    static const int64_t LOCATION_CHRYSSALID_CORPSE = 11;
    static const int64_t LOCATION_CHRYSSALID_TERRORIST = 12;
    static const int64_t LOCATION_CYBERDISC_CORPSE = 13;
    static const int64_t LOCATION_CYDONIA_OR_BUST = 14;
    static const int64_t LOCATION_ELERIUM_115 = 15;
    static const int64_t LOCATION_ETHEREAL = 16;
    static const int64_t LOCATION_EXAMINATION_ROOM = 17;
    static const int64_t LOCATION_FLOATER = 18;
    static const int64_t LOCATION_FLOATER_CORPSE = 19;
    static const int64_t LOCATION_FLYING_SUIT = 20;
    static const int64_t LOCATION_FUSION_BALL = 21;
    static const int64_t LOCATION_FUSION_BALL_LAUNCHER = 22;
    static const int64_t LOCATION_FUSION_DEFENCES = 23;
    static const int64_t LOCATION_GRAV_SHIELD = 24;
    static const int64_t LOCATION_HEAVY_LASER = 25;
    static const int64_t LOCATION_HEAVY_PLASMA = 26;
    static const int64_t LOCATION_HEAVY_PLASMA_CLIP = 27;
    static const int64_t LOCATION_HOVERTANK_LAUNCHER = 28;
    static const int64_t LOCATION_HOVERTANK_PLASMA = 29;
    static const int64_t LOCATION_HYPER_WAVE_DECODER = 30;
    static const int64_t LOCATION_LASER_CANNON = 31;
    static const int64_t LOCATION_LASER_DEFENCES = 32;
    static const int64_t LOCATION_LASER_PISTOL = 33;
    static const int64_t LOCATION_LASER_RIFLE = 34;
    static const int64_t LOCATION_LASER_WEAPONS = 35;
    static const int64_t LOCATION_MEDI_KIT = 36;
    static const int64_t LOCATION_MIND_PROBE = 37;
    static const int64_t LOCATION_MIND_SHIELD = 38;
    static const int64_t LOCATION_MOTION_SCANNER = 39;
    static const int64_t LOCATION_MUTON = 40;
    static const int64_t LOCATION_MUTON_CORPSE = 41;
    static const int64_t LOCATION_NEW_FIGHTER_CRAFT = 42;
    static const int64_t LOCATION_NEW_FIGHTER_TRANSPORTER = 43;
    static const int64_t LOCATION_PERSONAL_ARMOR = 44;
    static const int64_t LOCATION_PLASMA_CANNON = 45;
    static const int64_t LOCATION_PLASMA_DEFENCES = 46;
    static const int64_t LOCATION_PLASMA_PISTOL = 47;
    static const int64_t LOCATION_PLASMA_PISTOL_CLIP = 48;
    static const int64_t LOCATION_PLASMA_RIFLE = 49;
    static const int64_t LOCATION_PLASMA_RIFLE_CLIP = 50;
    static const int64_t LOCATION_POWER_SUIT = 51;
    static const int64_t LOCATION_PSI_LAB = 52;
    static const int64_t LOCATION_PSI_AMP = 53;
    static const int64_t LOCATION_REAPER_CORPSE = 54;
    static const int64_t LOCATION_REAPER_TERRORIST = 55;
    static const int64_t LOCATION_SECTOID = 56;
    static const int64_t LOCATION_SECTOID_CORPSE = 57;
    static const int64_t LOCATION_SECTOPOD_CORPSE = 58;
    static const int64_t LOCATION_SILACOID_CORPSE = 59;
    static const int64_t LOCATION_SILACOID_TERRORIST = 60;
    static const int64_t LOCATION_SMALL_LAUNCHER = 61;
    static const int64_t LOCATION_SNAKEMAN = 62;
    static const int64_t LOCATION_SNAKEMAN_CORPSE = 63;
    static const int64_t LOCATION_STUN_BOMB = 64;
    static const int64_t LOCATION_TANK_LASER_CANNON = 65;
    static const int64_t LOCATION_THE_MARTIAN_SOLUTION = 66;
    static const int64_t LOCATION_UFO_CONSTRUCTION = 67;
    static const int64_t LOCATION_UFO_NAVIGATION = 68;
    static const int64_t LOCATION_UFO_POWER_SOURCE = 69;
    static const int64_t LOCATION_ULTIMATE_CRAFT = 70;

};

}