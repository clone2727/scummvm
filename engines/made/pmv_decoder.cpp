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

PMVDecoder::PMVDecoder() : Video::VideoDecoder() {
	_stream = 0;
}

PMVDecoder::~PMVDecoder() {
	close();
}

bool PMVDecoder::loadStream(Common::SeekableReadStream *stream) {
	_stream = stream;

	uint32 chunkType, chunkSize;

	readChunk(chunkType, chunkSize);	// "MOVE"
	if (chunkType != MKTAG('M','O','V','E')) {
		warning("Unexpected PMV video header '%s', expected 'MOVE'", tag2str(chunkType));
		close();
		return false;
	}

	readChunk(chunkType, chunkSize);	// "MHED"
	if (chunkType != MKTAG('M','H','E','D')) {
		warning("Unexpected PMV video header '%s', expected 'MHED'", tag2str(chunkType));
		close();
		return false;
	}

	uint16 frameDelay = _stream->readUint16LE();
	_stream->skip(4);	// always 0?
	uint16 frameCount = _stream->readUint16LE();
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
	byte paletteRGB[768];
	_stream->read(paletteRGB, sizeof(paletteRGB));

	// Dive into the first frame to get a width/height
	uint32 startPos = _stream->pos();

	readChunk(chunkType, chunkSize);
	if (chunkType != MKTAG('M','F','R','M')) {
		warning("Unknown chunk type '%s'", tag2str(chunkType));
		close();
		return false;
	}

	_stream->skip(12);
	uint32 frameDataOffset = _stream->readUint32LE();
	_stream->seek(frameDataOffset + startPos + 8);
	uint16 width = _stream->readUint16LE();
	uint16 height = _stream->readUint16LE();

	// Create our surface
	addTrack(new PMVVideoTrack(width, height, frameDelay, frameCount, paletteRGB));

	// Initialize sound
	addTrack(new PMVAudioTrack(soundFreq));

	_stream->seek(startPos);
	return true;
}

void PMVDecoder::close() {
	VideoDecoder::close();

	delete _stream; _stream = 0;
}

void PMVDecoder::readNextPacket() {
	if (!isVideoLoaded() || endOfVideo())
		return;

	uint32 chunkType, chunkSize;
	readChunk(chunkType, chunkSize);
	if (chunkType != MKTAG('M','F','R','M'))
		error("Unknown chunk type");

	byte *frameData = new byte[chunkSize];

	uint32 bytesRead = _stream->read(frameData, chunkSize);

	if (bytesRead < chunkSize || _stream->eos())
		error("PMVDecoder: end of stream");

	uint32 soundChunkOfs = READ_LE_UINT32(frameData + 8);
	uint32 palChunkOfs = READ_LE_UINT32(frameData + 16);

	// Handle audio
	if (soundChunkOfs)
		((PMVAudioTrack *)getTrack(1))->queueSound(frameData + soundChunkOfs - 8);

	PMVVideoTrack *videoTrack = (PMVVideoTrack *)getTrack(0);

	// Handle palette
	if (palChunkOfs)
		videoTrack->decompressPalette(frameData + palChunkOfs - 8);

	// Handle video
	videoTrack->decodeFrame(frameData + READ_LE_UINT32(frameData + 12) - 8);

	delete[] frameData;
}

PMVDecoder::PMVVideoTrack::PMVVideoTrack(uint width, uint height, uint frameDelay, uint frameCount, const byte *palette) {
	_surface = new Graphics::Surface();
	_surface->create(width, height, getPixelFormat());
	_frameDelay = frameDelay;
	_frameCount = frameCount;
	_curFrame = -1;
	memcpy(_paletteRGB, palette, 768);
	_dirtyPalette = true;
}

PMVDecoder::PMVVideoTrack::~PMVVideoTrack() {
	_surface->free();
	delete _surface;
}

void PMVDecoder::PMVVideoTrack::decompressPalette(byte *palData) {
	uint32 palDataSize = READ_LE_UINT32(palData + 4);
	palData += 8;

	byte *palDataEnd = palData + palDataSize;
	while (palData < palDataEnd) {
		byte count = *palData++;
		byte entry = *palData++;

		if (count == 255 && entry == 255)
			break;

		memcpy(&_paletteRGB[entry * 3], palData, (count + 1) * 3);
		palData += (count + 1) * 3;
	}

	_dirtyPalette = true;
}

void PMVDecoder:: PMVVideoTrack::decodeFrame(byte *imageData) {
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
}

PMVDecoder::PMVAudioTrack::PMVAudioTrack(uint soundFreq) {
	_audioStream = Audio::makeQueuingAudioStream(soundFreq, false);
}

PMVDecoder::PMVAudioTrack::~PMVAudioTrack() {
	delete _audioStream;
}

void PMVDecoder::PMVAudioTrack::queueSound(byte *audioData) {
	uint16 chunkSize = READ_LE_UINT16(audioData + 4);
	uint16 chunkCount = READ_LE_UINT16(audioData + 6);

	debug(1, "audio chunkCount = %d; chunkSize = %d; total = %d\n", chunkCount, chunkSize, chunkCount * chunkSize);

	uint32 soundSize = chunkCount * chunkSize;
	byte *soundData = (byte *)malloc(soundSize);
	decompressSound(audioData + 8, soundData, chunkSize, chunkCount);
	_audioStream->queueBuffer(soundData, soundSize, DisposeAfterUse::YES, Audio::FLAG_UNSIGNED);
}

Audio::AudioStream *PMVDecoder::PMVAudioTrack::getAudioStream() const {
	return _audioStream;
}

void PMVDecoder::readChunk(uint32 &chunkType, uint32 &chunkSize) {
	chunkType = _stream->readUint32BE();
	chunkSize = _stream->readUint32LE();

	debug(2, "ofs = %08X; chunkType = %c%c%c%c; chunkSize = %d\n",
		_stream->pos(),
		(chunkType >> 24) & 0xFF, (chunkType >> 16) & 0xFF, (chunkType >> 8) & 0xFF, chunkType & 0xFF,
		chunkSize);

}

} // End of namespace Made
