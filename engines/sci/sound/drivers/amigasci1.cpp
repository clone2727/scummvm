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

#include "audio/softsynth/emumidi.h"
#include "sci/sound/drivers/mididriver.h"
#include "sci/resource.h"

#include "common/debug-channels.h"
#include "common/file.h"
#include "common/frac.h"
#include "common/memstream.h"
#include "common/system.h"
#include "common/textconsole.h"
#include "common/util.h"
#include "audio/mods/paula.h"

namespace Sci {

static const byte envSpeedToStep[32] = {
	0x40, 0x32, 0x24, 0x18, 0x14, 0x0f, 0x0d, 0x0b, 0x09, 0x08, 0x07, 0x06, 0x05, 0x0a, 0x04, 0x03,
	0x05, 0x02, 0x03, 0x0b, 0x05, 0x09, 0x09, 0x01, 0x02, 0x03, 0x07, 0x05, 0x04, 0x03, 0x03, 0x02
};

static const byte envSpeedToSkip[32] = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00,
	0x01, 0x00, 0x01, 0x07, 0x02, 0x05, 0x07, 0x00, 0x01, 0x02, 0x08, 0x08, 0x08, 0x09, 0x0e, 0x0b
};

static const byte noteToOctave[256] = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01,
	0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02,
	0x02, 0x02, 0x02, 0x02, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03,
	0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x05, 0x05, 0x05, 0x05,
	0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06,
	0x06, 0x06, 0x06, 0x06, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07,
	0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x09, 0x09, 0x09, 0x09,
	0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x0a, 0x0a, 0x0a, 0x0a, 0x0a, 0x0a, 0x0a, 0x0a,
	0x0a, 0x0a, 0x0a, 0x0a, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b,
	0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0d, 0x0d, 0x0d, 0x0d,
	0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0e, 0x0e, 0x0e, 0x0e, 0x0e, 0x0e, 0x0e, 0x0e,
	0x0e, 0x0e, 0x0e, 0x0e, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f,
	0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x11, 0x11, 0x11, 0x11,
	0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x12, 0x12, 0x12, 0x12, 0x12, 0x12, 0x12, 0x12,
	0x12, 0x12, 0x12, 0x12, 0x13, 0x13, 0x13, 0x13, 0x13, 0x13, 0x13, 0x13, 0x13, 0x13, 0x13, 0x13,
	0x14, 0x14, 0x14, 0x14, 0x14, 0x14, 0x14, 0x14, 0x14, 0x14, 0x14, 0x14, 0x15, 0x15, 0x15, 0x00
};

static const int16 pitchToSemitone[97] = {
	-12, -12, -12, -12, -11, -11, -11, -11,
	-10, -10, -10, -10, -9, -9, -9, -9,
	-8, -8, -8, -8, -7, -7, -7, -7,
	-6, -6, -6, -6, -5, -5, -5, -5,
	-4, -4, -4, -4, -3, -3, -3, -3,
	-2, -2, -2, -2, -1, -1, -1, -1,
	0, 0, 0, 0, 1, 1, 1, 1,
	2, 2, 2, 2, 3, 3, 3, 3,
	4, 4, 4, 4, 5, 5, 5, 5,
	6, 6, 6, 6, 7, 7, 7, 7,
	8, 8, 8, 8, 9, 9, 9, 9,
	10, 10, 10, 10, 11, 11, 11, 11,
	12
};

static const uint16 pitchToSemiRem[97] = {
	0x0000, 0x0004, 0x0008, 0x000c, 0x0000, 0x0004, 0x0008, 0x000c,
	0x0000, 0x0004, 0x0008, 0x000c, 0x0000, 0x0004, 0x0008, 0x000c,
	0x0000, 0x0004, 0x0008, 0x000c, 0x0000, 0x0004, 0x0008, 0x000c,
	0x0000, 0x0004, 0x0008, 0x000c, 0x0000, 0x0004, 0x0008, 0x000c,
	0x0000, 0x0004, 0x0008, 0x000c, 0x0000, 0x0004, 0x0008, 0x000c,
	0x0000, 0x0004, 0x0008, 0x000c, 0x0000, 0x0004, 0x0008, 0x000c,
	0x0000, 0x0004, 0x0008, 0x000c, 0x0000, 0x0004, 0x0008, 0x000c,
	0x0000, 0x0004, 0x0008, 0x000c, 0x0000, 0x0004, 0x0008, 0x000c,
	0x0000, 0x0004, 0x0008, 0x000c, 0x0000, 0x0004, 0x0008, 0x000c,
	0x0000, 0x0004, 0x0008, 0x000c, 0x0000, 0x0004, 0x0008, 0x000c,
	0x0000, 0x0004, 0x0008, 0x000c, 0x0000, 0x0004, 0x0008, 0x000c,
	0x0000, 0x0004, 0x0008, 0x000c, 0x0000, 0x0004, 0x0008, 0x000c,
	0x0000
};

