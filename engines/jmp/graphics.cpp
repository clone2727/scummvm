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

#include "jmp/bit.h"
#include "jmp/graphics.h"

#include "common/endian.h"
#include "common/file.h"
#include "common/str-array.h"
#include "common/stream.h"
#include "common/textconsole.h"
#include "engines/util.h"
#include "graphics/cursorman.h"
#include "graphics/fontman.h"
#include "graphics/wincursor.h"
#include "video/codecs/cinepak.h"

namespace JMP {
	
GraphicsManager::GraphicsManager(JMPEngine* vm) : _vm(vm) {
	initGraphics(640, 480, true, NULL);
		
	_pixelFormat = _vm->_system->getScreenFormat();
	useStandardCursor();
	CursorMan.showMouse(true);
		
	// FIXME: Use the correct font. MS Sans Serif in bit, I believe.
	// Don't remember what jman uses
	_font = FontMan.getFontByUsage(Graphics::FontManager::kBigGUIFont);
}
	
GraphicsManager::~GraphicsManager() {
}

// Windows cursor anyone?
static const byte s_standardCursor[] = {
	1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
	1, 2, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	1, 2, 2, 1, 0, 0, 0, 0, 0, 0, 0, 0,
	1, 2, 2, 2, 1, 0, 0, 0, 0, 0, 0, 0,
	1, 2, 2, 2, 2, 1, 0, 0, 0, 0, 0, 0,
	1, 2, 2, 2, 2, 2, 1, 0, 0, 0, 0, 0,
	1, 2, 2, 2, 2, 2, 2, 1, 0, 0, 0, 0,
	1, 2, 2, 2, 2, 2, 2, 2, 1, 0, 0, 0,
	1, 2, 2, 2, 2, 2, 2, 2, 2, 1, 0, 0,
	1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 0,
	1, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1,
	1, 2, 2, 2, 1, 2, 2, 1, 0, 0, 0, 0,
	1, 2, 2, 1, 1, 2, 2, 1, 0, 0, 0, 0,
	1, 2, 1, 0, 1, 1, 2, 2, 1, 0, 0, 0,
	1, 1, 0, 0, 0, 1, 2, 2, 1, 0, 0, 0,
	1, 0, 0, 0, 0, 0, 1, 2, 2, 1, 0, 0,
	0, 0, 0, 0, 0, 0, 1, 2, 2, 1, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 1, 2, 2, 1, 0,
	0, 0, 0, 0, 0, 0, 0, 1, 2, 2, 1, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0
};
	
static const byte s_standardCursorPalette[] = {
	0x00, 0x00, 0x00,	// Black
	0xFF, 0xFF, 0xFF	// White
};
	
void GraphicsManager::useStandardCursor() {
	CursorMan.replaceCursor(s_standardCursor, 12, 20, 0, 0, 0);
	CursorMan.replaceCursorPalette(s_standardCursorPalette, 1, 2);
}

void GraphicsManager::setCursor(uint16 id) {
	// HACK: The main cursor of BIT is broken in the exe (???)
	if (_vm->getGameType() == GType_BIT && id == kBITMainCursor) {
		useStandardCursor();
		return;
	}

	Graphics::WinCursorGroup *cursorGroup = 0;

	for (uint32 i = 0; i < _vm->_exeFiles.size() && !cursorGroup; i++)
		cursorGroup = Graphics::WinCursorGroup::createCursorGroup(*_vm->_exeFiles[i], id);

	if (cursorGroup) {
		Graphics::WinCursor *cursor = cursorGroup->cursors[0].cursor;
		CursorMan.replaceCursor(cursor->getSurface(), cursor->getWidth(), cursor->getHeight(), cursor->getHotspotX(), cursor->getHotspotY(), cursor->getKeyColor());
		CursorMan.replaceCursorPalette(cursor->getPalette(), 0, 256);
		delete cursorGroup;
	}
}
	
void GraphicsManager::drawString(Common::String string, Common::Rect rect, uint32 color) {
	if (!_font)
		return;

	Graphics::Surface *surf = _vm->_system->lockScreen();
	Common::StringArray strList;
	_font->wordWrapText(string, rect.width(), strList);
	for (uint i = 0; i < strList.size(); i++)
		_font->drawString(surf, strList[i], rect.left, rect.top + i * (_font->getFontHeight() + 1), rect.width(), color);
	_vm->_system->unlockScreen();
	_vm->_system->updateScreen();
}

void GraphicsManager::drawBitmap(Common::String filename, int x, int y) {
	Graphics::Surface surface = decodeBitmap(filename);
	_vm->_system->copyRectToScreen((byte*)surface.pixels, surface.pitch, x, y, surface.w, surface.h);
	surface.free();
	_vm->_system->updateScreen();
}

void GraphicsManager::drawBitmap(Common::String filename, Common::Rect srcRect, int x, int y, bool updateScreen) {
	Graphics::Surface surface = decodeBitmap(filename);
	_vm->_system->copyRectToScreen((const byte *)surface.getBasePtr(srcRect.left, srcRect.top), surface.pitch, x, y, srcRect.width(), srcRect.height());
	surface.free();
	
	if (updateScreen)
		_vm->_system->updateScreen();
}

void GraphicsManager::drawEXEBitmap(uint16 id, int x, int y) {
	Graphics::Surface surface = decodeBitmapNoHeader(_vm->getEXEResource(Common::kNEBitmap, id));
	_vm->_system->copyRectToScreen((byte*)surface.pixels, surface.pitch, x, y, surface.w, surface.h);
	surface.free();
}

Graphics::Surface GraphicsManager::decodeBitmap(Common::String filename) {
	Common::File *file = new Common::File();
	if (!file->open(filename.c_str()))
		error ("Couldn't open \'%s\'", filename.c_str());
	
	BitmapHeader header;
	
	header.type = file->readUint16BE();
	assert(header.type == 'BM');
	header.size = file->readUint32LE();
	assert(header.size != 0);
	header.res1 = file->readUint16LE();
	header.res2 = file->readUint16LE();
	header.imageOffset = file->readUint32LE();

	return decodeBitmapNoHeader(file, header.imageOffset);
}

#define BITMAP_CINEPAK_TAG MKTAG('d', 'i', 'v', 'c')

Graphics::Surface GraphicsManager::decodeBitmapNoHeader(Common::SeekableReadStream *stream, uint32 imageOffset) {
	assert(stream);

	InfoHeader info;

	info.size = stream->readUint32LE();
	info.width = stream->readUint32LE();
	info.height = stream->readUint32LE();
	info.planes = stream->readUint16LE();
	info.bitsPerPixel = stream->readUint16LE();
	info.compression = stream->readUint32LE();
	info.imageSize = stream->readUint32LE();
	info.pixelsPerMeterX = stream->readUint32LE();
	info.pixelsPerMeterY = stream->readUint32LE();
	info.colorsUsed = stream->readUint32LE();
	info.colorsImportant = stream->readUint32LE();

	assert(info.compression == 0 || BITMAP_CINEPAK_TAG);
	
	byte *palData = (byte *)malloc(3 * 256);
	
	if (info.bitsPerPixel == 8) {
		if (info.colorsUsed == 0)
			info.colorsUsed = 256;
		
		for (uint16 i = 0; i < info.colorsUsed; i++) {			
			palData[i * 3 + 2] = stream->readByte();
			palData[i * 3 + 1] = stream->readByte();
			palData[i * 3] = stream->readByte();
			stream->readByte();
		}
	} else if (info.bitsPerPixel > 8)
		assert(info.bitsPerPixel == 24);
	
	Graphics::Surface surface;
	surface.create(info.width, info.height, _pixelFormat);

	if (imageOffset != 0)
		stream->seek(imageOffset);

	if (info.compression == BITMAP_CINEPAK_TAG) {
		// The Cinepak Hack:
		// bitdata/title/bkg.btb contains a Cinepak frame in the bitmap
		// This is against the format. However, we can stil handle it.
		// Some images in the DLL are Cinepak as well.
		Common::SeekableReadStream *cinepakStream = stream->readStream(info.imageSize);
		
		Video::CinepakDecoder *cinepak = new Video::CinepakDecoder();
		const Graphics::Surface *cinepakSurface = cinepak->decodeImage(cinepakStream);
		surface.copyFrom(*cinepakSurface);
		
		delete cinepakStream;
		delete cinepak;
	} else {
		byte *dst = (byte *)surface.pixels + (surface.h - 1) * surface.pitch;
		int srcPitch = info.width * (info.bitsPerPixel >> 3);
		const int extraDataLength = (srcPitch % 4) ? 4 - (srcPitch % 4) : 0;

		for (uint16 i = 0; i < info.height; i++) {
			byte r = 0, g = 0, b = 0;
			for (uint16 j = 0; j < info.width; j++) {
				if (info.bitsPerPixel == 8) {
					byte index = stream->readByte();
					r = palData[index * 3];
					g = palData[index * 3 + 1];
					b = palData[index * 3 + 2];
				} else {
					b = stream->readByte();
					g = stream->readByte();
					r = stream->readByte();
				}

				if (_pixelFormat.bytesPerPixel == 2)
					*(uint16*)dst = _pixelFormat.RGBToColor(r, g, b);
				else
					*(uint32*)dst = _pixelFormat.RGBToColor(r, g, b);
				dst += _pixelFormat.bytesPerPixel;
			}

			dst -= surface.pitch * 2;
			stream->skip(extraDataLength);
		}
	}

	free(palData);
	delete stream;

	return surface;
}
	
} // End of namespace JMP
