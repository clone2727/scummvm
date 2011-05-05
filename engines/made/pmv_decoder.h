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

#ifndef MADE_PMV_DECODER_H
#define MADE_PMV_DECODER_H

#include "audio/mixer.h"
#include "common/rational.h"
#include "graphics/surface.h"

#include "video/video_decoder.h"

namespace Audio {
	class QueuingAudioStream;
}

namespace Made {

class PMVDecoder : public Video::FixedRateVideoDecoder {
public:
	PMVDecoder();
	~PMVDecoder();

	// VideoDecoder API
	bool loadStream(Common::SeekableReadStream *stream);
	void close();
	bool isVideoLoaded() const;
	uint16 getWidth() const { return _surface ? _surface->w : 0; }
	uint16 getHeight() const { return _surface ? _surface->h : 0; }
	Graphics::PixelFormat getPixelFormat() const { return Graphics::PixelFormat::createFormatCLUT8(); }
	const byte *getPalette() { return _paletteRGB; }
	bool hasDirtyPalette() const { return _dirtyPalette; }
	uint32 getFrameCount() const { return _frameCount; }
	uint32 getElapsedTime() const;
	Graphics::Surface *decodeNextFrame();

protected:
	// FixedRateVideoDecoder API
	Common::Rational getFrameRate() const { return Common::Rational(1000, _frameDelay); }

protected:
	byte _paletteRGB[768];
	bool _dirtyPalette;
	Graphics::Surface *_surface;

	uint16 _frameDelay;
	uint16 _frameCount;

	Common::SeekableReadStream *_stream;
	Audio::QueuingAudioStream *_audioStream;
	Audio::SoundHandle _audioStreamHandle;

	void readChunk(uint32 &chunkType, uint32 &chunkSize);
	void decompressPalette(byte *palData, byte *outPal, uint32 palDataSize);
};

} // End of namespace Made

#endif