static const byte velocityMap[64] = {
	0x01, 0x02, 0x03, 0x03, 0x04, 0x05, 0x05, 0x06, 0x07, 0x08, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d,
	0x0e, 0x0f, 0x10, 0x11, 0x12, 0x13, 0x14, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c,
	0x1d, 0x1e, 0x1f, 0x20, 0x21, 0x22, 0x23, 0x24, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2a, 0x2a,
	0x2b, 0x2c, 0x2d, 0x2e, 0x2f, 0x30, 0x31, 0x32, 0x34, 0x35, 0x37, 0x39, 0x3a, 0x3c, 0x3e, 0x40
};

class MidiDriver_AmigaSci1 : public MidiDriver, public Audio::Paula {
public:
	enum {
		kVoices = 10,
		kBufSize = 224
	};

	enum kEnvState {
		kEnvStateAttack,
		kEnvStateDecay,
		kEnvStateSustain,
		kEnvStateRelease
	};

	MidiDriver_AmigaSci1(Audio::Mixer *mixer);
	virtual ~MidiDriver_AmigaSci1();

	// MidiDriver
	int open();
	void close();
	void send(uint32 b);
	MidiChannel *allocateChannel() { return NULL; }
	MidiChannel *getPercussionChannel() { return NULL; }
	void setTimerCallback(void *timer_param, Common::TimerManager::TimerProc timer_proc);
	uint32 getBaseTempo() { return 1000000 / _baseFreq; }
	bool isOpen() const { return _isOpen; }
	virtual uint32 property(int prop, uint32 param) { return 0; }

	// Audio::Paula
	void interrupt();

	void setVolume(byte volume) { }
	void playSwitch(bool play) { }

private:
	enum {
		kModeLoop = 1 << 0 // Instrument looping flag
	};

	Audio::Mixer *_mixer;
	Audio::SoundHandle _mixerSoundHandle;
	Common::TimerManager::TimerProc _timerProc;
	void *_timerParam;
	bool _playSwitch;
	bool _isOpen;
	int _baseFreq;
	uint _masterVolume;
	uint32 _timerThreshold;
	uint32 _timerIncrease;
	uint32 _timerCounter;

	bool loadPatches(Common::SeekableReadStream &file);
	void voiceOn(byte voice, int8 note, int8 velocity);
	void voiceOff(byte voice);
	int8 findVoice(int8 channel);
	void voiceMapping(int8 channel, byte voices);
	void assignVoices(int8 channel, byte voices);
	void releaseVoices(int8 channel, byte voices);
	void donateVoices();
	frac_t calcStep(int8 note, byte voice, byte *noteRange, byte *wave, byte *stepTable);
	bool calcVoiceStep(byte voice);
	void noteOn(int8 channel, int8 note, int8 velocity);
	void noteOff(int8 channel, int8 note);
	void changePatch(int8 channel, int8 patch);
	void holdPedal(int8 channel, int8 pedal);
	void setPitchWheel(int8 channel, uint16 pitch);
	void calcMixVelocity(int8 voice);
	void processEnvelope(int8 voice);
	void initVoice(byte voice);
	void copyBuffer(byte voice, uint idx);
	void audioInterrupt(byte voice);

