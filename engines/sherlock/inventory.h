/* ScummVM - Graphic Adventure Engine
 *
 * ScummVM is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the COPYRIGHT
 * file distributed with this source distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 */

#ifndef SHERLOCK_INVENTORY_H
#define SHERLOCK_INVENTORY_H

#include "common/scummsys.h"
#include "common/array.h"
#include "common/serializer.h"
#include "common/str-array.h"
#include "sherlock/objects.h"
#include "sherlock/resources.h"

namespace Sherlock {

#define MAX_VISIBLE_INVENTORY 6

enum InvMode {
	INVMODE_EXIT = 0,
	INVMODE_LOOK = 1,
	INVMODE_USE = 2,
	INVMODE_GIVE = 3,
	INVMODE_FIRST = 4,
	INVMODE_PREVIOUS = 5,
	INVMODE_NEXT = 6,
	INVMODE_LAST = 7,
	INVMODE_INVALID = 8,
	INVMODE_USE55 = 255
};

enum InvNewMode {
	PLAIN_INVENTORY = 0, LOOK_INVENTORY_MODE = 1, USE_INVENTORY_MODE = 2,
	GIVE_INVENTORY_MODE = 3, INVENTORY_DONT_DISPLAY = 128
};

enum InvSlamMode { SLAM_DONT_DISPLAY, SLAM_DISPLAY = 1, SLAM_SECONDARY_BUFFER };


struct InventoryItem {
	int _requiredFlag;
	Common::String _name;
	Common::String _description;
	Common::String _examine;
	int _lookFlag;

	InventoryItem() : _requiredFlag(0), _lookFlag(0) {}
	InventoryItem(int requiredFlag, const Common::String &name,
		const Common::String &description, const Common::String &examine);

	/**
	 * Synchronize the data for an inventory item
	 */
	void synchronize(Common::Serializer &s);
};

class Inventory : public Common::Array<InventoryItem> {
private:
	SherlockEngine *_vm;
	Common::StringArray _names;

	/**
	 * Copy the passed object into the inventory
	 */
	void copyToInventory(Object &obj);
public:
	ImageFile *_invShapes[MAX_VISIBLE_INVENTORY];
	bool _invGraphicsLoaded;
	InvMode _invMode;
	int _invIndex;
	int _holdings;		// Used to hold number of visible items in active inventory.
						// Since Inventory array also contains some special hidden items
	/**
	 * Free any loaded inventory graphics
	 */
	void freeGraphics();
public:
	Inventory(SherlockEngine *vm);
	~Inventory();

	/**
	 * Free inventory data
	 */
	void freeInv();

	/**
	 * Load the list of names the inventory items correspond to, if not already loaded,
	 * and then calls loadGraphics to load the associated graphics
	 */
	void loadInv();

	/**
	 * Load the list of names of graphics for the inventory
	 */
	void loadGraphics();

	/**
	 * Searches through the list of names that correspond to the inventory items
	 * and returns the number that matches the passed name
	 */
	int findInv(const Common::String &name);

	/**
	 * Display the character's inventory. The slamIt parameter specifies:
	 */
	void putInv(InvSlamMode slamIt);

	/**
	 * Put the game into inventory mode and open the interface window.
	 */
	void drawInventory(InvNewMode flag);

	/**
	 * Prints the line of inventory commands at the top of an inventory window with
	 * the correct highlighting
	 */
	void invCommands(bool slamIt);

	/**
	 * Set the highlighting color of a given inventory item
	 */
	void highlight(int index, byte color);

	/**
	 * Support method for refreshing the display of the inventory
	 */
	void refreshInv();

	/**
	 * Adds a shape from the scene to the player's inventory
	 */
	int putNameInInventory(const Common::String &name);

	/**
	 * Moves a specified item into the player's inventory If the item has a *PICKUP* use action,
	 * then the item in the use action are added to the inventory.
	 */
	int putItemInInventory(Object &obj);

	/**
	 * Deletes a specified item from the player's inventory
	 */
	int deleteItemFromInventory(const Common::String &name);

	/**
	 * Synchronize the data for a savegame
	 */
	void synchronize(Common::Serializer &s);
};

} // End of namespace Sherlock

#endif
