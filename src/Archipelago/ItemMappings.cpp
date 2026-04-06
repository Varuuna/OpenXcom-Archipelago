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

#include "ItemMappings.h"

namespace OpenXcom
{

/**
 * Initialize and return the item-to-location mapping
 * Maps OpenXcom item names to their Archipelago location IDs
 * @param outMap Map to populate with item name -> location ID mappings
 */
void ItemMappings::getItemToLocationMap(std::map<std::string, int64_t>& outMap)
{
	// Map item names to location IDs (using OpenXcom internal IDs)
	outMap["STR_ALIEN_ALLOYS"] = APWorldConfig::LOCATION_ALIEN_ALLOYS;
	outMap["STR_ALIEN_ENTERTAINMENT"] = APWorldConfig::LOCATION_ALIEN_ENTERTAINMENT;
	outMap["STR_ALIEN_FOOD"] = APWorldConfig::LOCATION_ALIEN_FOOD;
	outMap["STR_ALIEN_GRENADE"] = APWorldConfig::LOCATION_ALIEN_GRENADE;
	outMap["STR_ALIEN_ORIGINS"] = APWorldConfig::LOCATION_ALIEN_ORIGINS;
	outMap["STR_ALIEN_SURGERY"] = APWorldConfig::LOCATION_ALIEN_SURGERY;
	outMap["STR_BLASTER_BOMB"] = APWorldConfig::LOCATION_BLASTER_BOMB;
	outMap["STR_BLASTER_LAUNCHER"] = APWorldConfig::LOCATION_BLASTER_LAUNCHER;
	outMap["STR_CELATID_CORPSE"] = APWorldConfig::LOCATION_CELATID_CORPSE;
	outMap["STR_CELATID_TERRORIST"] = APWorldConfig::LOCATION_CELATID_TERRORIST;
	outMap["STR_CHRYSSALID_CORPSE"] = APWorldConfig::LOCATION_CHRYSSALID_CORPSE;
	outMap["STR_CHRYSSALID_TERRORIST"] = APWorldConfig::LOCATION_CHRYSSALID_TERRORIST;
	outMap["STR_CYBERDISC_CORPSE"] = APWorldConfig::LOCATION_CYBERDISC_CORPSE;
	outMap["STR_CYDONIA_OR_BUST"] = APWorldConfig::LOCATION_CYDONIA_OR_BUST;
	outMap["STR_ELERIUM_115"] = APWorldConfig::LOCATION_ELERIUM_115;
	outMap["STR_ETHEREAL"] = APWorldConfig::LOCATION_ETHEREAL;
	outMap["STR_EXAMINATION_ROOM"] = APWorldConfig::LOCATION_EXAMINATION_ROOM;
	outMap["STR_FLOATER"] = APWorldConfig::LOCATION_FLOATER;
	outMap["STR_FLOATER_CORPSE"] = APWorldConfig::LOCATION_FLOATER_CORPSE;
	outMap["STR_FLYING_SUIT"] = APWorldConfig::LOCATION_FLYING_SUIT;
	outMap["STR_FUSION_BALL"] = APWorldConfig::LOCATION_FUSION_BALL;
	outMap["STR_FUSION_BALL_LAUNCHER"] = APWorldConfig::LOCATION_FUSION_BALL_LAUNCHER;
	outMap["STR_FUSION_DEFENSE"] = APWorldConfig::LOCATION_FUSION_DEFENCES;
	outMap["STR_GRAV_SHIELD"] = APWorldConfig::LOCATION_GRAV_SHIELD;
	outMap["STR_HEAVY_LASER"] = APWorldConfig::LOCATION_HEAVY_LASER;
	outMap["STR_HEAVY_PLASMA"] = APWorldConfig::LOCATION_HEAVY_PLASMA;
	outMap["STR_HEAVY_PLASMA_CLIP"] = APWorldConfig::LOCATION_HEAVY_PLASMA_CLIP;
	outMap["STR_HOVERTANK_LAUNCHER"] = APWorldConfig::LOCATION_HOVERTANK_LAUNCHER;
	outMap["STR_HOVERTANK_PLASMA"] = APWorldConfig::LOCATION_HOVERTANK_PLASMA;
	outMap["STR_HYPER_WAVE_DECODER"] = APWorldConfig::LOCATION_HYPER_WAVE_DECODER;
	outMap["STR_LASER_CANNON"] = APWorldConfig::LOCATION_LASER_CANNON;
	outMap["STR_LASER_DEFENSE"] = APWorldConfig::LOCATION_LASER_DEFENCES;
	outMap["STR_LASER_PISTOL"] = APWorldConfig::LOCATION_LASER_PISTOL;
	outMap["STR_LASER_RIFLE"] = APWorldConfig::LOCATION_LASER_RIFLE;
	outMap["STR_LASER_WEAPONS"] = APWorldConfig::LOCATION_LASER_WEAPONS;
	outMap["STR_MEDI_KIT"] = APWorldConfig::LOCATION_MEDI_KIT;
	outMap["STR_MIND_PROBE"] = APWorldConfig::LOCATION_MIND_PROBE;
	outMap["STR_MIND_SHIELD"] = APWorldConfig::LOCATION_MIND_SHIELD;
	outMap["STR_MOTION_SCANNER"] = APWorldConfig::LOCATION_MOTION_SCANNER;
	outMap["STR_MUTON"] = APWorldConfig::LOCATION_MUTON;
	outMap["STR_MUTON_CORPSE"] = APWorldConfig::LOCATION_MUTON_CORPSE;
	outMap["STR_NEW_FIGHTER_CRAFT"] = APWorldConfig::LOCATION_NEW_FIGHTER_CRAFT;
	outMap["STR_NEW_FIGHTER_TRANSPORTER"] = APWorldConfig::LOCATION_NEW_FIGHTER_TRANSPORTER;
	outMap["STR_PERSONAL_ARMOR"] = APWorldConfig::LOCATION_PERSONAL_ARMOR;
	outMap["STR_PLASMA_CANNON"] = APWorldConfig::LOCATION_PLASMA_CANNON;
	outMap["STR_PLASMA_DEFENSE"] = APWorldConfig::LOCATION_PLASMA_DEFENCES;
	outMap["STR_PLASMA_PISTOL"] = APWorldConfig::LOCATION_PLASMA_PISTOL;
	outMap["STR_PLASMA_PISTOL_CLIP"] = APWorldConfig::LOCATION_PLASMA_PISTOL_CLIP;
	outMap["STR_PLASMA_RIFLE"] = APWorldConfig::LOCATION_PLASMA_RIFLE;
	outMap["STR_PLASMA_RIFLE_CLIP"] = APWorldConfig::LOCATION_PLASMA_RIFLE_CLIP;
	outMap["STR_POWER_SUIT"] = APWorldConfig::LOCATION_POWER_SUIT;
	outMap["STR_PSI_LAB"] = APWorldConfig::LOCATION_PSI_LAB;
	outMap["STR_PSI_AMP"] = APWorldConfig::LOCATION_PSI_AMP;
	outMap["STR_REAPER_CORPSE"] = APWorldConfig::LOCATION_REAPER_CORPSE;
	outMap["STR_REAPER_TERRORIST"] = APWorldConfig::LOCATION_REAPER_TERRORIST;
	outMap["STR_SECTOID"] = APWorldConfig::LOCATION_SECTOID;
	outMap["STR_SECTOID_CORPSE"] = APWorldConfig::LOCATION_SECTOID_CORPSE;
	outMap["STR_SECTOPOD_CORPSE"] = APWorldConfig::LOCATION_SECTOPOD_CORPSE;
	outMap["STR_SILACOID_CORPSE"] = APWorldConfig::LOCATION_SILACOID_CORPSE;
	outMap["STR_SILACOID_TERRORIST"] = APWorldConfig::LOCATION_SILACOID_TERRORIST;
	outMap["STR_SMALL_LAUNCHER"] = APWorldConfig::LOCATION_SMALL_LAUNCHER;
	outMap["STR_SNAKEMAN"] = APWorldConfig::LOCATION_SNAKEMAN;
	outMap["STR_SNAKEMAN_CORPSE"] = APWorldConfig::LOCATION_SNAKEMAN_CORPSE;
	outMap["STR_STUN_BOMB"] = APWorldConfig::LOCATION_STUN_BOMB;
	outMap["STR_TANK_LASER_CANNON"] = APWorldConfig::LOCATION_TANK_LASER_CANNON;
	outMap["STR_THE_MARTIAN_SOLUTION"] = APWorldConfig::LOCATION_THE_MARTIAN_SOLUTION;
	outMap["STR_UFO_CONSTRUCTION"] = APWorldConfig::LOCATION_UFO_CONSTRUCTION;
	outMap["STR_UFO_NAVIGATION"] = APWorldConfig::LOCATION_UFO_NAVIGATION;
	outMap["STR_UFO_POWER_SOURCE"] = APWorldConfig::LOCATION_UFO_POWER_SOURCE;
	outMap["STR_ULTIMATE_CRAFT"] = APWorldConfig::LOCATION_ULTIMATE_CRAFT;
}

/**
 * Initialize and return the received item mapping
 * Maps Archipelago item IDs to OpenXcom item information
 * @param outMap Map to populate with AP item ID -> item info mappings
 */
void ItemMappings::getReceivedItemMap(std::map<int64_t, APItemInfo>& outMap)
{
	// Map AP item IDs to OpenXcom items (currently only research items)
	// All of these are Research type items
	outMap[APWorldConfig::ITEM_ALIEN_GRENADE] = APItemInfo("STR_ALIEN_GRENADE", APItemType::Research, APWorldConfig::LOCATION_ALIEN_GRENADE);
	outMap[APWorldConfig::ITEM_BLASTER_BOMB] = APItemInfo("STR_BLASTER_BOMB", APItemType::Research, APWorldConfig::LOCATION_BLASTER_BOMB);
	outMap[APWorldConfig::ITEM_BLASTER_LAUNCHER] = APItemInfo("STR_BLASTER_LAUNCHER", APItemType::Research, APWorldConfig::LOCATION_BLASTER_LAUNCHER);
	outMap[APWorldConfig::ITEM_ELERIUM_115] = APItemInfo("STR_ELERIUM_115", APItemType::Research, APWorldConfig::LOCATION_ELERIUM_115);
	outMap[APWorldConfig::ITEM_FLYING_SUIT] = APItemInfo("STR_FLYING_SUIT", APItemType::Research, APWorldConfig::LOCATION_FLYING_SUIT);
	outMap[APWorldConfig::ITEM_FUSION_BALL] = APItemInfo("STR_FUSION_BALL", APItemType::Research, APWorldConfig::LOCATION_FUSION_BALL);
	outMap[APWorldConfig::ITEM_FUSION_BALL_LAUNCHER] = APItemInfo("STR_FUSION_BALL_LAUNCHER", APItemType::Research, APWorldConfig::LOCATION_FUSION_BALL_LAUNCHER);
	outMap[APWorldConfig::ITEM_FUSION_BALL_DEFENCES] = APItemInfo("STR_FUSION_DEFENSE", APItemType::Research, APWorldConfig::LOCATION_FUSION_DEFENCES);
	outMap[APWorldConfig::ITEM_GRAV_SHIELD] = APItemInfo("STR_GRAV_SHIELD", APItemType::Research, APWorldConfig::LOCATION_GRAV_SHIELD);
	outMap[APWorldConfig::ITEM_HEAVY_LASER] = APItemInfo("STR_HEAVY_LASER", APItemType::Research, APWorldConfig::LOCATION_HEAVY_LASER);
	outMap[APWorldConfig::ITEM_HEAVY_PLASMA] = APItemInfo("STR_HEAVY_PLASMA", APItemType::Research, APWorldConfig::LOCATION_HEAVY_PLASMA);
	outMap[APWorldConfig::ITEM_HEAVY_PLASMA_CLIP] = APItemInfo("STR_HEAVY_PLASMA_CLIP", APItemType::Research, APWorldConfig::LOCATION_HEAVY_PLASMA_CLIP);
	outMap[APWorldConfig::ITEM_HOVERTANK_LAUNCHER] = APItemInfo("STR_HOVERTANK_LAUNCHER", APItemType::Research, APWorldConfig::LOCATION_HOVERTANK_LAUNCHER);
	outMap[APWorldConfig::ITEM_HOVERTANK_PLASMA] = APItemInfo("STR_HOVERTANK_PLASMA", APItemType::Research, APWorldConfig::LOCATION_HOVERTANK_PLASMA);
	outMap[APWorldConfig::ITEM_HYPER_WAVE_DECODER] = APItemInfo("STR_HYPER_WAVE_DECODER", APItemType::Research, APWorldConfig::LOCATION_HYPER_WAVE_DECODER);
	outMap[APWorldConfig::ITEM_LASER_CANNON] = APItemInfo("STR_LASER_CANNON", APItemType::Research, APWorldConfig::LOCATION_LASER_CANNON);
	outMap[APWorldConfig::ITEM_LASER_DEFENCES] = APItemInfo("STR_LASER_DEFENSE", APItemType::Research, APWorldConfig::LOCATION_LASER_DEFENCES);
	outMap[APWorldConfig::ITEM_LASER_PISTOL] = APItemInfo("STR_LASER_PISTOL", APItemType::Research, APWorldConfig::LOCATION_LASER_PISTOL);
	outMap[APWorldConfig::ITEM_LASER_RIFLE] = APItemInfo("STR_LASER_RIFLE", APItemType::Research, APWorldConfig::LOCATION_LASER_RIFLE);
	outMap[APWorldConfig::ITEM_MEDI_KIT] = APItemInfo("STR_MEDI_KIT", APItemType::Research, APWorldConfig::LOCATION_MEDI_KIT);
	outMap[APWorldConfig::ITEM_MIND_PROBE] = APItemInfo("STR_MIND_PROBE", APItemType::Research, APWorldConfig::LOCATION_MIND_PROBE);
	outMap[APWorldConfig::ITEM_MIND_SHIELD] = APItemInfo("STR_MIND_SHIELD", APItemType::Research, APWorldConfig::LOCATION_MIND_SHIELD);
	outMap[APWorldConfig::ITEM_MOTION_SCANNER] = APItemInfo("STR_MOTION_SCANNER", APItemType::Research, APWorldConfig::LOCATION_MOTION_SCANNER);
	outMap[APWorldConfig::ITEM_FIRESTORM] = APItemInfo("STR_NEW_FIGHTER_CRAFT", APItemType::Research, APWorldConfig::LOCATION_NEW_FIGHTER_CRAFT);
	outMap[APWorldConfig::ITEM_LIGHTNING] = APItemInfo("STR_NEW_FIGHTER_TRANSPORTER", APItemType::Research, APWorldConfig::LOCATION_NEW_FIGHTER_TRANSPORTER);
	outMap[APWorldConfig::ITEM_PERSONAL_ARMOR] = APItemInfo("STR_PERSONAL_ARMOR", APItemType::Research, APWorldConfig::LOCATION_PERSONAL_ARMOR);
	outMap[APWorldConfig::ITEM_PLASMA_BEAM] = APItemInfo("STR_PLASMA_CANNON", APItemType::Research, APWorldConfig::LOCATION_PLASMA_CANNON);
	outMap[APWorldConfig::ITEM_PLASMA_DEFENCES] = APItemInfo("STR_PLASMA_DEFENSE", APItemType::Research, APWorldConfig::LOCATION_PLASMA_DEFENCES);
	outMap[APWorldConfig::ITEM_PLASMA_PISTOL] = APItemInfo("STR_PLASMA_PISTOL", APItemType::Research, APWorldConfig::LOCATION_PLASMA_PISTOL);
	outMap[APWorldConfig::ITEM_PLASMA_PISTOL_CLIP] = APItemInfo("STR_PLASMA_PISTOL_CLIP", APItemType::Research, APWorldConfig::LOCATION_PLASMA_PISTOL_CLIP);
	outMap[APWorldConfig::ITEM_PLASMA_RIFLE] = APItemInfo("STR_PLASMA_RIFLE", APItemType::Research, APWorldConfig::LOCATION_PLASMA_RIFLE);
	outMap[APWorldConfig::ITEM_PLASMA_RIFLE_CLIP] = APItemInfo("STR_PLASMA_RIFLE_CLIP", APItemType::Research, APWorldConfig::LOCATION_PLASMA_RIFLE_CLIP);
	outMap[APWorldConfig::ITEM_POWER_SUIT] = APItemInfo("STR_POWER_SUIT", APItemType::Research, APWorldConfig::LOCATION_POWER_SUIT);
	outMap[APWorldConfig::ITEM_PSI_AMP] = APItemInfo("STR_PSI_AMP", APItemType::Research, APWorldConfig::LOCATION_PSI_AMP);
	outMap[APWorldConfig::ITEM_SMALL_LAUNCHER] = APItemInfo("STR_SMALL_LAUNCHER", APItemType::Research, APWorldConfig::LOCATION_SMALL_LAUNCHER);
	outMap[APWorldConfig::ITEM_AVENGER] = APItemInfo("STR_ULTIMATE_CRAFT", APItemType::Research, APWorldConfig::LOCATION_ULTIMATE_CRAFT);
}

}