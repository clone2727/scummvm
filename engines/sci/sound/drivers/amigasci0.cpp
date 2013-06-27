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

#include "common/file.h"
#include "common/frac.h"
#include "common/memstream.h"
#include "common/system.h"
#include "common/textconsole.h"
#include "common/util.h"

namespace Sci {

static const uint32 periodTable[216] = {
	0x3bb93ade, 0x3a053930, 0x385e378f, 0x36c335fa,
	0x35343471, 0x33b032f3, 0x32383180, 0x30ca3017,
	0x2f662eb8, 0x2e0d2d64, 0x2cbd2c19, 0x2b772ad7,
	0x2a3a299f, 0x29072870, 0x27dc274a, 0x26b9262b,
	0x259f2515, 0x248d2406, 0x23822300, 0x227f2201,
	0x21842109, 0x20902019, 0x1fa31f2f, 0x1ebc1e4b,
	0x3bb93ade, 0x3a053930, 0x385e378f, 0x36c335fa,
	0x35343471, 0x33b032f3, 0x32383180, 0x30ca3017,
	0x2f662eb8, 0x2e0d2d64, 0x2cbd2c19, 0x2b772ad7,
	0x2a3a299f, 0x29072870, 0x27dc274a, 0x26b9262b,
	0x259f2515, 0x248d2406, 0x23822300, 0x227f2201,
	0x21842109, 0x20902019, 0x1fa31f2f, 0x1ebc1e4b,
	0x3bb93ade, 0x3a053930, 0x385e378f, 0x36c335fa,
	0x35343471, 0x33b032f3, 0x32383180, 0x30ca3017,
	0x2f662eb8, 0x2e0d2d64, 0x2cbd2c19, 0x2b772ad7,
	0x2a3a299f, 0x29072870, 0x27dc274a, 0x26b9262b,
	0x259f2515, 0x248d2406, 0x23822300, 0x227f2201,
	0x21842109, 0x20902019, 0x1fa31f2f, 0x1ebc1e4b,
	0x1ddc1d6e, 0x1d021c98, 0x1c2f1bc8, 0x1b621afd,
	0x1a9a1a38, 0x19d81979, 0x191c18c0, 0x1865180b,
	0x17b3175c, 0x170616b1, 0x165e160c, 0x15bb156c,
	0x151d14d0, 0x14831438, 0x13ee13a5, 0x135c1315,
	0x12cf128a, 0x12461203, 0x11c11180, 0x11401100,
	0x10c21084, 0x1048100c, 0x0fd10f97, 0x0f5e0f26,
	0x0eee0eb7, 0x0e810e4c, 0x0e170de3, 0x0db10d7e,
	0x0d4d0d1c, 0x0cec0cbd, 0x0c8e0c60, 0x0c320c05,
	0x0bd90bae, 0x0b830b59, 0x0b2f0b06, 0x0add0ab5,
	0x0a8e0a67, 0x0a410a1c, 0x09f709d2, 0x09ae098a,
	0x09670945, 0x09230901, 0x08e008c0, 0x08a00880,
	0x08610842, 0x08240806, 0x07e807cb, 0x07af0793,
	0x0777075b, 0x07400725, 0x070b06f1, 0x06d806bf,
	0x06a6068e, 0x0676065e, 0x06470630, 0x06190602,
	0x05ec05d6, 0x05c105ac, 0x05970583, 0x056e055b,
	0x05470534, 0x0520050e, 0x04fb04e9, 0x04d604c5,
	0x04b304a2, 0x04910480, 0x04700460, 0x04500440,
	0x04300421, 0x04120403, 0x03f403e5, 0x03d703c9,
	0x03bb03ad, 0x03a00392, 0x03850378, 0x036c035f,
	0x03530347, 0x033b032f, 0x03230318, 0x030c0301,
	0x02f602eb, 0x02e002d6, 0x02cb02c1, 0x02b702ad,
	0x02a30299, 0x02900286, 0x027d0274, 0x026b0262,
	0x02590251, 0x02480240, 0x02380230, 0x02280220,
	0x02180210, 0x02090201, 0x01fa01f3, 0x01eb01e4,
	0x01dd01d6, 0x01cf01c9, 0x01c201bc, 0x01b501af,
	0x01a901a3, 0x019d0197, 0x0191018b, 0x01860180,
	0x017b0175, 0x0170016a, 0x01650160, 0x015b0156,
	0x0151014c, 0x01470143, 0x013e0139, 0x01350130,
	0x012c0128, 0x01240120, 0x011c0118, 0x01140110,
	0x010c0108, 0x01040101, 0x00fd00f9, 0x00f500f2,
	0x00ee00eb, 0x00e700e4, 0x00e100de, 0x00da00d7,
	0x00d400d1, 0x00ce00cb, 0x00c800c5, 0x00c200c0,
	0x00bd00ba, 0x00b700b5, 0x00b200af, 0x00ad00aa,
	0x00a800a6, 0x00a300a1, 0x009f009d, 0x009a0098,
	0x00960094, 0x00920090, 0x008e008c, 0x008a0088,
	0x00860084, 0x00820080, 0x007e0000, 0x00000000
};

class MidiDriver_AmigaSci0 : public MidiDriver_Emulated {
public:
	enum {
		kVoices = 4
	};

