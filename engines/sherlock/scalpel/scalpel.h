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

#ifndef SHERLOCK_SCALPEL_H
#define SHERLOCK_SCALPEL_H

#include "sherlock/sherlock.h"
#include "sherlock/scalpel/darts.h"

namespace Sherlock {

namespace Scalpel {

enum { BLACKWOOD_CAPTURE = 2, BAKER_STREET = 4, DRAWING_ROOM = 12, STATION = 17, PUB_INTERIOR = 19,
	LAWYER_OFFICE = 27, BAKER_ST_EXTERIOR = 39, RESCUE_ANNA = 52, MOOREHEAD_DEATH = 53, EXIT_GAME = 55,
	BRUMWELL_SUICIDE = 70, OVERHEAD_MAP2 = 98, DARTS_GAME = 99, OVERHEAD_MAP = 100 };

class ScalpelEngine : public SherlockEngine {
private:
	Darts *_darts;
	int _mapResult;

	/**
	 * Show the starting city cutscene which shows the game title
	 */
	bool showCityCutscene();

	/**
	 * Show the back alley where the initial murder takes place
	 */
	bool showAlleyCutscene();

	/**
	 * Show the Baker Street outside cutscene
	 */
	bool showStreetCutscene();

	/**
	 * Show Holmes and Watson at the breakfast table, lestrade's note, and then the scrolling credits
	 */
	bool showOfficeCutscene();

	/**
	 * Show the game credits
	 */
	bool scrollCredits();

	/**
	 * Load the default inventory for the game, which includes both the initial active inventory,
	 * as well as special pending inventory items which can appear automatically in the player's
	 * inventory once given required flags are set
	 */
	void loadInventory();

	/**
	 * Transition to show an image
	 */
	void showLBV(const Common::String &filename);
protected:
	/**
	 * Game initialization
	 */
	virtual void initialize();

	/**
	 * Show the opening sequence
	 */
	virtual void showOpening();

	/**
	 * Starting a scene within the game
	 */
	virtual void startScene();
public:
	ScalpelEngine(OSystem *syst, const SherlockGameDescription *gameDesc);
	virtual ~ScalpelEngine();

	/**
	 * Takes care of clearing the mirror in scene 12 (mansion drawing room), in case anything drew over it
	 */
	void eraseMirror12();

	/**
	 * Takes care of drawing Holme's reflection onto the mirror in scene 12 (mansion drawing room)
	 */
	void doMirror12();

	/**
	 * This clears the mirror in scene 12 (mansion drawing room) in case anything messed draw over it
	 */
	void flushMirror12();
};

} // End of namespace Scalpel

} // End of namespace Sherlock

#endif
