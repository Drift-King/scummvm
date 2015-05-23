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

#include "sherlock/surface.h"
#include "sherlock/sherlock.h"
#include "sherlock/resources.h"
#include "common/system.h"
#include "graphics/palette.h"

namespace Sherlock {

Surface::Surface(uint16 width, uint16 height) : _freePixels(true) {
	create(width, height);
}

Surface::Surface() : _freePixels(false) {
}

Surface::~Surface() {
	if (_freePixels)
		_surface.free();
}

void Surface::create(uint16 width, uint16 height) {
	if (_freePixels)
		_surface.free();

	_surface.create(width, height, Graphics::PixelFormat::createFormatCLUT8());
	_freePixels = true;
}

void Surface::blitFrom(const Surface &src) {
	blitFrom(src, Common::Point(0, 0));
}

void Surface::blitFrom(const ImageFrame &src) {
	blitFrom(src._frame, Common::Point(0, 0));
}

void Surface::blitFrom(const Graphics::Surface &src) {
	blitFrom(src, Common::Point(0, 0));
}

void Surface::blitFrom(const Surface &src, const Common::Point &pt) {
	blitFrom(src, pt, Common::Rect(0, 0, src._surface.w, src._surface.h));
}

void Surface::blitFrom(const ImageFrame &src, const Common::Point &pt) {
	blitFrom(src._frame, pt, Common::Rect(0, 0, src._frame.w, src._frame.h));
}

void Surface::blitFrom(const Graphics::Surface &src, const Common::Point &pt) {
	blitFrom(src, pt, Common::Rect(0, 0, src.w, src.h));
}

void Surface::blitFrom(const Graphics::Surface &src, const Common::Point &pt, const Common::Rect &srcBounds) {
	Common::Rect srcRect = srcBounds;
	Common::Rect destRect(pt.x, pt.y, pt.x + srcRect.width(), pt.y + srcRect.height());

	if (srcRect.isValidRect() && clip(srcRect, destRect)) {
		// Surface is at least partially or completely on-screen
		addDirtyRect(destRect);
		_surface.copyRectToSurface(src, destRect.left, destRect.top, srcRect);
	}
}

void Surface::blitFrom(const ImageFrame &src, const Common::Point &pt, const Common::Rect &srcBounds) {
	blitFrom(src._frame, pt, srcBounds);
}

void Surface::blitFrom(const Surface &src, const Common::Point &pt, const Common::Rect &srcBounds) {
	blitFrom(src._surface, pt, srcBounds);
}

void Surface::transBlitFrom(const ImageFrame &src, const Common::Point &pt,
		bool flipped, int overrideColor) {
	transBlitFrom(src._frame, pt + src._offset, flipped, overrideColor);
}

void Surface::transBlitFrom(const Graphics::Surface &src, const Common::Point &pt,
		bool flipped, int overrideColor) {
	Common::Rect drawRect(0, 0, src.w, src.h);
	Common::Rect destRect(pt.x, pt.y, pt.x + src.w, pt.y + src.h);

	// Clip the display area to on-screen
	if (!clip(drawRect, destRect))
		// It's completely off-screen
		return;

	if (flipped)
		drawRect = Common::Rect(src.w - drawRect.right, src.h - drawRect.bottom,
			src.w - drawRect.left, src.h - drawRect.top);

	Common::Point destPt(destRect.left, destRect.top);
	addDirtyRect(Common::Rect(destPt.x, destPt.y, destPt.x + drawRect.width(),
		destPt.y + drawRect.height()));

	// Draw loop
	const int TRANSPARENCY = 0xFF;
	for (int yp = 0; yp < drawRect.height(); ++yp) {
		const byte *srcP = (const byte *)src.getBasePtr(
			flipped ? drawRect.right - 1 : drawRect.left, drawRect.top + yp);
		byte *destP = (byte *)getBasePtr(destPt.x, destPt.y + yp);

		for (int xp = 0; xp < drawRect.width(); ++xp, ++destP) {
			if (*srcP != TRANSPARENCY)
				*destP = overrideColor ? overrideColor : *srcP;

			srcP = flipped ? srcP - 1 : srcP + 1;
		}
	}
}

void Surface::fillRect(int x1, int y1, int x2, int y2, byte color) {
	fillRect(Common::Rect(x1, y1, x2, y2), color);
}

void Surface::fillRect(const Common::Rect &r, byte color) {
	_surface.fillRect(r, color);
	addDirtyRect(r);
}

bool Surface::clip(Common::Rect &srcBounds, Common::Rect &destBounds) {
	if (destBounds.left >= _surface.w || destBounds.top >= _surface.h ||
			destBounds.right <= 0 || destBounds.bottom <= 0)
		return false;

	// Clip the bounds if necessary to fit on-screen
	if (destBounds.right > _surface.w) {
		srcBounds.right -= destBounds.right - _surface.w;
		destBounds.right = _surface.w;
	}

	if (destBounds.bottom > _surface.h) {
		srcBounds.bottom -= destBounds.bottom - _surface.h;
		destBounds.bottom = _surface.h;
	}

	if (destBounds.top < 0) {
		srcBounds.top += -destBounds.top;
		destBounds.top = 0;
	}

	if (destBounds.left < 0) {
		srcBounds.left += -destBounds.left;
		destBounds.left = 0;
	}

	return true;
}

void Surface::clear() {
	fillRect(Common::Rect(0, 0, _surface.w, _surface.h), 0);
}

void Surface::free() {
	if (_freePixels) {
		_surface.free();
		_freePixels = false;
	}
}

void Surface::setPixels(byte *pixels, int width, int height) {
	_surface.format = Graphics::PixelFormat::createFormatCLUT8();
	_surface.w = _surface.pitch = width;
	_surface.h = height;
	_surface.setPixels(pixels);
}

} // End of namespace Sherlock
