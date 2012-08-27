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

#include "common/config-manager.h"
#include "common/events.h"
#include "common/file.h"
#include "common/textconsole.h"

#include "base/plugins.h"
#include "base/version.h"

#include "jmp/jmp.h"

#include "video/avi_decoder.h"

namespace JMP {

JMPEngine::JMPEngine(OSystem *syst, const JMPGameDescription *gamedesc) : Engine(syst), _gameDescription(gamedesc) {
}

JMPEngine::~JMPEngine() {
	delete _gfx;
	delete _sound;
}

void JMPEngine::init() {
	_gfx = new GraphicsManager(this);
	_sound = new Sound(this);	
}

void JMPEngine::playVideo(Common::String filename, uint16 x, uint16 y) {
	Video::VideoDecoder *video = new Video::AVIDecoder();

	if (!video->loadFile(filename)) {
		delete video;
		return;
	}

	playVideo(video, x, y);
	delete video;
}

void JMPEngine::playVideoCentered(Common::String filename) {
	Video::VideoDecoder *video = new Video::AVIDecoder();

	if (!video->loadFile(filename)) {
		delete video;
		return;
	}

	playVideo(video, (_system->getWidth() - video->getWidth()) / 2, (_system->getHeight() - video->getHeight()) / 2);
	delete video;
}

void JMPEngine::playVideo(Video::VideoDecoder *video, uint16 x, uint16 y) {
	bool continuePlaying = true;

	video->start();

	while (!shouldQuit() && !video->endOfVideo() && continuePlaying) {		
		if (video->needsUpdate()) {
			const Graphics::Surface *frame = video->decodeNextFrame();
			Graphics::Surface *convertedFrame = 0;

			if (frame) {
				if (frame->format != _system->getScreenFormat()) {
					convertedFrame = frame->convertTo(_system->getScreenFormat(), video->getPalette());
					frame = convertedFrame;
				}

				_system->copyRectToScreen((byte *)frame->pixels, frame->pitch, x, y, frame->w, frame->h);
				_system->updateScreen();

				if (convertedFrame) {
					convertedFrame->free();
					delete convertedFrame;
				}
			}
		}

		Common::Event event;
		while (_system->getEventManager()->pollEvent(event)) {
			switch (event.type) {
			case Common::EVENT_KEYDOWN:
				if (event.kbd.keycode == Common::KEYCODE_ESCAPE)
					continuePlaying = false;
				break;
			default:
				break;
			}
		}

		_system->updateScreen();
		_system->delayMillis(10);
	}
}

void JMPEngine::loadMandatoryEXE(Common::String filename) {
	Common::NEResources *exe = new Common::NEResources();
	if (exe->loadFromEXE(filename)) {
		_exeFiles.push_back(exe);
		return;
	}

	// No dice on the uncompressed version, let's try compressed as a
	// last resort.
	Common::String compFileName = filename;
	compFileName.deleteLastChar();
	compFileName += '_';

	if (exe->loadFromCompressedEXE(compFileName)) {
		_exeFiles.push_back(exe);
		return;
	}

	error("Could not open exe '%s'", filename.c_str());
}

Common::SeekableReadStream *JMPEngine::getEXEResource(uint16 type, Common::WinResourceID id) {
	for (uint32 i = 0; i < _exeFiles.size(); i++) {
		Common::SeekableReadStream *stream = _exeFiles[i]->getResource(type, id);

		if (stream)
			return stream;
	}

	return 0;
}

Common::StringArray JMPEngine::getStringResource(Common::WinResourceID id) {
	Common::StringArray stringArray;

	Common::SeekableReadStream *stream = getEXEResource(Common::kNEString, id);
	if (!stream)
		return stringArray;

	while (stream->pos() < stream->size()) {
		Common::String string;
		byte length = stream->readByte();

		while (length--)
			string += stream->readByte();

		stringArray.push_back(string);
	}

	delete stream;
	return stringArray;
}

} // End of namespace JMP
