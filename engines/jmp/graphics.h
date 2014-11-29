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

#ifndef JMP_GRAPHICS_H
#define JMP_GRAPHICS_H

#include "common/rect.h"
#include "common/str.h"
#include "common/system.h"
#include "graphics/font.h"
#include "graphics/surface.h"

#include "jmp/jmp.h"

namespace JMP {
	
struct BitmapHeader {
	uint16 type;
	uint32 size;
	uint16 res1;
	uint16 res2;
	uint32 imageOffset;
};

struct InfoHeader {
	uint32 size;
	uint32 width;
	uint32 height;
	uint16 planes;
	uint16 bitsPerPixel;
	uint32 compression;
	uint32 imageSize;
	uint32 pixelsPerMeterX;
	uint32 pixelsPerMeterY;
	uint32 colorsUsed;
	uint32 colorsImportant;
};
	
class JMPEngine;

class GraphicsManager {
public:
	GraphicsManager(JMPEngine* vm);
	~GraphicsManager();
		
	void useStandardCursor();
	void setCursor(uint16 id);
		
	void drawString(Common::String string, Common::Rect rect, uint32 color);

	void drawBitmap(Common::String filename, int x, int y);
	void drawBitmap(Common::String filename, Common::Rect srcRect, int x, int y, bool updateScreen = true);
	void drawEXEBitmap(uint16 id, int x = 0, int y = 0);

private:
	Graphics::Surface decodeBitmap(Common::String filename);
	Graphics::Surface decodeBitmap(Common::SeekableReadStream *stream);
		
	JMPEngine *_vm;
	const Graphics::Font *_font;
	Graphics::PixelFormat _pixelFormat;
};

} // End of namespace JMP

#endif
