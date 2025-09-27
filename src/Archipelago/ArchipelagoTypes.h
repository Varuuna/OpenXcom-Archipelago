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
#include <vector>
#include <map>
#include <functional>
#include <stdint.h>

namespace OpenXcom
{

/**
 * Archipelago connection states
 */
enum ArchipelagoConnectionState
{
	AP_DISCONNECTED = 0,
	AP_CONNECTING,
	AP_CONNECTED,
	AP_AUTHENTICATED,
	AP_ERROR
};

/**
 * Archipelago message types based on the protocol specification
 */
enum ArchipelagoMessageType
{
	AP_MSG_ROOM_INFO = 0,
	AP_MSG_CONNECTION_REFUSED,
	AP_MSG_CONNECTED,
	AP_MSG_RECEIVED_ITEMS,
	AP_MSG_LOCATION_INFO,
	AP_MSG_ROOM_UPDATE,
	AP_MSG_PRINT,
	AP_MSG_PRINT_JSON,
	AP_MSG_DATA_PACKAGE,
	AP_MSG_BOUNCED,
	AP_MSG_INVALID_PACKET,
	AP_MSG_RETRIEVE,
	AP_MSG_RETRIEVED,
	AP_MSG_SET_NOTIFY,
	AP_MSG_SET_REPLY,
	AP_MSG_CONNECT = 20,
	AP_MSG_CONNECT_UPDATE,
	AP_MSG_LOCATION_CHECKS,
	AP_MSG_LOCATION_SCOUTS,
	AP_MSG_STATUS_UPDATE,
	AP_MSG_SAY,
	AP_MSG_GET_DATA_PACKAGE,
	AP_MSG_BOUNCE,
	AP_MSG_GET,
	AP_MSG_SET
};

/**
 * Archipelago item flags
 */
enum ArchipelagoItemFlags
{
	AP_ITEM_ADVANCEMENT = 1,
	AP_ITEM_NEVER_EXCLUDE = 2,
	AP_ITEM_PROGRESSION = 4,
	AP_ITEM_USEFUL = 8,
	AP_ITEM_TRAP = 16,
	AP_ITEM_FILLER = 32
};

/**
 * Archipelago location flags
 */
enum ArchipelagoLocationFlags
{
	AP_LOCATION_NONE = 0,
	AP_LOCATION_PROGRESSION = 1,
	AP_LOCATION_USEFUL = 2,
	AP_LOCATION_FILLER = 4,
	AP_LOCATION_TRAP = 8
};

/**
 * Structure representing an Archipelago item
 */
struct ArchipelagoItem
{
	int64_t item;
	int location;
	int player;
	int flags;
	
	ArchipelagoItem() : item(0), location(0), player(0), flags(0) {}
	ArchipelagoItem(int64_t i, int l, int p, int f) : item(i), location(l), player(p), flags(f) {}
};

/**
 * Structure representing an Archipelago location check
 */
struct ArchipelagoLocationCheck
{
	int64_t location;
	
	ArchipelagoLocationCheck() : location(0) {}
	ArchipelagoLocationCheck(int64_t l) : location(l) {}
};

/**
 * Structure for Archipelago connection configuration
 */
struct ArchipelagoConfig
{
	std::string hostname;
	int port;
	std::string slot_name;
	std::string password;
	std::string game;
	std::vector<std::string> tags;
	int version_major;
	int version_minor;
	int version_build;
	bool items_handling;
	
	// WebSocket compression settings
	bool enable_compression;
	int max_window_bits;
	bool no_context_takeover;
	
	ArchipelagoConfig() :
		hostname("archipelago.gg"),
		port(38281),
		game("OpenXcom"),
		version_major(0),
		version_minor(6),
		version_build(4),
		items_handling(true),
		enable_compression(true),
		max_window_bits(15),
		no_context_takeover(true)  // Safer default for compatibility
	{
		tags.push_back("AP");
	}
};

/**
 * Structure for research to location mapping
 */
struct ResearchLocationMapping
{
	std::string research_name;
	int64_t location_id;
	std::string display_name;
	
	ResearchLocationMapping() : location_id(0) {}
	ResearchLocationMapping(const std::string& name, int64_t id, const std::string& display) :
		research_name(name), location_id(id), display_name(display) {}
};

/**
 * Callback function types for Archipelago events
 */
typedef std::function<void(ArchipelagoConnectionState)> ArchipelagoConnectionCallback;
typedef std::function<void(const ArchipelagoItem&)> ArchipelagoItemReceivedCallback;
typedef std::function<void(const std::string&)> ArchipelagoMessageCallback;

}