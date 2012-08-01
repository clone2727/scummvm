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

#include "common/macresman.h"
#include "common/stream.h"
#include "common/system.h"
#include "engines/util.h"
#include "graphics/cursorman.h"
#include "graphics/maccursor.h"

#include "sprint/graphics.h"

namespace Sprint {

GraphicsManager::GraphicsManager(Common::MacResManager *resFork) : _resFork(resFork) {
	_imageFile = 0;

	initGraphics(544, 332, true, 0);

	if (g_system->getScreenFormat().bytesPerPixel == 1)
		error("Myst ME requires greater than 256 colors to run");
}

GraphicsManager::~GraphicsManager() {
	delete _imageFile;
}

void GraphicsManager::openImageArchive(const Common::String &prefix) {
	delete _imageFile;
	_imageEntries.clear();

	Common::String name = prefix + "pics";

	_imageFile = SearchMan.createReadStreamForMember(name);

	if (!_imageFile)
		error("Could not open image archive '%s'", name.c_str());

	uint32 count = _imageFile->readUint32BE();
	_imageEntries.reserve(count);

	while (count--) {
		ImageEntry entry;
		entry.offset = _imageFile->readUint32BE();
		entry.size = _imageFile->readUint32BE();
		entry.id = _imageFile->readUint16BE();
		entry.type = _imageFile->readUint16BE();
		entry.width = _imageFile->readUint16BE();
		entry.height = _imageFile->readUint16BE();
		_imageEntries.push_back(entry);
	}
}

void GraphicsManager::setCursor(uint16 id) {
	Common::SeekableReadStream *stream = _resFork->getResource(MKTAG('c', 'r', 's', 'r'), id);

	if (!stream)
		error("Failed to find cursor %d", id);

	Graphics::MacCursor cursor;
	if (!cursor.readFromStream(*stream))
		error("Failed to load Mac cursor %d", id);

	delete stream;

	CursorMan.replaceCursor(cursor.getSurface(), cursor.getWidth(), cursor.getHeight(), cursor.getHotspotX(), cursor.getHotspotY(), cursor.getKeyColor());
	CursorMan.replaceCursorPalette(cursor.getPalette(), cursor.getPaletteStartIndex(), cursor.getPaletteCount());
}

} // end of namespace Sprint