	int8 _voiceChannel[kVoices];
	bool _voiceReleased[kVoices];
	bool _voiceSustained[kVoices];
	int8 _voiceEnvCurVel[kVoices];
	kEnvState _voiceEnvState[kVoices];
	byte _voiceEnvCntDown[kVoices];
	frac_t _voicePos[kVoices];
	uint16 _voiceTicks[kVoices];
	uint16 _voiceReleaseTicks[kVoices];
	byte *_voicePatch[kVoices];
	byte *_voiceNoteRange[kVoices];
	byte *_voiceWave[kVoices];
	byte *_voiceStepTable[kVoices];
	const byte *_voiceVelocityAdjust[kVoices];
	byte _voiceVelocity[kVoices];
	int8 _voiceNote[kVoices];
	bool _voiceOn[kVoices];
	byte _voiceMixVelocity[kVoices];
	frac_t _voiceStep[kVoices];
	byte *_voiceBuf[kVoices];
	uint16 _voiceBufFree[kVoices][2];
	int8 _voiceNextBuffer[kVoices];
	bool _voiceIsLastBuffer[kVoices];
	int _voiceLastInt[kVoices];
	bool _voiceIsLooping[kVoices];

	int8 _chanPatch[MIDI_CHANNELS];
	uint16 _chanPitch[MIDI_CHANNELS];
	bool _chanHold[MIDI_CHANNELS];
	int8 _chanVolume[MIDI_CHANNELS];
	int8 _chanLastVoice[MIDI_CHANNELS];
	byte _chanExtraVoices[MIDI_CHANNELS];

	Resource *_patch;
};

#define PATCH_NAME 0
#define PATCH_NOTE_RANGE 10

#define WAVE_NAME 0
#define WAVE_IS_SIGNED 8
#define WAVE_PHASE1_START 10
#define WAVE_PHASE1_END 12
#define WAVE_PHASE2_START 14
#define WAVE_PHASE2_END 16
#define WAVE_NATIVE_NOTE 18
#define WAVE_STEP_TABLE_OFFSET 20
#define WAVE_SIZEOF 24

#define NOTE_RANGE_SIZE 20
#define NOTE_RANGE_START_NOTE 0
#define NOTE_RANGE_END_NOTE 2
#define NOTE_RANGE_SAMPLE_OFFSET 4
#define NOTE_RANGE_TRANSPOSE 8
#define NOTE_RANGE_ATTACK_SPEED 10
#define NOTE_RANGE_ATTACK_TARGET 11
#define NOTE_RANGE_DECAY_SPEED 12
#define NOTE_RANGE_DECAY_TARGET 13
#define NOTE_RANGE_RELEASE_SPEED 14
#define NOTE_RANGE_FIXED_NOTE 16
#define NOTE_RANGE_LOOP 18

#define INTERRUPT_FREQ 180 // Needs to be >= 130 for double buffering code

MidiDriver_AmigaSci1::MidiDriver_AmigaSci1(Audio::Mixer *mixer) :
	Audio::Paula(true, mixer->getOutputRate(), mixer->getOutputRate() / INTERRUPT_FREQ),
	_mixer(mixer),
	_timerProc(nullptr),
	_timerParam(nullptr),
	_playSwitch(true),
	_isOpen(false),
	_baseFreq(INTERRUPT_FREQ),
	_masterVolume(15),
	_patch(0),
	_timerThreshold(16667),
	_timerCounter(0) {

	_timerIncrease = getBaseTempo();

	for (uint i = 0; i < kVoices; ++i) {
		_voiceChannel[i] = -1;
		_voiceReleased[i] = false;
		_voiceSustained[i] = false;
		_voiceEnvCurVel[i] = 0;
		_voiceEnvState[i] = kEnvStateAttack;
		_voiceEnvCntDown[i] = 0;
		_voicePos[i] = 0;
		_voiceTicks[i] = 0;
		_voiceReleaseTicks[i] = 0;
		_voicePatch[i] = 0;
		_voiceNoteRange[i] = 0;
		_voiceWave[i] = 0;
		_voiceStepTable[i] = 0;
//		_voiceVelocityAdjust[i] = velocityAdjust;
		_voiceVelocity[i] = 0;
		_voiceNote[i] = -1;
		_voiceOn[i] = false;
		_voiceMixVelocity[i] = 0;
		_voiceStep[i] = 0;
		_voiceBuf[i] = new byte[kBufSize * 2];
		_voiceBufFree[i][0] = 0;
		_voiceBufFree[i][1] = 0;
		_voiceNextBuffer[i] = 0;
		_voiceIsLastBuffer[i] = false;
		_voiceLastInt[i] = 1;
		_voiceIsLooping[i] = false;
	}

	for (uint i = 0; i < MIDI_CHANNELS; ++i) {
		_chanPatch[i] = 0;
		_chanPitch[i] = 0x2000;
		_chanHold[i] = false;
		_chanVolume[i] = 63;
		_chanLastVoice[i] = 0;
		_chanExtraVoices[i] = 0;
	}
}

