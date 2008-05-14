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
 * $URL$
 * $Id$
 *
 */

#ifndef MADE_SCREENFX_H
#define MADE_SCREENFX_H

#include "common/endian.h"
#include "common/util.h"
#include "common/rect.h"

#include "graphics/surface.h"

#include "made/made.h"
#include "made/screen.h"

namespace Made {

class ScreenEffects {
public:
	ScreenEffects(Screen *screen);
	void run(int16 effectNum, Graphics::Surface *surface, byte *palette, byte *newPalette, int colorCount);
	void flash(int count, byte *palette, int colorCount);
private:
	Screen *_screen;
	byte _fxPalette[768];
	static const byte vfxOffsTable[64];
	static const byte vfxOffsIndexTable[8];
	const byte *vfxOffsTablePtr;
	int16 vfxX1, vfxY1, vfxWidth, vfxHeight;
	void setPalette(byte *palette);
	void setBlendedPalette(byte *palette, byte *newPalette, int colorCount, int16 value, int16 maxValue);
	void copyRect(Graphics::Surface *surface, int16 x1, int16 y1, int16 x2, int16 y2);
	void vfx00(Graphics::Surface *surface, byte *palette, byte *newPalette, int colorCount);
	void vfx09(Graphics::Surface *surface, byte *palette, byte *newPalette, int colorCount);
	void vfx14(Graphics::Surface *surface, byte *palette, byte *newPalette, int colorCount);
	void vfx17(Graphics::Surface *surface, byte *palette, byte *newPalette, int colorCount);
};

} // End of namespace Made

#endif /* MADE_H */
