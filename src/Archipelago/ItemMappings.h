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
#include <cstdint>

namespace OpenXcom
{

/**
 * Type of unlockable item in Archipelago
 */
enum class APItemType
{
	Research,  // Research topic
	Item       // Physical item (weapon, armor, etc.)
};

/**
 * Information about an unlockable item
 */
struct APItemInfo
{
	std::string name;      // OpenXcom internal name (e.g., "STR_LASER_RIFLE")
	APItemType type;       // Type of item (Research or Item)
	int64_t locationId;    // Archipelago location ID for this item
	
	APItemInfo() : type(APItemType::Research), locationId(0) {}
	APItemInfo(const std::string& n, APItemType t, int64_t loc) 
		: name(n), type(t), locationId(loc) {}
};

/**
 * Static mappings between OpenXcom items and Archipelago items/locations
 */
class ItemMappings
{
public:
	/**
	 * Initialize and return the item-to-location mapping
	 * Maps OpenXcom item names to their Archipelago location IDs
	 * @param outMap Map to populate with item name -> location ID mappings
	 */
	static void getItemToLocationMap(std::map<std::string, int64_t>& outMap);
	
	/**
	 * Initialize and return the received item mapping
	 * Maps Archipelago item IDs to OpenXcom item information
	 * @param outMap Map to populate with AP item ID -> item info mappings
	 */
	static void getReceivedItemMap(std::map<int64_t, APItemInfo>& outMap);
};

}