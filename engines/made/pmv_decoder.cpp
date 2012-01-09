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

#include "common/stream.h"
#include "common/system.h"
#include "audio/audiostream.h"
#include "audio/decoders/raw.h"

#include "made/graphics.h"
#include "made/pmv_decoder.h"
#include "made/screen.h"


namespace Made {

PMVDecoder::PMVDecoder() : Video::FixedRateVideoDecoder() {
	_stream = 0;
	_surface = 0;
	_audioStream = 0;
}

PMVDecoder::~PMVDecoder() {
	close();
}

bool PMVDecoder::loadStream(Common::SeekableReadStream *stream) {
	_stream = stream;

	uint32 chunkType, chunkSize;

	readChunk(chunkType, chunkSize);	// "MOVE"
	if (chunkType != MKTAG('M','O','V','E')) {
		warning("Unexpected PMV video header, expected 'MOVE'");
		close();
		return false;
	}

	readChunk(chunkType, chunkSize);	// "MHED"
	if (chunkType != MKTAG('M','H','E','D')) {
		warning("Unexpected PMV video header, expected 'MHED'");
		close();
		return false;
	}

	_frameDelay = _stream->readUint16LE();
	_stream->skip(4);	// always 0?
	_frameCount = _stream->readUint16LE();
	_stream->skip(4);	// always 0?

	uint soundFreq = _stream->readUint16LE();
	// Note: There seem to be weird sound frequencies in PMV videos.
	// Not sure why, but leaving those original frequencies intact
	// results to sound being choppy. Therefore, we set them to more
	// "common" values here (11025 instead of 11127 and 22050 instead
	// of 22254)
	if (soundFreq == 11127) soundFreq = 11025;
	if (soundFreq == 22254) soundFreq = 22050;

	for (int i = 0; i < 22; i++) {
		int unk = _stream->readUint16LE();
		debug(2, "%i ", unk);
	}

	// Read palette
	_stream->read(_paletteRGB, 768);
	_dirtyPalette = true;

	// Dive into the first frame to get a width/height
	uint32 startPos = _stream->pos();

	readChunk(chunkType, chunkSize);
	if (chunkType != MKTAG('M','F','R','M')) {
		warning("Unknown chunk type");
		close();
		return false;
	}

	_stream->skip(12);
	uint32 frameDataOffset = _stream->readUint32LE();
	_stream->seek(frameDataOffset + startPos + 8);
	uint16 width = _stream->readUint16LE();
	uint16 height = _stream->readUint16LE();

	// Create our surface
	_surface = new Graphics::Surface();
	_surface->create(width, height, getPixelFormat());
	_stream->seek(startPos);

	// Initialize sound
	_audioStream = Audio::makeQueuingAudioStream(soundFreq, false);
	g_system->getMixer()->playStream(Audio::Mixer::kPlainSoundType, &_audioStreamHandle, _audioStream);

	return true;
}

void PMVDecoder::close() {
	if (_surface) {
		_surface->free();
		delete _surface;
		_surface = 0;
	}

	delete _stream; _stream = 0;
	_audioStream->finish();
}

bool PMVDecoder::isVideoLoaded() const {
	return _stream != 0;
}

Graphics::Surface *PMVDecoder::decodeNextFrame() {
	if (!isVideoLoaded() || endOfVideo())
		return 0;

	uint32 chunkType, chunkSize;
	readChunk(chunkType, chunkSize);
	if (chunkType != MKTAG('M','F','R','M'))
		warning("Unknown chunk type");

	byte *frameData = new byte[chunkSize];

	uint32 bytesRead = _stream->read(frameData, chunkSize);

	if (bytesRead < chunkSize || _stream->eos()) {
		warning("PMVDecoder: end of stream");
		close();
		return 0;
	}

	uint32 soundChunkOfs = READ_LE_UINT32(frameData + 8);
	uint32 palChunkOfs = READ_LE_UINT32(frameData + 16);

	// Handle audio
	if (soundChunkOfs) {
		byte *audioData = frameData + soundChunkOfs - 8;
		chunkSize = READ_LE_UINT16(audioData + 4);
		uint16 chunkCount = READ_LE_UINT16(audioData + 6);

		debug(1, "chunkCount = %d; chunkSize = %d; total = %d\n", chunkCount, chunkSize, chunkCount * chunkSize);

		uint32 soundSize = chunkCount * chunkSize;
		byte *soundData = (byte *)malloc(soundSize);
		decompressSound(audioData + 8, soundData, chunkSize, chunkCount);;
		_audioStream->queueBuffer(soundData, soundSize, DisposeAfterUse::YES, Audio::FLAG_UNSIGNED);
	}

	// Handle palette
	if (palChunkOfs) {
		byte *palData = frameData + palChunkOfs - 8;
		uint32 palSize = READ_LE_UINT32(palData + 4);
		decompressPalette(palData + 8, _paletteRGB, palSize);
		_dirtyPalette = true;
	}

	// Handle video
	byte *imageData = frameData + READ_LE_UINT32(frameData + 12) - 8;

	// frameNum @0
	//uint16 width = READ_LE_UINT16(imageData + 8);
	//uint16 height = READ_LE_UINT16(imageData + 10);
	uint16 cmdOffs = READ_LE_UINT16(imageData + 12);
	uint16 pixelOffs = READ_LE_UINT16(imageData + 16);
	uint16 maskOffs = READ_LE_UINT16(imageData + 20);
	uint16 lineSize = READ_LE_UINT16(imageData + 24);

	debug(2, "width = %d; height = %d; cmdOffs = %04X; pixelOffs = %04X; maskOffs = %04X; lineSize = %d\n",
		getWidth(), getHeight(), cmdOffs, pixelOffs, maskOffs, lineSize);

	// TODO: Mac 'MMV' use additional compression methods not yet handled
	decompressMovieImage(imageData, *_surface, cmdOffs, pixelOffs, maskOffs, lineSize);

	_curFrame++;

	if (_curFrame == 0)
		_startTime = g_system->getMillis();

	delete[] frameData;
	return _surface;
}

void PMVDecoder::readChunk(uint32 &chunkType, uint32 &chunkSize) {
	chunkType = _stream->readUint32BE();
	chunkSize = _stream->readUint32LE();

	debug(2, "ofs = %08X; chunkType = %c%c%c%c; chunkSize = %d\n",
		_stream->pos(),
		(chunkType >> 24) & 0xFF, (chunkType >> 16) & 0xFF, (chunkType >> 8) & 0xFF, chunkType & 0xFF,
		chunkSize);

}

void PMVDecoder::decompressPalette(byte *palData, byte *outPal, uint32 palDataSize) {
	byte *palDataEnd = palData + palDataSize;
	while (palData < palDataEnd) {
		byte count = *palData++;
		byte entry = *palData++;
		if (count == 255 && entry == 255)
			break;
		memcpy(&outPal[entry * 3], palData, (count + 1) * 3);
		palData += (count + 1) * 3;
	}
}

uint32 PMVDecoder::getElapsedTime() const {
	// FIXME: Using this A/V synced code cannot work properly. While it is correct in
	// theory, PMV files do not have properly interleaving. For example, in the intro
	// video, the first audio frame is 98ms while every frame is 101ms. This leads to
	// audio underflow and the Mixer getting confused because it stops getting samples
	// for ~3ms. This results in very choppy playback. The old code worked because
	// its syncing calculation would *always* end up skipping the first frame.
	//
	// The syncing using only start time should be sufficient for now, and would only
	// break if we decoded frames too quickly, which shouldn't happen if only
	// needsUpdate() and getTimeToNextFrame() are used (which is normal operation
	// anyway).
	//
	// In order to get better A/V sync, we can do one of the following things:
	// 1) Ignore interleaving and do manual demuxing
	// 2) Skip the first frame like the old code did (very hacky)
	//
	// IMO, neither are really worth it as this code works well enough.
#if 1
	return VideoDecoder::getElapsedTime();
#else
	if (!isVideoLoaded() || _curFrame < 0)
		return 0;

	// TODO: This is not working properly
	// It sometimes jumps backwards in time. I can't explain it.
	return g_system->getMixer()->getSoundElapsedTime(_audioStreamHandle);
#endif
}

} // End of namespace Made
