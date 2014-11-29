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
#include "image/bmp.h"

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
	
void GraphicsManager::useStandardCursor() {
	Graphics::Cursor *cursor = Graphics::makeDefaultWinCursor();
	CursorMan.replaceCursor(cursor->getSurface(), cursor->getWidth(), cursor->getHeight(), cursor->getHotspotX(), cursor->getHotspotY(), cursor->getKeyColor());
	CursorMan.replaceCursorPalette(cursor->getPalette(), cursor->getPaletteStartIndex(), cursor->getPaletteCount());
	delete cursor;
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
		Graphics::Cursor *cursor = cursorGroup->cursors[0].cursor;
		CursorMan.replaceCursor(cursor->getSurface(), cursor->getWidth(), cursor->getHeight(), cursor->getHotspotX(), cursor->getHotspotY(), cursor->getKeyColor());
		CursorMan.replaceCursorPalette(cursor->getPalette(), cursor->getPaletteStartIndex(), cursor->getPaletteCount());
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
	_vm->_system->copyRectToScreen((byte *)surface.getPixels(), surface.pitch, x, y, surface.w, surface.h);
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
	Graphics::Surface surface = decodeBitmap(_vm->getEXEResource(Common::kNEBitmap, id));
	_vm->_system->copyRectToScreen((byte *)surface.getPixels(), surface.pitch, x, y, surface.w, surface.h);
	surface.free();
}

Graphics::Surface GraphicsManager::decodeBitmap(Common::String filename) {
	Common::File *file = new Common::File();
	if (!file->open(filename.c_str()))
		error ("Couldn't open \'%s\'", filename.c_str());

	return decodeBitmap(file);
}

Graphics::Surface GraphicsManager::decodeBitmap(Common::SeekableReadStream *stream) {
	assert(stream);

	Image::BitmapDecoder bmp;
	if (!bmp.loadStream(*stream))
		error("Failed to decode bitmap");

	const Graphics::Surface *decodedSurface = bmp.getSurface();
	const byte *palette = bmp.getPalette();

	Graphics::Surface output;
	output.copyFrom(*decodedSurface);
	output.convertToInPlace(g_system->getScreenFormat(), palette);

	return output;
}
	
} // End of namespace JMP