MidiDriver_AmigaSci1::~MidiDriver_AmigaSci1() {
	for (uint i = 0; i < kVoices; ++i) {
		delete[] _voiceBuf[i];
	}
}

int MidiDriver_AmigaSci1::open() {
	_patch = g_sci->getResMan()->findResource(ResourceId(kResourceTypePatch, 9), true);
	if (!_patch) {
		warning("Could not open patch for Amiga SCI1 sound driver");
		return Common::kUnknownError;
	}

	_mixer->playStream(Audio::Mixer::kPlainSoundType, &_mixerSoundHandle, this, -1, _mixer->kMaxChannelVolume, 0, DisposeAfterUse::NO);

	return Common::kNoError;
}

void MidiDriver_AmigaSci1::close() {
	g_sci->getResMan()->unlockResource(_patch);
}

void MidiDriver_AmigaSci1::processEnvelope(int8 voice) {
	byte attackTarget = _voiceNoteRange[voice][NOTE_RANGE_ATTACK_TARGET];
	byte decayTarget = _voiceNoteRange[voice][NOTE_RANGE_DECAY_TARGET];

	if (READ_BE_UINT16(_voiceNoteRange[voice] + NOTE_RANGE_LOOP) != 0) {
		_voiceEnvCurVel[voice] = attackTarget;
		return;
	}

	if (_voiceReleased[voice])
		_voiceEnvState[voice] = kEnvStateRelease;

	switch(_voiceEnvState[voice]) {
	case kEnvStateAttack: {
		if (_voiceEnvCntDown[voice] != 0) {
			--_voiceEnvCntDown[voice];
			return;
		}
		byte attackSpeed = _voiceNoteRange[voice][NOTE_RANGE_ATTACK_SPEED];
		_voiceEnvCntDown[voice] = envSpeedToSkip[attackSpeed];
		_voiceEnvCurVel[voice] += envSpeedToStep[attackSpeed];
		if (_voiceEnvCurVel[voice] >= attackTarget) {
			_voiceEnvCurVel[voice] = attackTarget;
			_voiceEnvState[voice] = kEnvStateDecay;
		}
		break;
	}
	case kEnvStateDecay: {
		if (_voiceEnvCntDown[voice] != 0) {
			--_voiceEnvCntDown[voice];
			return;
		}
		byte decaySpeed = _voiceNoteRange[voice][NOTE_RANGE_DECAY_SPEED];
		_voiceEnvCntDown[voice] = envSpeedToSkip[decaySpeed];
		_voiceEnvCurVel[voice] -= envSpeedToStep[decaySpeed];
		if (_voiceEnvCurVel[voice] <= decayTarget) {
			_voiceEnvCurVel[voice] = decayTarget;
			_voiceEnvState[voice] = kEnvStateSustain;
		}
		break;
	}
	case kEnvStateSustain:
		_voiceEnvCurVel[voice] = decayTarget;
		break;
	case kEnvStateRelease: {
		if (_voiceEnvCntDown[voice] != 0) {
			--_voiceEnvCntDown[voice];
			return;
		}
		byte releaseSpeed = _voiceNoteRange[voice][NOTE_RANGE_RELEASE_SPEED];
		_voiceEnvCntDown[voice] = envSpeedToSkip[releaseSpeed];
		_voiceEnvCurVel[voice] -= envSpeedToStep[releaseSpeed];
		if (_voiceEnvCurVel[voice] <= 0)
			voiceOff(voice);
	}
	}
}

void MidiDriver_AmigaSci1::calcMixVelocity(int8 voice) {
	byte chanVol = _chanVolume[_voiceChannel[voice]];
	byte voiceVelocity = _voiceVelocity[voice];

	if (chanVol != 0) {
		if (voiceVelocity != 0) {
			voiceVelocity = voiceVelocity * chanVol / 63;
			if (_voiceEnvCurVel[voice] != 0) {
				voiceVelocity = voiceVelocity * _voiceEnvCurVel[voice] / 63;
				if (_masterVolume != 0) {
					voiceVelocity = voiceVelocity * (_masterVolume << 2) / 63;
					if (voiceVelocity == 0)
						++voiceVelocity;
				} else {
					voiceVelocity = 0;
				}
			} else {
				voiceVelocity = 0;
			}
		}
	} else {
		voiceVelocity = 0;
	}

	if (!_playSwitch)
		voiceVelocity = 0;

	_voiceMixVelocity[voice] = voiceVelocity;
//	_voiceVelocityAdjust[voice] = velocityAdjust + (voiceVelocity << 8);
}

