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

#ifndef FOGFX_H_INCLUDED
#define FOGFX_H_INCLUDED

#include "zvision/graphics/effect.h"

namespace ZVision {

class ZVision;

class FogFx : public Effect {
public:

	FogFx(ZVision *engine, uint32 key, Common::Rect region, bool ported, EffectMap *Map, const Common::String &clouds);
	~FogFx();

	const Graphics::Surface *draw(const Graphics::Surface &srcSubRect);

	void update();

private:
	EffectMap *_map;
	Graphics::Surface _fog;
	uint8 _r, _g, _b;
	int32 _pos;
	Common::Array< Common::Array< bool > > _mp;
	uint16 _colorMap[32];
};
} // End of namespace ZVision

#endif // FOGFX_H_INCLUDED
