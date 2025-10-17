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

#include "ResearchLocationMapper.h"

namespace OpenXcom
{

/**
 * Creates the research location mapper
 */
ResearchLocationMapper::ResearchLocationMapper()
{
    initialize();
}

/**
 * Initialize the mappings based on AP world configuration
 */
void ResearchLocationMapper::initialize()
{
    initializeDefaultMappings();
}

/**
 * Get location ID for a research topic
 * @param researchName Research topic name
 * @return location ID or 0 if not found
 */
int64_t ResearchLocationMapper::getLocationId(const std::string& researchName) const
{
    auto it = _researchToLocation.find(researchName);
    return (it != _researchToLocation.end()) ? it->second.locationId : 0;
}

/**
 * Get research name for a location ID
 * @param locationId Location ID
 * @return research name or empty string if not found
 */
std::string ResearchLocationMapper::getResearchName(int64_t locationId) const
{
    auto it = _locationToResearch.find(locationId);
    return (it != _locationToResearch.end()) ? it->second : "";
}

/**
 * Get research name for an item ID
 * @param itemId Item ID
 * @return research name or empty string if not found
 */
std::string ResearchLocationMapper::getResearchFromItemId(int64_t itemId) const
{
    auto it = _itemIdToResearch.find(itemId);
    return (it != _itemIdToResearch.end()) ? it->second.itemName : "";
}

/**
 * Check if a research topic is mapped to AP
 * @param researchName Research topic name
 * @return true if mapped
 */
bool ResearchLocationMapper::isResearchMapped(const std::string& researchName) const
{
    return _researchToLocation.find(researchName) != _researchToLocation.end();
}

/**
 * Check if a location ID is valid
 * @param locationId Location ID
 * @return true if valid
 */
bool ResearchLocationMapper::isLocationValid(int64_t locationId) const
{
    return _locationToResearch.find(locationId) != _locationToResearch.end();
}

/**
 * Check if an item ID is valid
 * @param itemId Item ID
 * @return true if valid
 */
bool ResearchLocationMapper::isItemValid(int64_t itemId) const
{
    return _itemIdToResearch.find(itemId) != _itemIdToResearch.end();
}

/**
 * Get all mapped research locations
 * @return map of research locations
 */
const std::map<std::string, APResearchLocation>& ResearchLocationMapper::getAllLocations() const
{
    return _researchToLocation;
}

/**
 * Get all mapped research items
 * @return map of research items
 */
const std::map<int64_t, APResearchItem>& ResearchLocationMapper::getAllItems() const
{
    return _itemIdToResearch;
}

/**
 * Create location object for research
 * @param researchName Research name
 * @return location object
 */
APResearchLocation ResearchLocationMapper::createLocation(const std::string& researchName) const
{
    auto it = _researchToLocation.find(researchName);
    if (it != _researchToLocation.end())
    {
        return it->second;
    }
    return APResearchLocation();
}

/**
 * Create item object for research
 * @param itemId Item ID
 * @return item object
 */
APResearchItem ResearchLocationMapper::createItem(int64_t itemId) const
{
    auto it = _itemIdToResearch.find(itemId);
    if (it != _itemIdToResearch.end())
    {
        return it->second;
    }
    return APResearchItem();
}

/**
 * Add a research mapping
 * @param researchName Research name
 * @param locationId Location ID
 * @param itemId Item ID
 */
void ResearchLocationMapper::addResearchMapping(const std::string& researchName, int64_t locationId, int64_t itemId)
{
    // Create location mapping
    APResearchLocation location(locationId, researchName + " Location", researchName);
    _researchToLocation[researchName] = location;
    _locationToResearch[locationId] = researchName;
    
    // Create item mapping
    APResearchItem item(itemId, researchName);
    _itemIdToResearch[itemId] = item;
}

/**
 * Initialize default mappings from AP world configuration
 */
void ResearchLocationMapper::initializeDefaultMappings()
{
    // Map all 70 research topics to their corresponding locations and items
    // Note: Research names must match OpenXcom's internal research IDs
    
    addResearchMapping("STR_ALIEN_ALLOYS", APWorldConfig::LOCATION_ALIEN_ALLOYS, 0);
    addResearchMapping("STR_ALIEN_ENTERTAINMENT", APWorldConfig::LOCATION_ALIEN_ENTERTAINMENT, 0);
    addResearchMapping("STR_ALIEN_FOOD", APWorldConfig::LOCATION_ALIEN_FOOD, 0);
    addResearchMapping("STR_ALIEN_GRENADE", APWorldConfig::LOCATION_ALIEN_GRENADE, APWorldConfig::ITEM_ALIEN_GRENADE);
    addResearchMapping("STR_ALIEN_ORIGINS", APWorldConfig::LOCATION_ALIEN_ORIGINS, 0);
    addResearchMapping("STR_ALIEN_SURGERY", APWorldConfig::LOCATION_ALIEN_SURGERY, 0);
    addResearchMapping("STR_BLASTER_BOMB", APWorldConfig::LOCATION_BLASTER_BOMB, APWorldConfig::ITEM_BLASTER_BOMB);
    addResearchMapping("STR_BLASTER_LAUNCHER", APWorldConfig::LOCATION_BLASTER_LAUNCHER, APWorldConfig::ITEM_BLASTER_LAUNCHER);
    addResearchMapping("STR_CELATID_CORPSE", APWorldConfig::LOCATION_CELATID_CORPSE, 0);
    addResearchMapping("STR_CELATID_TERRORIST", APWorldConfig::LOCATION_CELATID_TERRORIST, 0);
    addResearchMapping("STR_CHRYSSALID_CORPSE", APWorldConfig::LOCATION_CHRYSSALID_CORPSE, 0);
    addResearchMapping("STR_CHRYSSALID_TERRORIST", APWorldConfig::LOCATION_CHRYSSALID_TERRORIST, 0);
    addResearchMapping("STR_CYBERDISC_CORPSE", APWorldConfig::LOCATION_CYBERDISC_CORPSE, 0);
    addResearchMapping("STR_CYDONIA_OR_BUST", APWorldConfig::LOCATION_CYDONIA_OR_BUST, 0);
    addResearchMapping("STR_ELERIUM_115", APWorldConfig::LOCATION_ELERIUM_115, APWorldConfig::ITEM_ELERIUM_115);
    addResearchMapping("STR_ETHEREAL", APWorldConfig::LOCATION_ETHEREAL, 0);
    addResearchMapping("STR_EXAMINATION_ROOM", APWorldConfig::LOCATION_EXAMINATION_ROOM, 0);
    addResearchMapping("STR_FLOATER", APWorldConfig::LOCATION_FLOATER, 0);
    addResearchMapping("STR_FLOATER_CORPSE", APWorldConfig::LOCATION_FLOATER_CORPSE, 0);
    addResearchMapping("STR_FLYING_SUIT", APWorldConfig::LOCATION_FLYING_SUIT, APWorldConfig::ITEM_FLYING_SUIT);
    addResearchMapping("STR_FUSION_BALL", APWorldConfig::LOCATION_FUSION_BALL, APWorldConfig::ITEM_FUSION_BALL);
    addResearchMapping("STR_FUSION_BALL_LAUNCHER", APWorldConfig::LOCATION_FUSION_BALL_LAUNCHER, APWorldConfig::ITEM_FUSION_BALL_LAUNCHER);
    addResearchMapping("STR_FUSION_DEFENSE", APWorldConfig::LOCATION_FUSION_DEFENCES, APWorldConfig::ITEM_FUSION_BALL_DEFENCES);
    addResearchMapping("STR_GRAV_SHIELD", APWorldConfig::LOCATION_GRAV_SHIELD, APWorldConfig::ITEM_GRAV_SHIELD);
    addResearchMapping("STR_HEAVY_LASER", APWorldConfig::LOCATION_HEAVY_LASER, APWorldConfig::ITEM_HEAVY_LASER);
    addResearchMapping("STR_HEAVY_PLASMA", APWorldConfig::LOCATION_HEAVY_PLASMA, APWorldConfig::ITEM_HEAVY_PLASMA);
    addResearchMapping("STR_HEAVY_PLASMA_CLIP", APWorldConfig::LOCATION_HEAVY_PLASMA_CLIP, APWorldConfig::ITEM_HEAVY_PLASMA_CLIP);
    addResearchMapping("STR_HOVERTANK_LAUNCHER", APWorldConfig::LOCATION_HOVERTANK_LAUNCHER, APWorldConfig::ITEM_HOVERTANK_LAUNCHER);
    addResearchMapping("STR_HOVERTANK_PLASMA", APWorldConfig::LOCATION_HOVERTANK_PLASMA, APWorldConfig::ITEM_HOVERTANK_PLASMA);
    addResearchMapping("STR_HYPER_WAVE_DECODER", APWorldConfig::LOCATION_HYPER_WAVE_DECODER, APWorldConfig::ITEM_HYPER_WAVE_DECODER);
    addResearchMapping("STR_LASER_CANNON", APWorldConfig::LOCATION_LASER_CANNON, APWorldConfig::ITEM_LASER_CANNON);
    addResearchMapping("STR_LASER_DEFENSE", APWorldConfig::LOCATION_LASER_DEFENCES, APWorldConfig::ITEM_LASER_DEFENCES);
    addResearchMapping("STR_LASER_PISTOL", APWorldConfig::LOCATION_LASER_PISTOL, APWorldConfig::ITEM_LASER_PISTOL);
    addResearchMapping("STR_LASER_RIFLE", APWorldConfig::LOCATION_LASER_RIFLE, APWorldConfig::ITEM_LASER_RIFLE);
    addResearchMapping("STR_LASER_WEAPONS", APWorldConfig::LOCATION_LASER_WEAPONS, 0);
    addResearchMapping("STR_MEDI_KIT", APWorldConfig::LOCATION_MEDI_KIT, APWorldConfig::ITEM_MEDI_KIT);
    addResearchMapping("STR_MIND_PROBE", APWorldConfig::LOCATION_MIND_PROBE, APWorldConfig::ITEM_MIND_PROBE);
    addResearchMapping("STR_MIND_SHIELD", APWorldConfig::LOCATION_MIND_SHIELD, APWorldConfig::ITEM_MIND_SHIELD);
    addResearchMapping("STR_MOTION_SCANNER", APWorldConfig::LOCATION_MOTION_SCANNER, APWorldConfig::ITEM_MOTION_SCANNER);
    addResearchMapping("STR_MUTON", APWorldConfig::LOCATION_MUTON, 0);
    addResearchMapping("STR_MUTON_CORPSE", APWorldConfig::LOCATION_MUTON_CORPSE, 0);
    addResearchMapping("STR_NEW_FIGHTER_CRAFT", APWorldConfig::LOCATION_NEW_FIGHTER_CRAFT, APWorldConfig::ITEM_FIRESTORM);
    addResearchMapping("STR_NEW_FIGHTER_TRANSPORTER", APWorldConfig::LOCATION_NEW_FIGHTER_TRANSPORTER, APWorldConfig::ITEM_LIGHTNING);
    addResearchMapping("STR_PERSONAL_ARMOR", APWorldConfig::LOCATION_PERSONAL_ARMOR, APWorldConfig::ITEM_PERSONAL_ARMOR);
    addResearchMapping("STR_PLASMA_CANNON", APWorldConfig::LOCATION_PLASMA_CANNON, APWorldConfig::ITEM_PLASMA_BEAM);
    addResearchMapping("STR_PLASMA_DEFENSE", APWorldConfig::LOCATION_PLASMA_DEFENCES, APWorldConfig::ITEM_PLASMA_DEFENCES);
    addResearchMapping("STR_PLASMA_PISTOL", APWorldConfig::LOCATION_PLASMA_PISTOL, APWorldConfig::ITEM_PLASMA_PISTOL);
    addResearchMapping("STR_PLASMA_PISTOL_CLIP", APWorldConfig::LOCATION_PLASMA_PISTOL_CLIP, APWorldConfig::ITEM_PLASMA_PISTOL_CLIP);
    addResearchMapping("STR_PLASMA_RIFLE", APWorldConfig::LOCATION_PLASMA_RIFLE, APWorldConfig::ITEM_PLASMA_RIFLE);
    addResearchMapping("STR_PLASMA_RIFLE_CLIP", APWorldConfig::LOCATION_PLASMA_RIFLE_CLIP, APWorldConfig::ITEM_PLASMA_RIFLE_CLIP);
    addResearchMapping("STR_POWER_SUIT", APWorldConfig::LOCATION_POWER_SUIT, APWorldConfig::ITEM_POWER_SUIT);
    addResearchMapping("STR_PSI_LAB", APWorldConfig::LOCATION_PSI_LAB, 0);
    addResearchMapping("STR_PSI_AMP", APWorldConfig::LOCATION_PSI_AMP, APWorldConfig::ITEM_PSI_AMP);
    addResearchMapping("STR_REAPER_CORPSE", APWorldConfig::LOCATION_REAPER_CORPSE, 0);
    addResearchMapping("STR_REAPER_TERRORIST", APWorldConfig::LOCATION_REAPER_TERRORIST, 0);
    addResearchMapping("STR_SECTOID", APWorldConfig::LOCATION_SECTOID, 0);
    addResearchMapping("STR_SECTOID_CORPSE", APWorldConfig::LOCATION_SECTOID_CORPSE, 0);
    addResearchMapping("STR_SECTOPOD_CORPSE", APWorldConfig::LOCATION_SECTOPOD_CORPSE, 0);
    addResearchMapping("STR_SILACOID_CORPSE", APWorldConfig::LOCATION_SILACOID_CORPSE, 0);
    addResearchMapping("STR_SILACOID_TERRORIST", APWorldConfig::LOCATION_SILACOID_TERRORIST, 0);
    addResearchMapping("STR_SMALL_LAUNCHER", APWorldConfig::LOCATION_SMALL_LAUNCHER, APWorldConfig::ITEM_SMALL_LAUNCHER);
    addResearchMapping("STR_SNAKEMAN", APWorldConfig::LOCATION_SNAKEMAN, 0);
    addResearchMapping("STR_SNAKEMAN_CORPSE", APWorldConfig::LOCATION_SNAKEMAN_CORPSE, 0);
    addResearchMapping("STR_STUN_BOMB", APWorldConfig::LOCATION_STUN_BOMB, 0);
    addResearchMapping("STR_TANK_LASER_CANNON", APWorldConfig::LOCATION_TANK_LASER_CANNON, 0);
    addResearchMapping("STR_THE_MARTIAN_SOLUTION", APWorldConfig::LOCATION_THE_MARTIAN_SOLUTION, 0);
    addResearchMapping("STR_UFO_CONSTRUCTION", APWorldConfig::LOCATION_UFO_CONSTRUCTION, 0);
    addResearchMapping("STR_UFO_NAVIGATION", APWorldConfig::LOCATION_UFO_NAVIGATION, 0);
    addResearchMapping("STR_UFO_POWER_SOURCE", APWorldConfig::LOCATION_UFO_POWER_SOURCE, 0);
    addResearchMapping("STR_ULTIMATE_CRAFT", APWorldConfig::LOCATION_ULTIMATE_CRAFT, APWorldConfig::ITEM_AVENGER);
}

}