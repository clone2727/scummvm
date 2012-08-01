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

#ifndef SPRINT_GRAPHICS_H
#define SPRINT_GRAPHICS_H

#include "common/array.h"

namespace Common {
class MacResManager;
class SeekableReadStream;
}

namespace Sprint {

class GraphicsManager {
public:
	GraphicsManager(Common::MacResManager *resFork);
	~GraphicsManager();

	void openImageArchive(const Common::String &prefix);

private:
	enum ImageType {
		kImageTypeJPEG = 0,
		kImageTypePICT = 1
	};

	struct ImageEntry {
		uint32 offset;
		uint32 size;
		uint16 id;
		uint16 type;
		uint16 width;
		uint16 height;
	};

	Common::Array<ImageEntry> _imageEntries;
	Common::SeekableReadStream *_imageFile;
	Common::MacResManager *_resFork;	
};

} // End of namespace Sprint

#endif
