/* ScummVM - Scumm Interpreter
 * Copyright (C) 2002-2003 The ScummVM project
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
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 * $Header$
 */

#include "stdafx.h"
#include "scumm.h"
#include "nut_renderer.h"

NutRenderer::NutRenderer(Scumm *vm) {
	_vm = vm;
	_initialized = false;
	_loaded = false;
	_dataSrc = NULL;
}

NutRenderer::~NutRenderer() {
	if (_dataSrc != NULL) {
		free(_dataSrc);
		_dataSrc = NULL;
	}
}

void NutRenderer::decodeCodec44(byte *dst, byte *src, uint32 length) {
	byte val;
	uint16 size_line, num;

	do {
		size_line = READ_LE_UINT16(src);
		src += 2;
		length -= 2;

		while (size_line != 0) {
			num = *src++;
			val = *src++;
			memset(dst, val, num);
			dst += num;
			length -= 2;
			size_line -= 2;
			if (size_line != 0) {
				num = READ_LE_UINT16(src) + 1;
				src += 2;
				memcpy(dst, src, num);
				dst += num;
				src += num;
				length -= num + 2;
				size_line -= num + 2;
			}
		}
		dst--;

	} while (length > 1);
}

bool NutRenderer::loadFont(const char *filename, const char *dir) {
	debug(8, "NutRenderer::loadFont() called");
	if (_loaded == true) {
		debug(8, "NutRenderer::loadFont() Font already loaded, ok, loading...");
	}
	
	File file;
	file.open(filename, dir);
	if (file.isOpen() == false) {
		warning("NutRenderer::loadFont() Can't open font file: %s/%s", dir, filename);
		return false;
	}

	uint32 tag = file.readUint32BE();
	if (tag != 'ANIM') {
		debug(8, "NutRenderer::loadFont() there is no ANIM chunk in font header");
		return false;
	}

	if (_dataSrc != NULL) {
		free(_dataSrc);
		_dataSrc = NULL;
	}

	uint32 length = file.readUint32BE();
	_dataSrc = (byte *)malloc(length);
	file.read(_dataSrc, length);
	file.close();

	if (READ_BE_UINT32(_dataSrc) != 'AHDR') {
		debug(8, "NutRenderer::loadFont() there is no AHDR chunk in font header");
		free(_dataSrc);
		_dataSrc = NULL;
		return false;
	}
	
	int32 l;
	uint32 offset = READ_BE_UINT32(_dataSrc + 4) + 8;
	memset(_offsets, 0, 256 * sizeof(int32));
	for (l = 0; l < 256; l++) {
		if (READ_BE_UINT32(_dataSrc + offset) == 'FRME') {
			offset += 8;
			if (READ_BE_UINT32(_dataSrc + offset) == 'FOBJ') {
				_offsets[l] = offset + 8;
				offset += READ_BE_UINT32(_dataSrc + offset + 4) + 8;
			} else {
				debug(8, "NutRenderer::loadFont(%s, %s) there is no FOBJ chunk in FRME chunk %d (offset %x)", filename, dir, l, offset);
				break;
			}
		} else {
			debug(8, "NutRenderer::loadFont(%s, %s) there is no FRME chunk %d (offset %x)", filename, dir, l, offset);
			break;
		}
	}

	_loaded = true;
	return true;
}

int32 NutRenderer::getCharWidth(byte c) {
	debug(8, "NutRenderer::getCharWidth() called");
	if (_loaded == false) {
		debug(8, "NutRenderer::getCharWidth() Font is not loaded");
		return 0;
	}

	return READ_LE_UINT16(_dataSrc + _offsets[c] + 6);
}

int32 NutRenderer::getCharHeight(byte c) {
	debug(8, "NutRenderer::getCharHeight() called");
	if (_loaded == false) {
		debug(8, "NutRenderer::getCharHeight() Font is not loaded");
		return 0;
	}

	return READ_LE_UINT16(_dataSrc + _offsets[c] + 8);
}

int32 NutRenderer::getStringWidth(const byte *string) {
	debug(8, "NutRenderer::getStringWidth() called");
	if (_loaded == false) {
		debug(8, "NutRenderer::getStringWidth() Font is not loaded");
		return 0;
	}
	int32 width = 0;
	
	while (*string) {
		width += getCharWidth(*string++);
	}

	return width;
}

void NutRenderer::drawChar(byte c, int32 x, int32 y, byte color, bool useMask) {
	debug(8, "NutRenderer::drawChar('%c', %d, %d, %d, %d) called", c, x, y, (int)color, useMask);
	if (_loaded == false) {
		debug(8, "NutRenderer::drawChar() Font is not loaded");
		return;
	}

	const uint32 length = READ_BE_UINT32(_dataSrc + _offsets[c] - 4) - 14;
	const int32 width = READ_LE_UINT16(_dataSrc + _offsets[c] + 6);
	const int32 height = READ_LE_UINT16(_dataSrc + _offsets[c] + 8);

	byte *src = (byte*)(_dataSrc + _offsets[c] + 14);
	decodeCodec44(_tmpCodecBuffer, src, length);
	
	// HACK: we draw the character a total of 7 times: 6 times shifted
	// and in black for the shadow, and once in the right color and position.
	// This way we achieve the exact look as the original CMI had. However,
	// the question remains whether they did it this way, too, or if there is
	// some "font shadow" resource we don't know yet.
	// One problem remains: the fonts on the save/load screen don't have a
	// shadow. So how do we know whether to draw text with or without shadow?
	
	int offsetX[7] = { -1,  0, 1, 0, 1, 2, 0 };
	int offsetY[7] = {  0, -1, 0, 1, 2, 1, 0 };
	int cTable[7] =  {  0,  0, 0, 0, 0, 0, color };

	byte *dst, *mask = NULL;
	byte maskmask;
	int maskpos;
	
	for (int i = 0; i < 7; i++) {
		x += offsetX[i];
		y += offsetY[i];
		color = cTable[i];
	
		dst = _vm->virtscr[0].screenPtr + y * _vm->_screenWidth + x + _vm->virtscr[0].xstart;
		mask = _vm->getMaskBuffer(x, y, 0);
	
		src = _tmpCodecBuffer;
	
		for (int32 ty = 0; ty < height; ty++) {
			maskmask = revBitMask[x & 7];
			maskpos = 0;
			for (int32 tx = 0; tx < width; tx++) {
				byte pixel = *src++;
				if (x + tx < 0 || x + tx >= _vm->_screenWidth || y + ty < 0 || y + ty >= _vm->_screenHeight)
					continue;
#if 1
				if (pixel != 0) {
					dst[tx] = color;
					if (useMask)
						mask[maskpos] |= maskmask;
				}
#else
				if (pixel != 0) {
					if (pixel == 0x01)
						pixel = (color == 0) ? 0xf : color;
					if (pixel == 0xff)
						pixel = 0x0;
					dst[tx] = pixel;
					if (useMask)
						mask[maskpos] |= maskmask;
				}
#endif
				maskmask >>= 1;
				if (maskmask == 0) {
					maskmask = 0x80;
					maskpos++;
				}
			}
			dst += _vm->_screenWidth;
			mask += _vm->gdi._numStrips;
		}
	
		x -= offsetX[i];
		y -= offsetY[i];
	}
}
