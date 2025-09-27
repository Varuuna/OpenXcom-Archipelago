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
    // Add mappings for the three research items from OpenXcomAPWorld
    addResearchMapping(APWorldConfig::RESEARCH_LASER_WEAPONS, 
                      APWorldConfig::LOCATION_LASER_WEAPONS, 
                      APWorldConfig::ITEM_LASER_WEAPONS);
    
    addResearchMapping(APWorldConfig::RESEARCH_MEDI_KIT, 
                      APWorldConfig::LOCATION_MEDI_KIT, 
                      APWorldConfig::ITEM_MEDI_KIT);
    
    addResearchMapping(APWorldConfig::RESEARCH_MOTION_SCANNER, 
                      APWorldConfig::LOCATION_MOTION_SCANNER, 
                      APWorldConfig::ITEM_MOTION_SCANNER);
}

}