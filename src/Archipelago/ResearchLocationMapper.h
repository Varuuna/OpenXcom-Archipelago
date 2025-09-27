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
#include <string>
#include <vector>
#include <map>

namespace OpenXcom
{

class Mod;

/**
 * Maps OpenXcom research projects to Archipelago locations and items.
 * This class handles the bidirectional mapping between OpenXcom research names
 * and Archipelago location/item IDs for synchronization.
 */
class ResearchLocationMapper
{
private:
	// Base location ID offset for OpenXcom research
	static const int64_t BASE_LOCATION_ID = 1000000;
	static const int64_t BASE_ITEM_ID = 1000000;
	
	// Mapping tables
	std::map<std::string, int64_t> _researchToLocation;
	std::map<int64_t, std::string> _locationToResearch;
	std::map<std::string, int64_t> _researchToItem;
	std::map<int64_t, std::string> _itemToResearch;
	
	// Display names for locations/items
	std::map<int64_t, std::string> _locationDisplayNames;
	std::map<int64_t, std::string> _itemDisplayNames;
	
	// Configuration
	bool _initialized;
	
	// Helper methods
	void initializeDefaultMappings();
	void loadCustomMappings();
	void saveCustomMappings();
	std::string sanitizeResearchName(const std::string& name);
	
public:
	/// Creates a new research location mapper
	ResearchLocationMapper();
	/// Destructor
	~ResearchLocationMapper();
	
	/// Initializes the mapper with mod data
	void initialize(const Mod* mod);
	/// Checks if the mapper is initialized
	bool isInitialized() const;
	
	/// Gets the location ID for a research project
	int64_t getLocationId(const std::string& researchName);
	/// Gets the research name for a location ID
	std::string getResearchName(int64_t locationId);
	
	/// Gets the item ID for a research project
	int64_t getItemId(const std::string& researchName);
	/// Gets the research name for an item ID
	std::string getResearchFromItem(int64_t itemId);
	
	/// Gets the display name for a location
	std::string getLocationDisplayName(int64_t locationId);
	/// Gets the display name for an item
	std::string getItemDisplayName(int64_t itemId);
	
	/// Gets all mapped research projects
	std::vector<std::string> getAllMappedResearch();
	/// Gets all location IDs
	std::vector<int64_t> getAllLocationIds();
	
	/// Adds a custom mapping
	void addCustomMapping(const std::string& researchName, int64_t locationId, int64_t itemId, const std::string& displayName = "");
	/// Removes a custom mapping
	void removeCustomMapping(const std::string& researchName);
	
	/// Checks if a research project is mapped
	bool isMapped(const std::string& researchName);
	/// Gets the number of mapped research projects
	size_t getMappingCount() const;
	
	/// Exports mappings to a file
	bool exportMappings(const std::string& filename);
	/// Imports mappings from a file
	bool importMappings(const std::string& filename);
};

}