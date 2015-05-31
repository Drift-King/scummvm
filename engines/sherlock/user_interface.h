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

#ifndef SHERLOCK_UI_H
#define SHERLOCK_UI_H

#include "common/scummsys.h"
#include "common/events.h"
#include "sherlock/surface.h"
#include "sherlock/objects.h"
#include "sherlock/resources.h"

namespace Sherlock {

#define CONTROLS_Y  138
#define CONTROLS_Y1 151

enum MenuMode {
	STD_MODE		=  0,
	LOOK_MODE		=  1,
	MOVE_MODE		=  2,
	TALK_MODE		=  3,
	PICKUP_MODE		=  4,
	OPEN_MODE		=  5,
	CLOSE_MODE		=  6,
	INV_MODE		=  7,
	USE_MODE		=  8,
	GIVE_MODE		=  9,
	JOURNAL_MODE	= 10,
	FILES_MODE		= 11,
	SETUP_MODE		= 12
};

class UserInterface {
protected:
	SherlockEngine *_vm;

	UserInterface(SherlockEngine *vm);
public:
	MenuMode _menuMode;
	int _menuCounter;
	bool _infoFlag;
	bool _windowOpen;
	bool _endKeyActive;
	int _invLookFlag;
	bool _slideWindows;
	bool _helpStyle;
	Common::Rect _windowBounds;
	bool _lookScriptFlag;

	// TODO: Not so sure these should be in the base class. May want to refactor them to SherlockEngine, or refactor
	// various Scalpel dialogs to keep their own private state of key/selections
	char _key, _oldKey;
	int _selector, _oldSelector;
	int _temp, _oldTemp;
	int _temp1;
	int _lookHelp;
public:
	static UserInterface *init(SherlockEngine *vm);
	virtual ~UserInterface() {}

	/**
	 * Resets the user interface
	 */
	virtual void reset() {}

	/**
	 * Draw the user interface onto the screen's back buffers
	 */	
	virtual void drawInterface(int bufferNum = 3) {}

	/**
	 * Main input handler for the user interface
	 */
	virtual void handleInput() {}

	/**
	 * Displays a passed window by gradually scrolling it vertically on-screen
	 */
	virtual void summonWindow(const Surface &bgSurface, bool slideUp = true) {}

	/**
	 * Slide the window stored in the back buffer onto the screen
	 */
	virtual void summonWindow(bool slideUp = true, int height = CONTROLS_Y) {}

	/**
	 * Close a currently open window
	 * @param flag	0 = slide old window down, 1 = slide prior UI back up
	 */
	virtual void banishWindow(bool slideUp = true) {}

	/**
	 * Clears the info line of the screen
	 */
	virtual void clearInfo() {}

	/**
	 * Clear any active text window
	 */
	virtual void clearWindow() {}

	/**
	 * Print the previously selected object's decription
	 */
	virtual void printObjectDesc() {}

	/**
	 * Initializes scroll variables
	 */
	virtual void initScrollVars() {}
};

} // End of namespace Sherlock

#endif
