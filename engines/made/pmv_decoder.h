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

class PMVDecoder : public Video::VideoDecoder {
public:
	PMVDecoder();
	~PMVDecoder();

	// VideoDecoder API
	bool loadStream(Common::SeekableReadStream *stream);
	void close();

protected:
	void readNextPacket();
	bool useAudioSync() const { return false; }

private:
	class PMVVideoTrack : public FixedRateVideoTrack {
	public:
		PMVVideoTrack(uint width, uint height, uint frameDelay, uint frameCount, const byte *palette);
		~PMVVideoTrack();

		uint16 getWidth() const { return _surface->w; }
		uint16 getHeight() const { return _surface->h; }
		Graphics::PixelFormat getPixelFormat() const { return Graphics::PixelFormat::createFormatCLUT8(); }
		int getCurFrame() const { return _curFrame; }
		int getFrameCount() const { return _frameCount; }
		const Graphics::Surface *decodeNextFrame() { return _surface; }
		const byte *getPalette() const { _dirtyPalette = false; return _paletteRGB; }
		bool hasDirtyPalette() const { return _dirtyPalette; }

		void decompressPalette(byte *palData);
		void decodeFrame(byte *frameData);

	protected:
		Common::Rational getFrameRate() const { return Common::Rational(1000, _frameDelay); }

	private:
		byte _paletteRGB[768];
		mutable bool _dirtyPalette;
		Graphics::Surface *_surface;

		int _curFrame;
		uint _frameDelay;
		int _frameCount;
	};

	class PMVAudioTrack : public AudioTrack {
	public:
		PMVAudioTrack(uint soundFreq);
		~PMVAudioTrack();

		void queueSound(byte *audioData);

	protected:
		Audio::AudioStream *getAudioStream() const;

	private:
		Audio::QueuingAudioStream *_audioStream;
	};

	Common::SeekableReadStream *_stream;

	void readChunk(uint32 &chunkType, uint32 &chunkSize);
};

} // End of namespace Made

#endif