	enum kEnvState {
		kEnvStateAttack,
		kEnvStateDecay,
		kEnvStateSustain,
		kEnvStateRelease
	};

	MidiDriver_AmigaSci0(Audio::Mixer *mixer);
	virtual ~MidiDriver_AmigaSci0() { }

	// MidiDriver
	int open();
	void close();
	void send(uint32 b);
	MidiChannel *allocateChannel() { return NULL; }
	MidiChannel *getPercussionChannel() { return NULL; }

	// AudioStream
	bool isStereo() const { return false; }
	int getRate() const { return 11127; }

	// MidiDriver_Emulated
	void generateSamples(int16 *buf, int len);
	void onTimer();

	void setVolume(byte volume);
	void playSwitch(bool play) { }
	void onNewSound(uint16 channels);

private:
	bool readInstruments();

	struct Instrument {
		uint16 flags;
		uint16 seg1Size;
		uint32 seg2Offset;
		uint16 seg2Size;
		uint32 seg3Offset;
		uint16 seg3Size;
		byte *samples;
		byte transpose;
		byte envelope[12];
		byte name[31];
	};

	struct {
		char name[30];
		uint16 instrumentCount;
		Instrument *instrument[128];
		uint16 patchNr[128];
	} _bank;

	uint32 _timerThreshold;
	uint32 _timerIncrease;
	uint32 _timerCounter;

	void noteOn(int8 voice, int8 note, int8 velocity);
	void noteOff(int8 voice, int8 note);
	void voiceOff(int8 voice);

	bool _playSwitch;

