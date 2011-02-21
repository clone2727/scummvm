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

#include "jmp/sound.h"

#include "common/file.h"
#include "common/types.h"
#include "audio/decoders/wave.h"

namespace JMP {

Sound::Sound(JMPEngine* vm) : _vm(vm) {
}
        
void Sound::playSound(Common::String filename, bool loop) {
	Common::File *file = new Common::File();
	if (!file->open(filename.c_str()))
		error ("Could not open file \'%s\'", filename.c_str());

	playSound(file, loop);
}

void Sound::playSound(Common::SeekableReadStream *stream, bool loop) {
	assert(stream);

	SndHandle *handle = getHandle();
	handle->type = kUsedHandle;

	Audio::AudioStream *audStream = Audio::makeWAVStream(stream, DisposeAfterUse::YES);

	if (audStream) {
		if (loop)
			audStream = Audio::makeLoopingAudioStream((Audio::RewindableAudioStream *)audStream, 0); 

		_vm->_mixer->playStream(Audio::Mixer::kPlainSoundType, &handle->handle, audStream);
	}
}

SndHandle *Sound::getHandle() {
	for (int i = 0; i < SOUND_HANDLES; i++) {
		if (_handles[i].type == kFreeHandle)
			return &_handles[i];

		if (!_vm->_mixer->isSoundHandleActive(_handles[i].handle)) {
			_handles[i].type = kFreeHandle;
			return &_handles[i];
		}
	}

	error("Sound::getHandle(): Too many sound handles");

	return NULL;
}

bool Sound::isPlaying() {
	for (int i = 0; i < SOUND_HANDLES; i++)
		if (_handles[i].type == kUsedHandle)
			if (_vm->_mixer->isSoundHandleActive(_handles[i].handle))
				return true;
	return false;
}

void Sound::stopSound() {
	_vm->_mixer->stopAll();
}

void Sound::pauseSound() {
	_vm->_mixer->pauseAll(true);
}

void Sound::resumeSound() {
	_vm->_mixer->pauseAll(false);
}

} // End of namespace JMP
