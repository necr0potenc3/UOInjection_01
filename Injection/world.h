////////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2002 mamaich
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
////////////////////////////////////////////////////////////////////////////////
//
//  Module Name:
// 
//     world.h
// 
//  Abstract:
// 
//     World handling classes
//     
//  Author:
// 
//     mamaich
//
//  Revision history:
//
//  [2/24/2002] - created
//
////////////////////////////////////////////////////////////////////////////////

#pragma once
#include "common.h"

class GameObject;

const uint16 INVALID_XY = 0xffff;

const int LAYER_NONE = 0;
const int LAYER_ONE_HANDED = 1;
const int LAYER_TWO_HANDED = 2;
const int LAYER_SHOES = 3;
const int LAYER_PANTS = 4;
const int LAYER_SHIRT = 5;
const int LAYER_HELM = 6;  // hat
const int LAYER_GLOVES = 7;
const int LAYER_RING = 8;
const int LAYER_9 = 9;      // unused
const int LAYER_NECK = 10;
const int LAYER_HAIR = 11;
const int LAYER_WAIST = 12; // half apron
const int LAYER_TORSO = 13; // chest armour
const int LAYER_BRACELET = 14;
const int LAYER_15 = 15;        // unused
const int LAYER_FACIAL_HAIR = 16;
const int LAYER_TUNIC = 17; // surcoat, tunic, full apron, sash
const int LAYER_EARRINGS = 18;
const int LAYER_ARMS = 19;
const int LAYER_CLOAK = 20;
const int LAYER_BACKPACK = 21;
const int LAYER_ROBE = 22;
const int LAYER_SKIRT = 23; // skirt, kilt
const int LAYER_LEGS = 24;  // leg armour
const int LAYER_MOUNT = 25; // horse, ostard, etc
const int LAYER_VENDOR_BUY_RESTOCK = 26;
const int LAYER_VENDOR_BUY = 27;
const int LAYER_VENDOR_SELL = 28;
const int LAYER_BANK = 29;

const int GRAPHIC_BANDAGES		= 0x0e21;
const int GRAPHIC_CURE_POTION	= 0x0f07;
const int GRAPHIC_MANA_POTION	= 0x0f09;
const int GRAPHIC_LAVA_POTION	= 0x0f0b;
const int GRAPHIC_HEAL_POTION	= 0x0f0c;
const int GRAPHIC_ARROWS		= 0x0f3f;
const int GRAPHIC_BLACKMOOR		= 0x0f79;
const int GRAPHIC_BLACK_PEARL	= 0x0f7a;
const int GRAPHIC_BLOOD_MOSS	= 0x0f7b;
const int GRAPHIC_FERTILE_DIRT	= 0x0f81;
const int GRAPHIC_GARLIC		= 0x0f84;
const int GRAPHIC_GINSENG		= 0x0f85;
const int GRAPHIC_MANDRAKE_ROOT	= 0x0f86;
const int GRAPHIC_EYE_OF_NEWT	= 0x0f87;
const int GRAPHIC_NIGHTSHADE	= 0x0f88;
const int GRAPHIC_SULFUROUS_ASH	= 0x0f8c;
const int GRAPHIC_SPIDERS_SILK	= 0x0f8d;
const int GRAPHIC_VOLCANIC_ASH	= 0x0f8f;
const int GRAPHIC_WYRMS_HEART	= 0x0f91;
const int GRAPHIC_BOLT			= 0x1bfb;

const int I_BACKPACK	= 0x0e75;
const int I_BAG			= 0x0e76;

void DumpWorldToLog();
void ForceCleanupWorld();
void CountItemsInContainer(uint32 Serial);
