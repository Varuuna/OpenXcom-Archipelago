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
#include "../Mod/Mod.h"
#include "../Mod/RuleResearch.h"
#include "../Engine/Logger.h"
#include "../Engine/CrossPlatform.h"
#include "../Engine/Options.h"
#include <fstream>
#include <algorithm>
#include <sstream>

namespace OpenXcom
{

/**
 * Creates a new research location mapper.
 */
ResearchLocationMapper::ResearchLocationMapper() : _initialized(false)
{
}

/**
 * Destructor.
 */
ResearchLocationMapper::~ResearchLocationMapper()
{
}

/**
 * Initializes the mapper with mod data.
 * @param mod Mod containing research data
 */
void ResearchLocationMapper::initialize(const Mod* mod)
{
	if (_initialized)
		return;
	
	Log(LOG_INFO) << "ResearchLocationMapper: Initializing with mod data";
	
	// Clear existing mappings
	_researchToLocation.clear();
	_locationToResearch.clear();
	_researchToItem.clear();
	_itemToResearch.clear();
	_locationDisplayNames.clear();
	_itemDisplayNames.clear();
	
	// Initialize default mappings
	initializeDefaultMappings();
	
	// Load research projects from mod
	if (mod)
	{
		const std::vector<std::string>& researchList = mod->getResearchList();
		int64_t currentLocationId = BASE_LOCATION_ID;
		int64_t currentItemId = BASE_ITEM_ID;
		
		for (const std::string& researchName : researchList)
		{
			const RuleResearch* research = mod->getResearch(researchName);
			if (research && research->getCost() > 0) // Only map research that costs time
			{
				// Skip if already mapped by default mappings
				if (_researchToLocation.find(researchName) != _researchToLocation.end())
					continue;
				
				// Create mapping
				_researchToLocation[researchName] = currentLocationId;
				_locationToResearch[currentLocationId] = researchName;
				_researchToItem[researchName] = currentItemId;
				_itemToResearch[currentItemId] = researchName;
				
				// Create display names (remove STR_ prefix and convert underscores)
				std::string displayName = researchName;
				if (displayName.substr(0, 4) == "STR_")
					displayName = displayName.substr(4);
				
				std::replace(displayName.begin(), displayName.end(), '_', ' ');
				
				_locationDisplayNames[currentLocationId] = displayName;
				_itemDisplayNames[currentItemId] = displayName;
				
				currentLocationId++;
				currentItemId++;
			}
		}
	}
	
	// Load custom mappings from file
	loadCustomMappings();
	
	_initialized = true;
	
	Log(LOG_INFO) << "ResearchLocationMapper: Initialized with " << _researchToLocation.size() << " research mappings";
}

/**
 * Checks if the mapper is initialized.
 * @return True if initialized
 */
bool ResearchLocationMapper::isInitialized() const
{
	return _initialized;
}

/**
 * Gets the location ID for a research project.
 * @param researchName Research project name
 * @return Location ID or 0 if not found
 */
int64_t ResearchLocationMapper::getLocationId(const std::string& researchName)
{
	auto it = _researchToLocation.find(researchName);
	return (it != _researchToLocation.end()) ? it->second : 0;
}

/**
 * Gets the research name for a location ID.
 * @param locationId Location ID
 * @return Research name or empty string if not found
 */
std::string ResearchLocationMapper::getResearchName(int64_t locationId)
{
	auto it = _locationToResearch.find(locationId);
	return (it != _locationToResearch.end()) ? it->second : "";
}

/**
 * Gets the item ID for a research project.
 * @param researchName Research project name
 * @return Item ID or 0 if not found
 */
int64_t ResearchLocationMapper::getItemId(const std::string& researchName)
{
	auto it = _researchToItem.find(researchName);
	return (it != _researchToItem.end()) ? it->second : 0;
}

/**
 * Gets the research name for an item ID.
 * @param itemId Item ID
 * @return Research name or empty string if not found
 */
std::string ResearchLocationMapper::getResearchFromItem(int64_t itemId)
{
	auto it = _itemToResearch.find(itemId);
	return (it != _itemToResearch.end()) ? it->second : "";
}

/**
 * Gets the display name for a location.
 * @param locationId Location ID
 * @return Display name or empty string if not found
 */
std::string ResearchLocationMapper::getLocationDisplayName(int64_t locationId)
{
	auto it = _locationDisplayNames.find(locationId);
	return (it != _locationDisplayNames.end()) ? it->second : "";
}

/**
 * Gets the display name for an item.
 * @param itemId Item ID
 * @return Display name or empty string if not found
 */
std::string ResearchLocationMapper::getItemDisplayName(int64_t itemId)
{
	auto it = _itemDisplayNames.find(itemId);
	return (it != _itemDisplayNames.end()) ? it->second : "";
}

/**
 * Gets all mapped research projects.
 * @return Vector of research names
 */
std::vector<std::string> ResearchLocationMapper::getAllMappedResearch()
{
	std::vector<std::string> result;
	result.reserve(_researchToLocation.size());
	
	for (const auto& pair : _researchToLocation)
	{
		result.push_back(pair.first);
	}
	
	return result;
}

/**
 * Gets all location IDs.
 * @return Vector of location IDs
 */
std::vector<int64_t> ResearchLocationMapper::getAllLocationIds()
{
	std::vector<int64_t> result;
	result.reserve(_locationToResearch.size());
	
	for (const auto& pair : _locationToResearch)
	{
		result.push_back(pair.first);
	}
	
	return result;
}

/**
 * Adds a custom mapping.
 * @param researchName Research project name
 * @param locationId Location ID
 * @param itemId Item ID
 * @param displayName Display name (optional)
 */
void ResearchLocationMapper::addCustomMapping(const std::string& researchName, int64_t locationId, int64_t itemId, const std::string& displayName)
{
	// Remove existing mapping if present
	removeCustomMapping(researchName);
	
	// Add new mapping
	_researchToLocation[researchName] = locationId;
	_locationToResearch[locationId] = researchName;
	_researchToItem[researchName] = itemId;
	_itemToResearch[itemId] = researchName;
	
	std::string display = displayName.empty() ? researchName : displayName;
	_locationDisplayNames[locationId] = display;
	_itemDisplayNames[itemId] = display;
	
	// Save to file
	saveCustomMappings();
	
	Log(LOG_INFO) << "ResearchLocationMapper: Added custom mapping for " << researchName;
}

/**
 * Removes a custom mapping.
 * @param researchName Research project name
 */
void ResearchLocationMapper::removeCustomMapping(const std::string& researchName)
{
	auto locIt = _researchToLocation.find(researchName);
	if (locIt != _researchToLocation.end())
	{
		int64_t locationId = locIt->second;
		_locationToResearch.erase(locationId);
		_locationDisplayNames.erase(locationId);
		_researchToLocation.erase(locIt);
	}
	
	auto itemIt = _researchToItem.find(researchName);
	if (itemIt != _researchToItem.end())
	{
		int64_t itemId = itemIt->second;
		_itemToResearch.erase(itemId);
		_itemDisplayNames.erase(itemId);
		_researchToItem.erase(itemIt);
	}
	
	// Save to file
	saveCustomMappings();
}

/**
 * Checks if a research project is mapped.
 * @param researchName Research project name
 * @return True if mapped
 */
bool ResearchLocationMapper::isMapped(const std::string& researchName)
{
	return _researchToLocation.find(researchName) != _researchToLocation.end();
}

/**
 * Gets the number of mapped research projects.
 * @return Number of mappings
 */
size_t ResearchLocationMapper::getMappingCount() const
{
	return _researchToLocation.size();
}

/**
 * Exports mappings to a file.
 * @param filename Output filename
 * @return True if successful
 */
bool ResearchLocationMapper::exportMappings(const std::string& filename)
{
	try
	{
		std::ofstream file(filename);
		if (!file.is_open())
			return false;
		
		file << "# OpenXcom Archipelago Research Mappings\n";
		file << "# Format: research_name,location_id,item_id,display_name\n\n";
		
		for (const auto& pair : _researchToLocation)
		{
			const std::string& researchName = pair.first;
			int64_t locationId = pair.second;
			int64_t itemId = getItemId(researchName);
			std::string displayName = getLocationDisplayName(locationId);
			
			file << researchName << "," << locationId << "," << itemId << "," << displayName << "\n";
		}
		
		file.close();
		return true;
	}
	catch (const std::exception& e)
	{
		Log(LOG_ERROR) << "ResearchLocationMapper: Failed to export mappings: " << e.what();
		return false;
	}
}

/**
 * Imports mappings from a file.
 * @param filename Input filename
 * @return True if successful
 */
bool ResearchLocationMapper::importMappings(const std::string& filename)
{
	try
	{
		std::ifstream file(filename);
		if (!file.is_open())
			return false;
		
		std::string line;
		while (std::getline(file, line))
		{
			// Skip comments and empty lines
			if (line.empty() || line[0] == '#')
				continue;
			
			// Parse CSV line
			std::istringstream ss(line);
			std::string researchName, locationIdStr, itemIdStr, displayName;
			
			if (std::getline(ss, researchName, ',') &&
			    std::getline(ss, locationIdStr, ',') &&
			    std::getline(ss, itemIdStr, ',') &&
			    std::getline(ss, displayName))
			{
				try
				{
					int64_t locationId = std::stoll(locationIdStr);
					int64_t itemId = std::stoll(itemIdStr);
					addCustomMapping(researchName, locationId, itemId, displayName);
				}
				catch (const std::exception& e)
				{
					Log(LOG_WARNING) << "ResearchLocationMapper: Failed to parse line: " << line;
				}
			}
		}
		
		file.close();
		return true;
	}
	catch (const std::exception& e)
	{
		Log(LOG_ERROR) << "ResearchLocationMapper: Failed to import mappings: " << e.what();
		return false;
	}
}

/**
 * Initializes default mappings for common research projects.
 */
void ResearchLocationMapper::initializeDefaultMappings()
{
	// Add some common vanilla research mappings with specific IDs
	// These can be customized or extended as needed
	
	struct DefaultMapping
	{
		std::string research;
		int64_t locationId;
		int64_t itemId;
		std::string displayName;
	};
	
	std::vector<DefaultMapping> defaults = {
		{"STR_LASER_WEAPONS", BASE_LOCATION_ID + 1, BASE_ITEM_ID + 1, "Laser Weapons"},
		{"STR_PLASMA_RIFLE", BASE_LOCATION_ID + 2, BASE_ITEM_ID + 2, "Plasma Rifle"},
		{"STR_HEAVY_PLASMA", BASE_LOCATION_ID + 3, BASE_ITEM_ID + 3, "Heavy Plasma"},
		{"STR_PLASMA_CANNON", BASE_LOCATION_ID + 4, BASE_ITEM_ID + 4, "Plasma Cannon"},
		{"STR_FUSION_MISSILE", BASE_LOCATION_ID + 5, BASE_ITEM_ID + 5, "Fusion Missile"},
		{"STR_BLASTER_LAUNCHER", BASE_LOCATION_ID + 6, BASE_ITEM_ID + 6, "Blaster Launcher"},
		{"STR_SMALL_LAUNCHER", BASE_LOCATION_ID + 7, BASE_ITEM_ID + 7, "Small Launcher"},
		{"STR_ALIEN_GRENADE", BASE_LOCATION_ID + 8, BASE_ITEM_ID + 8, "Alien Grenade"},
		{"STR_MIND_PROBE", BASE_LOCATION_ID + 9, BASE_ITEM_ID + 9, "Mind Probe"},
		{"STR_PSI_AMP", BASE_LOCATION_ID + 10, BASE_ITEM_ID + 10, "Psi Amp"},
		{"STR_PERSONAL_ARMOR", BASE_LOCATION_ID + 11, BASE_ITEM_ID + 11, "Personal Armor"},
		{"STR_POWER_SUIT", BASE_LOCATION_ID + 12, BASE_ITEM_ID + 12, "Power Suit"},
		{"STR_FLYING_SUIT", BASE_LOCATION_ID + 13, BASE_ITEM_ID + 13, "Flying Suit"},
		{"STR_SKYRANGER", BASE_LOCATION_ID + 14, BASE_ITEM_ID + 14, "Skyranger"},
		{"STR_LIGHTNING", BASE_LOCATION_ID + 15, BASE_ITEM_ID + 15, "Lightning"},
		{"STR_AVENGER", BASE_LOCATION_ID + 16, BASE_ITEM_ID + 16, "Avenger"},
		{"STR_FIRESTORM", BASE_LOCATION_ID + 17, BASE_ITEM_ID + 17, "Firestorm"},
		{"STR_INTERCEPTOR", BASE_LOCATION_ID + 18, BASE_ITEM_ID + 18, "Interceptor"},
		{"STR_ALIEN_ORIGINS", BASE_LOCATION_ID + 19, BASE_ITEM_ID + 19, "Alien Origins"},
		{"STR_THE_MARTIAN_SOLUTION", BASE_LOCATION_ID + 20, BASE_ITEM_ID + 20, "The Martian Solution"}
	};
	
	for (const auto& mapping : defaults)
	{
		_researchToLocation[mapping.research] = mapping.locationId;
		_locationToResearch[mapping.locationId] = mapping.research;
		_researchToItem[mapping.research] = mapping.itemId;
		_itemToResearch[mapping.itemId] = mapping.research;
		_locationDisplayNames[mapping.locationId] = mapping.displayName;
		_itemDisplayNames[mapping.itemId] = mapping.displayName;
	}
}

/**
 * Loads custom mappings from file.
 */
void ResearchLocationMapper::loadCustomMappings()
{
	std::string filename = Options::getMasterUserFolder() + "archipelago_mappings.csv";
	if (CrossPlatform::fileExists(filename))
	{
		importMappings(filename);
	}
}

/**
 * Saves custom mappings to file.
 */
void ResearchLocationMapper::saveCustomMappings()
{
	std::string filename = Options::getMasterUserFolder() + "archipelago_mappings.csv";
	exportMappings(filename);
}

/**
 * Sanitizes a research name for display.
 * @param name Research name
 * @return Sanitized name
 */
std::string ResearchLocationMapper::sanitizeResearchName(const std::string& name)
{
	std::string result = name;
	
	// Remove STR_ prefix
	if (result.substr(0, 4) == "STR_")
		result = result.substr(4);
	
	// Replace underscores with spaces
	std::replace(result.begin(), result.end(), '_', ' ');
	
	// Capitalize first letter of each word
	bool capitalizeNext = true;
	for (char& c : result)
	{
		if (capitalizeNext && std::isalpha(c))
		{
			c = std::toupper(c);
			capitalizeNext = false;
		}
		else if (c == ' ')
		{
			capitalizeNext = true;
		}
		else
		{
			c = std::tolower(c);
		}
	}
	
	return result;
}

}