void MidiDriver_AmigaSci1::audioInterrupt(byte voice) {
	if (_voiceNextBuffer[voice] == -1)
		voiceOff(voice);
	else {
		if (_voiceIsLastBuffer[voice])
			_voiceNextBuffer[voice] = -1;
		else
			copyBuffer(voice, _voiceNextBuffer[voice]);
	}
}

void MidiDriver_AmigaSci1::interrupt() {
	_timerCounter += _timerIncrease;

	if (_timerCounter < _timerThreshold)
		return;

	if (_timerProc)
		(*_timerProc)(_timerParam);

	for (uint i = 0; i < kVoices; ++i) {
		uint curBuffer = getChannelOffset(i).int_off < kBufSize;
		if (curBuffer != _voiceLastInt[i]) {
			audioInterrupt(i);
			_voiceLastInt[i] = curBuffer;
		}
	}

	for (uint i = 0; i < kVoices; ++i) {
		if (_voiceNote[i] != -1) {
			++_voiceTicks[i];
			if (_voiceReleased[i])
				++_voiceReleaseTicks[i];
			processEnvelope(i);
			calcMixVelocity(i);
		}
	}

	_timerCounter -= _timerThreshold;
}

#if 0
// Voice mapping version (not supported by the interpreter at this time)
int8 MidiDriver_AmigaSci1::findVoice(int8 channel) {
	int8 voice = _chanLastVoice[channel];
	uint16 maxTicks = 0;
	int8 maxTicksVoice = -1;

	do {
		voice = (voice + 1) % kVoices;

		if (_voiceChannel[voice] == channel) {
			if (_voiceNote[voice] == -1) {
				_chanLastVoice[channel] = voice;
				return voice;
			}
			uint16 ticks;
			if (_voiceReleaseTicks[voice] != 0)
				ticks = _voiceReleaseTicks[voice] + 0x8000;
			else
				ticks = _voiceReleaseTicks[voice];

			if (ticks >= maxTicks) {
				maxTicks = ticks;
				maxTicksVoice = voice;
			}
		}
	} while (voice != _chanLastVoice[channel]);

	if (maxTicksVoice != -1) {
		voiceOff(maxTicksVoice);
		_chanLastVoice[channel] = maxTicksVoice;
		return maxTicksVoice;
	}

	return -1;
}
#endif

int8 MidiDriver_AmigaSci1::findVoice(int8 channel) {
	uint16 maxTicks = 0;
	int8 maxTicksVoice = -1;

	for (int8 voice = 0; voice < kVoices; ++voice) {
		if (_voiceNote[voice] == -1) {
			_voiceChannel[voice] = channel;
			return voice;
		}
		uint16 ticks;
		if (_voiceReleaseTicks[voice] != 0)
			ticks = _voiceReleaseTicks[voice] + 0x8000;
		else
			ticks = _voiceReleaseTicks[voice];

		if (ticks >= maxTicks) {
			maxTicks = ticks;
			maxTicksVoice = voice;
		}
	}

	if (maxTicksVoice != -1) {
		voiceOff(maxTicksVoice);
		_voiceChannel[maxTicksVoice] = channel;
		return maxTicksVoice;
	}

	return -1;
}

void MidiDriver_AmigaSci1::voiceMapping(int8 channel, byte voices) {
	int curVoices = 0;

	for (int i = 0; i < kVoices; i++)
		if (_voiceChannel[i] == channel)
			curVoices++;

	curVoices += _chanExtraVoices[channel];

	if (curVoices < voices)
		assignVoices(channel, voices - curVoices);
	else if (curVoices > voices) {
		releaseVoices(channel, curVoices - voices);
		donateVoices();
	}
}

void MidiDriver_AmigaSci1::assignVoices(int8 channel, byte voices) {
	for (int i = 0; i < kVoices; i++)
		if (_voiceChannel[i] == -1) {
			_voiceChannel[i] = channel;

			if (_voiceNote[i] != -1)
				voiceOff(i);

			if (--voices == 0)
				break;
		}

	_chanExtraVoices[channel] += voices;
}

