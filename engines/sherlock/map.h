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

#ifndef SHERLOCK_MAP_H
#define SHERLOCK_MAP_H

#include "common/scummsys.h"
#include "common/array.h"
#include "common/rect.h"
#include "common/serializer.h"
#include "common/str.h"
#include "common/str-array.h"
#include "sherlock/surface.h"
#include "sherlock/objects.h"

namespace Sherlock {

class SherlockEngine;

struct MapEntry : Common::Point {
	int _translate;

	MapEntry() : Common::Point(), _translate(-1) {}

	MapEntry(int posX, int posY, int translate) : Common::Point(posX, posY), _translate(translate) {}
};

class MapPaths {
private:
	int _numLocations;
	Common::Array< Common::Array<byte> > _paths;
public:
	void load(int numLocations, Common::SeekableReadStream &s);

	const byte *getPath(int srcLocation, int destLocation);
};

class Map {
private:
	SherlockEngine *_vm;
	Common::Array<MapEntry> _points;	// Map locations for each scene
	Common::StringArray _locationNames;
	MapPaths _paths;
	Common::Array<Common::Point> _pathPoints;
	Common::Point _savedPos;
	Common::Point _savedSize;
	Surface _topLine;
	ImageFile *_mapCursors;
	ImageFile *_shapes;
	ImageFile *_iconShapes;
	byte _sequences[MAX_HOLMES_SEQUENCE][MAX_FRAME];
	Point32 _lDrawnPos;
	int _point;
	bool _placesShown;
	int _cursorIndex;
	bool _drawMap;
	Surface _iconSave;
private:
	void loadData();

	void setupSprites();
	void freeSprites();

	void showPlaces();

	void saveTopLine();
	void eraseTopLine();
	void showPlaceName(int idx, bool highlighted);

	void updateMap(bool flushScreen);

	void walkTheStreets();

	void saveIcon(ImageFrame *src, const Common::Point &pt);
	void restoreIcon();

	void highlightIcon(const Common::Point &pt);
public:
	bool _active;
	Point32 _overPos;
	Point32 _bigPos;
	int _charPoint, _oldCharPoint;
	bool _frameChangeFlag;
public:
	Map(SherlockEngine *vm);

	const MapEntry &operator[](int idx) { return _points[idx]; }

	void loadPoints(int count, const int *xList, const int *yList, const int *transList);
	void loadSequences(int count, const byte *seq);

	int show();

	void synchronize(Common::Serializer &s);
};

} // End of namespace Sherlock

#endif