	byte _voicePatch[kVoices];
	byte _voiceVelocity[kVoices];
	int8 _chanVoice[MIDI_CHANNELS];
	int8 _voiceNote[kVoices];
};

MidiDriver_AmigaSci0::MidiDriver_AmigaSci0(Audio::Mixer *mixer) : MidiDriver_Emulated(mixer),
	_playSwitch(true), _timerCounter(0), _timerThreshold(16667) {

	_timerIncrease = getBaseTempo();
}

int MidiDriver_AmigaSci0::open() {
	if (!readInstruments()) {
		warning("Could not read patch data from bank.001");
		return Common::kUnknownError;
	}

	return Common::kUnknownError;
}

void MidiDriver_AmigaSci0::close() {
}

void MidiDriver_AmigaSci0::generateSamples(int16 *data, int len) {
}

void MidiDriver_AmigaSci0::onNewSound(uint16 channels) {
	uint voices = 0;

	for (uint i = 0; i < MIDI_CHANNELS; ++i) {
		if (channels & (1 << i))
			_chanVoice[i] = voices++;
		else
			_chanVoice[i] = -1;

		if (voices == 4)
			break;
	}

	for (uint i = 0; i < kVoices; ++i)
		_voiceNote[i] = -1;
}

void MidiDriver_AmigaSci0::onTimer() {
	// This callback is 250Hz and we need 60Hz for doEnvelope()
	_timerCounter += _timerIncrease;

	if (_timerCounter > _timerThreshold) {
		_timerCounter -= _timerThreshold;
	}
}

void MidiDriver_AmigaSci0::voiceOff(int8 voice) {
}

void MidiDriver_AmigaSci0::noteOn(int8 voice, int8 note, int8 velocity) {
}

void MidiDriver_AmigaSci0::noteOff(int8 voice, int8 note) {
}

void MidiDriver_AmigaSci0::send(uint32 b) {
	byte command = b & 0xf0;
	byte channel = b & 0xf;
	byte op1 = (b >> 8) & 0xff;
	byte op2 = (b >> 16) & 0xff;

	int8 voice = _chanVoice[channel];

	if (voice == -1)
		return;

	switch(command) {
	case 0x80:
		noteOff(voice, op1);
		break;
	case 0x90:
		noteOn(voice, op1, op2);
		break;
	case 0xb0:
		// Not in original driver
		if (op1 == 0x7b)
			voiceOff(voice);
		break;
	case 0xc0:
		_voicePatch[voice] = op1;
		break;
	}
}

void MidiDriver_AmigaSci0::setVolume(byte volume) {
}

bool MidiDriver_AmigaSci0::readInstruments() {
	// FIXME: Memory allocated here is never freed

	Common::File file;

	if (!file.open("bank.001"))
		return false;

	memset(&_bank, 0, sizeof(_bank));
	file.read(_bank.name, 8);
	if (strcmp(_bank.name, "X0iUo123") != 0)
		return false;

	file.read(_bank.name, 30);
	_bank.instrumentCount = file.readUint16BE();

	for (uint i = 0; i < _bank.instrumentCount; ++i) {
		Instrument *ins = new Instrument();
		_bank.patchNr[i] = file.readUint16BE();
		_bank.instrument[i] = ins;
		if (i == 0) {
			for (uint j = 0; j < _bank.instrumentCount; ++j)
				_bank.instrument[j] = ins;
		}
		file.read(ins->name, 30);
		ins->flags = file.readUint16BE();
		ins->transpose = file.readByte();
		ins->seg1Size = file.readUint16BE();
		ins->seg2Offset = file.readUint32BE();
		ins->seg2Size = file.readUint16BE();
		ins->seg3Offset = file.readUint32BE();
		ins->seg3Size = file.readUint16BE();
		file.read(ins->envelope, 12);

		uint32 sampleSize = ins->seg1Size + ins->seg2Size + ins->seg3Size;
		sampleSize <<= 1;
		ins->samples = new byte[sampleSize];

		if (!ins->samples)
			return false;

		file.read(ins->samples, sampleSize);
	}

	return true;
}

class MidiPlayer_AmigaSci0 : public MidiPlayer {
public:
	MidiPlayer_AmigaSci0(SciVersion version) : MidiPlayer(version) { _driver = new MidiDriver_AmigaSci0(g_system->getMixer()); }
	byte getPlayId() const { return 0x40; }
	int getPolyphony() const { return MidiDriver_AmigaSci0::kVoices; }
	bool hasRhythmChannel() const { return false; }
	void setVolume(byte volume) { static_cast<MidiDriver_AmigaSci0 *>(_driver)->setVolume(volume); }
	void playSwitch(bool play) { static_cast<MidiDriver_AmigaSci0 *>(_driver)->playSwitch(play); }
	void onNewSound(uint16 channels) { static_cast<MidiDriver_AmigaSci0 *>(_driver)->onNewSound(channels); }
};

MidiPlayer *MidiPlayer_AmigaSci0_create(SciVersion version) {
	return new MidiPlayer_AmigaSci0(version);
}

} // End of namespace Sci
