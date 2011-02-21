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
#include "common/file.h"

#include "base/plugins.h"
#include "base/version.h"

#include "jmp/jmp.h"

#include "video/avi_decoder.h"

// Ouch, need to fix the tables to not use Common::String's
// Including this will cause a segfault upon quitting
//#include "jmp/text.h"

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
	Video::VideoDecoder *video = new Video::AviDecoder(_mixer);

	if (!video->loadFile(filename)) {
		delete video;
		return;
	}

	playVideo(video, x, y);
	delete video;
}

void JMPEngine::playVideoCentered(Common::String filename) {
	Video::VideoDecoder *video = new Video::AviDecoder(_mixer);

	if (!video->loadFile(filename)) {
		delete video;
		return;
	}

	playVideo(video, (_system->getWidth() - video->getWidth()) / 2, (_system->getHeight() - video->getHeight()) / 2);
	delete video;
}

void JMPEngine::playVideo(Video::VideoDecoder *video, uint16 x, uint16 y) {
	bool continuePlaying = true;

	while (!shouldQuit() && !video->endOfVideo() && continuePlaying) {		
		if (video->needsUpdate()) {
			const Graphics::Surface *frame = video->decodeNextFrame();
			Graphics::Surface *convertedFrame = 0;

			if (frame) {
				if (frame->bytesPerPixel == 1) {
					convertedFrame = new Graphics::Surface();
					convertedFrame->create(frame->w, frame->h, _system->getScreenFormat().bytesPerPixel);
					const byte *palette = video->getPalette();
					assert(palette);

					for (uint16 j = 0; j < frame->h; j++) {
						for (uint16 k = 0; k < frame->w; k++) {
							byte palIndex = *((byte *)frame->getBasePtr(k, j));
							byte r = palette[palIndex * 3];
							byte g = palette[palIndex * 3 + 1];
							byte b = palette[palIndex * 3 + 2];
							if (_system->getScreenFormat().bytesPerPixel == 2)
								*((uint16 *)convertedFrame->getBasePtr(k, j)) = _system->getScreenFormat().RGBToColor(r, g, b);
							else
								*((uint32 *)convertedFrame->getBasePtr(k, j)) = _system->getScreenFormat().RGBToColor(r, g, b);
						}
					}

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

	// No dice on the uncompressed version, let's try compressed
	Common::String compFileName = filename;
	compFileName.deleteLastChar();
	compFileName += '_';

	if (exe->loadFromCompressedEXE(compFileName)) {
		_exeFiles.push_back(exe);
		return;
	}

	error("Could not open exe '%s'", filename.c_str());
}

Common::Array<Common::NECursorGroup> JMPEngine::getCursorGroups() {
	for (uint32 i = 0; i < _exeFiles.size(); i++) {
		Common::Array<Common::NECursorGroup> group = _exeFiles[i]->getCursors();
		if (!group.empty())
			return group;
	}

	return Common::Array<Common::NECursorGroup>();
}

Common::SeekableReadStream *JMPEngine::getEXEResource(uint16 type, Common::NEResourceID id) {
	for (uint32 i = 0; i < _exeFiles.size(); i++) {
		Common::SeekableReadStream *stream = _exeFiles[i]->getResource(type, id);

		if (stream)
			return stream;
	}

	return 0;
}

} // End of namespace JMP
