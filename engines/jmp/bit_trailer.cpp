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

namespace JMP {

Common::Error JMPEngine_BITTrailer::run() {
	init();

	loadMandatoryEXE(getEXEFileName());

	// For Buried in Time trailers:
	// Bitmap 101 is the 8bpp version
	// Bitmap 102 is the 24bpp version
#if 0
	Common::SeekableReadStream *bitmap = getEXEResource(kNEBitmap, 0x65);
	assert(bitmap);
	byte *data = (byte *)malloc(bitmap->size());
	bitmap->read(data, bitmap->size());
	Common::hexdump(data, bitmap->size());
	free(data);
	delete bitmap;
#endif

	_gfx->drawEXEBitmap(useHighColor() ? 102 : 101);
	playVideo(getAVIFileName(), 103, 135);
		
	return Common::kNoError;
}

} // End of namespace JMP