void MidiDriver_AmigaSci1::releaseVoices(int8 channel, byte voices) {
	if (_chanExtraVoices[channel] >= voices) {
		_chanExtraVoices[channel] -= voices;
		return;
	}

	voices -= _chanExtraVoices[channel];
	_chanExtraVoices[channel] = 0;

	for (int i = 0; i < kVoices; i++) {
		if ((_voiceChannel[i] == channel) && (_voiceNote[i] == -1)) {
			_voiceChannel[i] = -1;
			if (--voices == 0)
				return;
		}
	}

	do {
		uint16 maxTicks = 0;
		int8 maxTicksVoice = 0;

		for (int i = 0; i < kVoices; i++) {
			if (_voiceChannel[i] == channel) {
				// The original code seems to be broken here. It reads a word value from
				// byte array _voiceSustained.
				uint16 ticks = _voiceReleaseTicks[i];
				if (ticks > 0)
					ticks += 0x8000;
				else
					ticks = _voiceTicks[i];

				if (ticks >= maxTicks) {
					maxTicks = ticks;
					maxTicksVoice = i;
				}
			}
		}
		_voiceSustained[maxTicksVoice] = false;
		voiceOff(maxTicksVoice);
		_voiceChannel[maxTicksVoice] = -1;
	} while (--voices > 0);
}

void MidiDriver_AmigaSci1::donateVoices() {
	int freeVoices = 0;

	for (int i = 0; i < kVoices; i++)
		if (_voiceChannel[i] == -1)
			freeVoices++;

	if (freeVoices == 0)
		return;

	for (int i = 0; i < MIDI_CHANNELS; i++) {
		if (_chanExtraVoices[i] != 0) {
			if (_chanExtraVoices[i] >= freeVoices) {
				_chanExtraVoices[i] -= freeVoices;
				assignVoices(i, freeVoices);
				return;
			} else {
				freeVoices -= _chanExtraVoices[i];
				byte extraVoices = _chanExtraVoices[i];
				_chanExtraVoices[i] = 0;
				assignVoices(i, extraVoices);
			}
		}
	}
}

void MidiDriver_AmigaSci1::copyBuffer(byte voice, uint idx) {
	byte *buf = _voiceBuf[voice];

	if (idx != 0)
		buf += kBufSize;

	uint16 offset = fracToInt(_voicePos[voice]) & 0xfffe;
	_voiceBufFree[voice][idx] = 224 >> 1;
	byte *samples = _voiceWave[voice] + WAVE_SIZEOF + offset;

	for (uint i = 0; i < 224; ++i)
		*buf++ = *samples++;

	uint16 phase2End = READ_BE_UINT16(_voiceWave[voice] + WAVE_PHASE2_END);
	uint16 endOffset = phase2End;

	if (endOffset == 0)
		endOffset = READ_BE_UINT16(_voiceWave[voice] + WAVE_PHASE1_END);

	offset += 224;	

	if (endOffset <= offset) {
		if (phase2End == 0 || READ_BE_UINT16(_voiceNoteRange[voice] + NOTE_RANGE_LOOP) != 0) {
			offset -= endOffset;
			_voiceBufFree[voice][idx] = (224 - offset) >> 1;
			_voiceIsLastBuffer[voice] = true;
		} else {
			_voiceIsLooping[voice] = true;
			uint16 val = endOffset - READ_BE_UINT16(_voiceWave[voice] + WAVE_PHASE2_START) + 1;
			do {
				offset -= val;
			} while (offset > endOffset);
			offset &= 0xfffe;
		}
	}

	_voicePos[voice] = intToFrac(offset) | (_voicePos[voice] & FRAC_LO_MASK);
	_voiceNextBuffer[voice] = (idx + 1) % 2;
	// AUDO0LC = _voiceBuf[voice][idx]
}

void MidiDriver_AmigaSci1::initVoice(byte voice) {
	_voiceNextBuffer[voice] = 0;
	_voiceIsLastBuffer[voice] = false;
	_voiceLastInt[voice] = 1;
	setChannelVolume(voice, 0);
	// CopyBuffer1
}

