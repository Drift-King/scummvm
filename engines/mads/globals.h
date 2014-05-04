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

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 */

#ifndef MADS_GLOBALS_H
#define MADS_GLOBALS_H

#include "common/scummsys.h"
#include "common/array.h"
#include "common/serializer.h"

namespace MADS {

class Globals {
protected:
	Common::Array<int16> _data;
public:
	Globals() {}

	virtual ~Globals() {}

	/**
	* Square brackets operator for accessing flags
	*/
	int16 &operator[](int idx) { return _data[idx]; }

	/*
	 * Resets all the globals to empty
	 */
	void reset();

	/**
	 * Synchronize the globals data
	 */
	virtual void synchronize(Common::Serializer &s);
};

} // End of namespace MADS

#endif /* MADS_GLOBALS_H */
