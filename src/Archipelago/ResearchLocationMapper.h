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
#include <map>
#include <string>

namespace OpenXcom
{

class RuleResearch;

/**
 * Maps OpenXcom research topics to Archipelago location IDs
 * Handles research completion events and manages location checking logic
 */
class ResearchLocationMapper
{
private:
    std::map<std::string, APResearchLocation> _researchToLocation;
    std::map<int64_t, std::string> _locationToResearch;
    std::map<int64_t, APResearchItem> _itemIdToResearch;
    
public:
    /// Creates the research location mapper
    ResearchLocationMapper();
    
    /// Initialize the mappings based on AP world configuration
    void initialize();
    
    /// Get location ID for a research topic
    int64_t getLocationId(const std::string& researchName) const;
    
    /// Get research name for a location ID
    std::string getResearchName(int64_t locationId) const;
    
    /// Get research name for an item ID
    std::string getResearchFromItemId(int64_t itemId) const;
    
    /// Check if a research topic is mapped to AP
    bool isResearchMapped(const std::string& researchName) const;
    
    /// Check if a location ID is valid
    bool isLocationValid(int64_t locationId) const;
    
    /// Check if an item ID is valid
    bool isItemValid(int64_t itemId) const;
    
    /// Get all mapped research locations
    const std::map<std::string, APResearchLocation>& getAllLocations() const;
    
    /// Get all mapped research items
    const std::map<int64_t, APResearchItem>& getAllItems() const;
    
    /// Create location object for research
    APResearchLocation createLocation(const std::string& researchName) const;
    
    /// Create item object for research
    APResearchItem createItem(int64_t itemId) const;
    
private:
    /// Add a research mapping
    void addResearchMapping(const std::string& researchName, int64_t locationId, int64_t itemId);
    
    /// Initialize default mappings from AP world configuration
    void initializeDefaultMappings();
};

}