void MidiDriver_AmigaSci1::voiceOn(byte voice, int8 note, int8 velocity) {
	_voiceReleased[voice] = false;
	_voiceEnvCurVel[voice] = 0;
	_voiceEnvState[voice] = kEnvStateAttack;
	_voiceEnvCntDown[voice] = 0;
	_voiceTicks[voice] = 0;
	_voiceReleaseTicks[voice] = 0;

	int8 patchId = _chanPatch[_voiceChannel[voice]];
	uint32 offset = READ_BE_UINT32(_patch->data + patchId * 4);

	if (offset == 0)
		return;

	byte *patch = _patch->data + offset;
	byte *noteRange = patch + PATCH_NOTE_RANGE;

	while (1) {
		int16 startNote = READ_BE_UINT16(noteRange + NOTE_RANGE_START_NOTE);

		if (startNote == -1)
			return;

		int16 endNote = READ_BE_UINT16(noteRange + NOTE_RANGE_END_NOTE);

		if (startNote <= note && note <= endNote)
			break;

		noteRange += NOTE_RANGE_SIZE;
	}

	byte *wave = _patch->data + READ_BE_UINT32(noteRange + NOTE_RANGE_SAMPLE_OFFSET);
	byte *stepTable = _patch->data + READ_BE_UINT32(wave + WAVE_STEP_TABLE_OFFSET) + 16;

	_voicePatch[voice] = patch;
	_voiceNoteRange[voice] = noteRange;
	_voiceWave[voice] = wave;
	_voiceStepTable[voice] = stepTable;
	_voicePos[voice] = intToFrac(READ_BE_UINT16(wave + WAVE_PHASE1_START));

	if (velocity != 0)
		velocity = velocityMap[velocity >> 1];

	_voiceVelocity[voice] = velocity;
	_voiceNote[voice] = note;

	if (!calcVoiceStep(voice))
		_voiceNote[voice] = -1;
	else {
//		_voiceVelocityAdjust[voice] = velocityAdjust;
		_voiceOn[voice] = true;
	}
}

void MidiDriver_AmigaSci1::voiceOff(byte voice) {
	_voiceOn[voice] = false;
	_voiceVelocity[voice] = 0;
	_voiceNote[voice] = -1;
	_voiceSustained[voice] = false;
	_voiceReleased[voice] = false;
	_voiceEnvState[voice] = kEnvStateAttack;
	_voiceEnvCntDown[voice] = 0;
	_voiceTicks[voice] = 0;
	_voiceReleaseTicks[voice] = 0;
}

frac_t MidiDriver_AmigaSci1::calcStep(int8 note, byte voice, byte *noteRange, byte *wave, byte *stepTable) {
	uint16 noteAdj = note + 127 - READ_BE_UINT16(wave + WAVE_NATIVE_NOTE);
	byte channel = _voiceChannel[voice];
	uint16 pitch = _chanPitch[channel];
	pitch /= 170;
	noteAdj += pitchToSemitone[pitch];
	byte offset = pitchToSemiRem[pitch];
	int octave = noteToOctave[noteAdj];

	while (noteAdj < 243)
		noteAdj += 12;
	noteAdj -= 243;

	frac_t step = READ_BE_UINT32(stepTable + (noteAdj << 4) + offset);

	int16 transpose = READ_BE_UINT16(noteRange + NOTE_RANGE_TRANSPOSE);
	if (transpose > 0) {
		frac_t delta = READ_BE_UINT32(stepTable + (noteAdj << 4) + offset + 16) - step;
		delta >>= 4;
		delta >>= octave;
		delta *= transpose;
		step >>= octave;
		step += delta;
	} else if (transpose < 0) {
		frac_t delta = step - READ_BE_UINT32(stepTable + (noteAdj << 4) + offset - 16);
		delta >>= 4;
		delta >>= octave;
		delta *= -transpose;
		step >>= octave;
		step -= delta;
	} else {
		if (octave != 0)
			step >>= octave;
	}

	return step;
}

bool MidiDriver_AmigaSci1::calcVoiceStep(byte voice) {
	int8 note = _voiceNote[voice];
	byte *noteRange = _voiceNoteRange[voice];
	byte *wave = _voiceWave[voice];
	byte *stepTable = _voiceStepTable[voice];

	int16 fixedNote = READ_BE_UINT16(noteRange + NOTE_RANGE_FIXED_NOTE);
	if (fixedNote != -1)
		note = fixedNote;

	frac_t step = calcStep(note, voice, noteRange, wave, stepTable);
	if (step == -1)
		return false;

	_voiceStep[voice] = step;
	return true;
}

