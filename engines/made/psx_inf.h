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

#ifndef MADE_PSXINF_H
#define MADE_PSXINF_H

#include "common/file.h"
#include "common/hashmap.h"
#include "common/hash-str.h"
#include "common/str.h"

namespace Video {
class VideoDecoder;
}

namespace Made {

/**
 * An archive implementation of RTZ PSX inf (index) and stl/xai (stream) files
 */
class PSXStreamINF {
public:
	PSXStreamINF(const Common::String &baseFileName, bool audio);
	~PSXStreamINF();

	bool hasFile(const Common::String &name) const;
	Video::VideoDecoder *createVideoDecoderForMember(const Common::String &name) const;

private:
	struct FileEntry {
		uint32 offset;
		uint32 size;
		bool highSpeed;
	};

	Common::String _streamFileName;

	typedef Common::HashMap<Common::String, FileEntry, Common::IgnoreCase_Hash, Common::IgnoreCase_EqualTo> FileMap;
	FileMap _map;
};

} // End of namespace Made

#endif