void MidiDriver_AmigaSci1::noteOn(int8 channel, int8 note, int8 velocity) {
	if (velocity == 0) {
		noteOff(channel, note);
		return;
	}

	for (uint i = 0; i < kVoices; i++) {
		if (_voiceChannel[i] == channel && _voiceNote[i] == note) {
			_voiceSustained[i] = false;
			voiceOff(i);
			voiceOn(i, note, velocity);
			return;
		}
	}

	int8 voice = findVoice(channel);
	if (voice != -1)
		voiceOn(voice, note, velocity);
}

void MidiDriver_AmigaSci1::noteOff(int8 channel, int8 note) {
	for (uint i = 0; i < kVoices; i++) {
		if (_voiceChannel[i] == channel && _voiceNote[i] == note) {
			if (_chanHold[channel])
				_voiceSustained[i] = true;
			else {
				_voiceReleased[i] = true;
				_voiceEnvCntDown[i] = 0;
			}
			return;
		}
	}
}

void MidiDriver_AmigaSci1::changePatch(int8 channel, int8 patch) {
	_chanPatch[channel] = patch;
}

void MidiDriver_AmigaSci1::holdPedal(int8 channel, int8 pedal) {
	_chanHold[channel] = pedal;

	if (pedal != 0)
		return;

	for (uint voice = 0; voice < kVoices; ++voice) {
		if (_voiceChannel[voice] == channel && _voiceSustained[voice]) {
			_voiceSustained[voice] = false;
			_voiceReleased[voice] = true;
		}
	}
}

void MidiDriver_AmigaSci1::setPitchWheel(int8 channel, uint16 pitch) {
	_chanPitch[channel] = pitch;

	for (int i = 0; i < kVoices; i++)
		if (_voiceNote[i] != -1 && _voiceChannel[i] == channel)
			calcVoiceStep(i);
}

void MidiDriver_AmigaSci1::setTimerCallback(void *timer_param, Common::TimerManager::TimerProc timer_proc) {
	_timerProc = timer_proc;
	_timerParam = timer_param;
}

void MidiDriver_AmigaSci1::send(uint32 b) {
	byte command = b & 0xf0;
	byte channel = b & 0xf;
	byte op1 = (b >> 8) & 0xff;
	byte op2 = (b >> 16) & 0xff;

	switch(command) {
	case 0x80:
		noteOff(channel, op1);
		break;
	case 0x90:
		noteOn(channel, op1, op2);
		break;
	case 0xb0:
		switch (op1) {
		case 0x07:
			if (op2 != 0) {
				op2 >>= 1;
				if (op2 == 0)
					++op2;
			}
			_chanVolume[channel] = op2;
			break;
		case 0x40:
			holdPedal(channel, op2);
			break;
		case 0x4b:
			voiceMapping(channel, op2);
			break;
		case 0x7b:
			for (uint voice = 0; voice < kVoices; ++voice) {
				if (_voiceChannel[voice] == channel && _voiceNote[voice] != -1)
					voiceOff(voice);
			}
		}
		break;
	case 0xc0:
		changePatch(channel, op1);
		break;
	case 0xe0:
		setPitchWheel(channel, (op2 << 7) | op1);
		break;
	}
}

class MidiPlayer_AmigaSci1 : public MidiPlayer {
public:
	MidiPlayer_AmigaSci1(SciVersion version) : MidiPlayer(version) { _driver = new MidiDriver_AmigaSci1(g_system->getMixer()); }
	byte getPlayId() const;
	int getPolyphony() const { return MidiDriver_AmigaSci1::kVoices; }
	bool hasRhythmChannel() const { return false; }
	void setVolume(byte volume) { static_cast<MidiDriver_AmigaSci1 *>(_driver)->setVolume(volume); }
	void playSwitch(bool play) { static_cast<MidiDriver_AmigaSci1 *>(_driver)->playSwitch(play); }
	void loadInstrument(int idx, byte *data) { }
};

MidiPlayer *MidiPlayer_AmigaSci1_create(SciVersion version) {
	return new MidiPlayer_AmigaSci1(version);
}

byte MidiPlayer_AmigaSci1::getPlayId() const {
	return 0x06;
}

} // End of namespace Sci
