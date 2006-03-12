/* ScummVM - Scumm Interpreter
 * Copyright (C) 2006 The ScummVM project
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

#include "common/stdafx.h"
#include "common/system.h"
#include "common/mutex.h"
#include "common/timer.h"
#include "kyra/resource.h"
#include "kyra/sound.h"

#include "sound/mixer.h"
#include "sound/fmopl.h"
#include "sound/audiostream.h"

// TODO:
//   - check how the sounds are stopped (doesn't work atm whyever)
//   - implement music pausing + stop and fadeing
//   - check why the sfx sounds strange sometimes

// Basic Adlib Programming:
// http://www.gamedev.net/reference/articles/article446.asp

namespace Kyra {

class AdlibDriver : public AudioStream {
public:
	AdlibDriver(Audio::Mixer *mixer);
	~AdlibDriver();

	int callback(int opcode, ...);
	void callback();

	// AudioStream API
	int readBuffer(int16 *buffer, const int numSamples) {
		memset(buffer, 0, sizeof(int16)*numSamples);
		lock();
		YM3812UpdateOne(_adlib, buffer, numSamples);
		unlock();
		return numSamples;
	}

	bool isStereo() const { return false; }
	bool endOfData() const { return false; }
	int getRate() const { return _mixer->getOutputRate(); }

private:
	struct OpcodeEntry {
		typedef int (AdlibDriver::*DriverOpcode)(va_list &list);
		DriverOpcode function;
		const char *name;
	};

	static const OpcodeEntry _opcodeList[];
	static const int _opcodesEntries;

	int snd_ret0x100(va_list &list);
	int snd_ret0x1983(va_list &list);
	int snd_initDriver(va_list &list);
	int snd_deinitDriver(va_list &list);
	int snd_setSoundData(va_list &list);
	int snd_unkOpcode1(va_list &list);
	int snd_startSong(va_list &list);
	int snd_unkOpcode2(va_list &list);
	int snd_unkOpcode3(va_list &list);
	int snd_readByte(va_list &list);
	int snd_writeByte(va_list &list);
	int snd_setUnk5(va_list &list);
	int snd_unkOpcode4(va_list &list);
	int snd_dummy(va_list &list);
	int snd_getNullvar4(va_list &list);
	int snd_setNullvar3(va_list &list);
	int snd_setFlag(va_list &list);
	int snd_clearFlag(va_list &list);

	// These variables have not yet been named, but some of them are partly
	// known nevertheless:
	//
	// unk1  - Unknown. Used for updating unk4.
	// unk3  - Unknown. Used for turning off some notes.
	// unk4  - Unknown. Related to sound timing?
	// unk5  - Unknown. Used for turning off some notes.
	// unk6  - Unknown. Used to indicate unk1 should be reset to _tempo.
	// unk7  - Unknown. Used for turning off some notes.
	// unk11 - Unknown. Used for updating unk5.
	// unk12 - Unknown. Used for updating unk7.
	// unk16 - Sound-related. Possibly some sort of pitch bend.
	// unk18 - Sound-effect. Used for secondaryEffect1()
	// unk19 - Sound-effect. Used for secondaryEffect1()
	// unk20 - Sound-effect. Used for secondaryEffect1()
	// unk21 - Sound-effect. Used for secondaryEffect1()
	// unk22 - Sound-effect. Used for secondaryEffect1()
	// unk29 - Sound-effect. Used for primaryEffect1()
	// unk30 - Sound-effect. Used for primaryEffect1()
	// unk31 - Sound-effect. Used for primaryEffect1()
	// unk32 - Sound-effect. Used for primaryEffect2()
	// unk33 - Sound-effect. Used for primaryEffect2()
	// unk34 - Sound-effect. Used for primaryEffect2()
	// unk35 - Sound-effect. Used for primaryEffect2()
	// unk36 - Sound-effect. Used for primaryEffect2()
	// unk37 - Sound-effect. Used for primaryEffect2()
	// unk38 - Sound-effect. Used for primaryEffect2()
	// unk39 - Currently unused, except for updateCallback56()
	// unk40 - Currently unused, except for updateCallback56()
	// unk41 - Sound-effect. Used for primaryEffect2()

	struct OutputState {
		uint8 opExtraLevel2;
		uint8 *dataptr;
		uint8 unk5;
		uint8 repeatCounter;
		int8 baseOctave;
		uint8 priority;
		uint8 dataptrStackPos;
		uint8 *dataptrStack[4];
		int8 baseNote;
		uint8 unk29;
		int8 unk31;
		uint16 unk30;
		uint16 unk37;
		uint8 unk33;
		uint8 unk34;
		uint8 unk35;
		uint8 unk36;
		uint8 unk32;
		uint8 unk41;
		uint8 unk38;
		uint8 opExtraLevel1;
		uint8 unk7;
		uint8 baseFreq;
		int8 unk1;
		int8 unk4;
		uint8 regAx;
		uint8 regBx;
		typedef void (AdlibDriver::*Callback)(OutputState&);
		Callback callback1;
		Callback callback2;
		uint8 unk12;
		uint8 opLevel1;
		uint8 opLevel2;
		uint8 opExtraLevel3;
		uint8 twoChan;
		uint8 unk39;	
		uint8 unk40;
		uint8 unk3;
		uint8 unk11;
		uint8 unk19;
		int8 unk18;
		int8 unk20;
		int8 unk21;
		uint8 unk22;
		uint16 offset;
		uint8 unk6;
		uint8 rawNote;
		int8 unk16;
	};

	void primaryEffect1(OutputState &state);
	void primaryEffect2(OutputState &state);
	void secondaryEffect1(OutputState& state);

	void resetAdlibState();
	void writeOPL(byte reg, byte val);
	void initTable(OutputState &table);
	void noteOff(OutputState &table);
	void unkOutput2(uint8 num);

	uint16 getRandomNr();
	void setupDuration(uint8 duration, OutputState &state);

	void setupNote(uint8 rawNote, OutputState &state, bool flag = false);
	void setupInstrument(uint8 regOffset, uint8 *dataptr, OutputState &state);
	void noteOn(OutputState &state);

	void adjustVolume(OutputState &state);

	uint8 calculateOpLevel1(OutputState &state);
	uint8 calculateOpLevel2(OutputState &state);

	uint16 checkValue(int16 val) {
		if (val < 0)
			val = 0;
		if (val > 0x3F)
			val = 0x3F;
		return val;
	}

	void callbackOutput();
	void callbackProcess();

	struct ParserOpcode {
		typedef int (AdlibDriver::*POpcode)(uint8 *&dataptr, OutputState &state, uint8 value);
		POpcode function;
		const char *name;
	};
	static const ParserOpcode _parserOpcodeTable[];
	static const int _parserOpcodeTableSize;

	int update_setRepeat(uint8 *&dataptr, OutputState &state, uint8 value);
	int update_checkRepeat(uint8 *&dataptr, OutputState &state, uint8 value);
	int updateCallback3(uint8 *&dataptr, OutputState &state, uint8 value);
	int updateCallback4(uint8 *&dataptr, OutputState &state, uint8 value);
	int update_jump(uint8 *&dataptr, OutputState &state, uint8 value);
	int update_jumpToSubroutine(uint8 *&dataptr, OutputState &state, uint8 value);
	int update_returnFromSubroutine(uint8 *&dataptr, OutputState &state, uint8 value);
	int update_setBaseOctave(uint8 *&dataptr, OutputState &state, uint8 value);
	int updateCallback9(uint8 *&dataptr, OutputState &state, uint8 value);
	int update_playRest(uint8 *&dataptr, OutputState &state, uint8 value);
	int update_writeAdlib(uint8 *&dataptr, OutputState &state, uint8 value);
	int updateCallback12(uint8 *&dataptr, OutputState &state, uint8 value);
	int update_setBaseNote(uint8 *&dataptr, OutputState &state, uint8 value);
	int update_setupSecondaryEffect1(uint8 *&dataptr, OutputState &state, uint8 value);
	int updateCallback15(uint8 *&dataptr, OutputState &state, uint8 value);
	int updateCallback16(uint8 *&dataptr, OutputState &state, uint8 value);
	int updateCallback17(uint8 *&dataptr, OutputState &state, uint8 value);
	int update_setupPrimaryEffect1(uint8 *&dataptr, OutputState &state, uint8 value);
	int update_removePrimaryEffect1(uint8 *&dataptr, OutputState &state, uint8 value);
	int update_setBaseFreq(uint8 *&dataptr, OutputState &state, uint8 value);
	int update_setupPrimaryEffect2(uint8 *&dataptr, OutputState &state, uint8 value);
	int update_setPriority(uint8 *&dataptr, OutputState &state, uint8 value);
	int updateCallback23(uint8 *&dataptr, OutputState &state, uint8 value);
	int updateCallback24(uint8 *&dataptr, OutputState &state, uint8 value);
	int update_setExtraLevel1(uint8 *&dataptr, OutputState &state, uint8 value);
	int updateCallback26(uint8 *&dataptr, OutputState &state, uint8 value);
	int update_playNote(uint8 *&dataptr, OutputState &state, uint8 value);
	int updateCallback28(uint8 *&dataptr, OutputState &state, uint8 value);
	int update_setTempo(uint8 *&dataptr, OutputState &state, uint8 value);
	int update_removeSecondaryEffect1(uint8 *&dataptr, OutputState &state, uint8 value);
	int updateCallback31(uint8 *&dataptr, OutputState &state, uint8 value);
	int update_setExtraLevel3(uint8 *&dataptr, OutputState &state, uint8 value);
	int update_setExtraLevel2(uint8 *&dataptr, OutputState &state, uint8 value);
	int update_changeExtraLevel2(uint8 *&dataptr, OutputState &state, uint8 value);
	int update_setAMDepth(uint8 *&dataptr, OutputState &state, uint8 value);
	int update_setVibratoDepth(uint8 *&dataptr, OutputState &state, uint8 value);
	int update_changeExtraLevel1(uint8 *&dataptr, OutputState &state, uint8 value);
	int updateCallback38(uint8 *&dataptr, OutputState &state, uint8 value);
	int updateCallback39(uint8 *&dataptr, OutputState &state, uint8 value);
	int update_removePrimaryEffect2(uint8 *&dataptr, OutputState &state, uint8 value);
	int updateCallback41(uint8 *&dataptr, OutputState &state, uint8 value);
	int updateCallback42(uint8 *&dataptr, OutputState &state, uint8 value);
	int updateCallback43(uint8 *&dataptr, OutputState &state, uint8 value);
	int updateCallback44(uint8 *&dataptr, OutputState &state, uint8 value);
	int updateCallback45(uint8 *&dataptr, OutputState &state, uint8 value);
	int updateCallback46(uint8 *&dataptr, OutputState &state, uint8 value);
	int updateCallback47(uint8 *&dataptr, OutputState &state, uint8 value);
	int updateCallback48(uint8 *&dataptr, OutputState &state, uint8 value);
	int updateCallback49(uint8 *&dataptr, OutputState &state, uint8 value);
	int updateCallback50(uint8 *&dataptr, OutputState &state, uint8 value);
	int updateCallback51(uint8 *&dataptr, OutputState &state, uint8 value);
	int updateCallback52(uint8 *&dataptr, OutputState &state, uint8 value);
	int updateCallback53(uint8 *&dataptr, OutputState &state, uint8 value);
	int updateCallback54(uint8 *&dataptr, OutputState &state, uint8 value);
	int updateCallback55(uint8 *&dataptr, OutputState &state, uint8 value);
	int updateCallback56(uint8 *&dataptr, OutputState &state, uint8 value);
private:
	// These variables have not yet been named, but some of them are partly
	// known nevertheless:
	//
	// _unk4           - Unknown, but probably indicates that Adlib's
	//                   rhythm section is active.
	// _unk5           - Currently unused, except for updateCallback54()
	// _unkValue1      - Unknown. Used for updating _unkValue2
	// _unkValue2      - Unknown. Used for updating _unkValue4
	// _unkValue3      - Unknown. Used for updating _unkValue2
	// _unkValue4      - Unknown. Used for updating _unkValue5
	// _unkValue5      - Unknown. Used for controlling updateCallback24().
	// _unkValue6      - Unknown. Something to do with channel 1 volume?
	// _unkValue7      - Unknown. Something to do with channel 2 volume?
	// _unkValue8      - Unknown. Something to do with channel 2 volume?
	// _unkValue9      - Unknown. Something to do with channel 3 volume?
	// _unkValue10     - Unknown. Something to do with channel 3 volume?
	// _unkValue11     - Unknown. Something to do with channel 2 volume?
	// _unkValue12     - Unknown. Something to do with channel 2 volume?
	// _unkValue13     - Unknown. Something to do with channel 3 volume?
	// _unkValue14     - Unknown. Something to do with channel 3 volume?
	// _unkValue15     - Unknown. Something to do with channel 3 volume?
	// _unkValue16     - Unknown. Something to do with channel 3 volume?
	// _unkValue17     - Unknown. Something to do with channel 2 volume?
	// _unkValue18     - Unknown. Something to do with channel 2 volume?
	// _unkValue19     - Unknown. Something to do with channel 1 volume?
	// _unkValue20     - Unknown. Something to do with channel 1 volume?
	// _unkOutputByte2 - Unknown. Something to do with the BD register.
	// _unkTable[]     - Probably frequences for the 12-tone scale.
	// _unkTable2[]    - Unknown. Currently only used by updateCallback46()
	// _unkTable2_1[]  - One of the tables in _unkTable2[]
	// _unkTable2_2[]  - One of the tables in _unkTable2[]
	// _unkTable2_3[]  - One of the tables in _unkTable2[]

	int _lastProcessed;
	int8 _flagTrigger;
	int _curTable;
	uint8 _unk4;
	uint8 _unk5;
	int _soundsPlaying;

	uint16 _rnd;
	uint8 _continueFlag;

	uint8 _unkValue1;
	uint8 _unkValue2;
	int8 _unkValue3;
	uint8 _unkValue4;
	uint8 _unkValue5;
	uint8 _unkValue6;
	uint8 _unkValue7;
	uint8 _unkValue8;
	uint8 _unkValue9;
	uint8 _unkValue10;
	uint8 _unkValue11;
	uint8 _unkValue12;
	uint8 _unkValue13;
	uint8 _unkValue14;
	uint8 _unkValue15;
	uint8 _unkValue16;
	uint8 _unkValue17;
	uint8 _unkValue18;
	uint8 _unkValue19;
	uint8 _unkValue20;

	int _flags;
	FM_OPL *_adlib;

	uint8 *_soundData;

	uint8 _soundIdTable[0x10];
	OutputState _outputTables[10];

	uint8 _unkOutputByte2;
	uint8 _curRegOffset;
	int8 _tempo;

	const uint8 *_tablePtr1;
	const uint8 *_tablePtr2;

	static const uint8 _outputTable[];
	static const uint16 _unkTable[];
	static const uint8 *_unkTable2[];
	static const uint8 _unkTable2_1[];
	static const uint8 _unkTable2_2[];
	static const uint8 _unkTable2_3[];
	static const uint8 _unkTables[][32];

	Common::Mutex _mutex;
	Audio::Mixer *_mixer;

	void lock() { _mutex.lock(); }
	void unlock() { _mutex.unlock(); }
};

void AdlibTimerCall(void *refCon) {
	AdlibDriver *driver = (AdlibDriver*)refCon;
	driver->callback();
}

AdlibDriver::AdlibDriver(Audio::Mixer *mixer) {
	_mixer = mixer;

	_flags = 0;
	_adlib = makeAdlibOPL(getRate());
	assert(_adlib);

	memset(_outputTables, 0, sizeof(_outputTables));
	_soundData = 0;

	_unkOutputByte2 = _curRegOffset = 0;

	_lastProcessed = _flagTrigger = _curTable = _unk4 = 0;
	_rnd = 0x1234;
	_continueFlag = 0;

	_tempo = 0;

	_unkValue3 = -1;
	_unkValue1 = _unkValue2 = _unkValue4 = _unkValue5 = 0;
	_unkValue6 = _unkValue7 = _unkValue8 = _unkValue9 = _unkValue10 = 0;
	_unkValue11 = _unkValue12 = _unkValue13 = _unkValue14 = _unkValue15 =
	_unkValue16 = _unkValue17 = _unkValue18 = _unkValue19 = _unkValue20 = 0;

	_tablePtr1 = _tablePtr2 = 0;

	_mixer->setupPremix(this);

	Common::g_timer->installTimerProc(&AdlibTimerCall, 13888, this);
}

AdlibDriver::~AdlibDriver() {
	Common::g_timer->removeTimerProc(&AdlibTimerCall);
	_mixer->setupPremix(0);
	OPLDestroy(_adlib);
	_adlib = 0;
}

int AdlibDriver::callback(int opcode, ...) {
	lock();
	if (opcode >= _opcodesEntries || opcode < 0) {
		warning("AdlibDriver: calling unknown opcode '%d'", opcode);
		return 0;
	}

	debugC(9, kDebugLevelSound, "Calling opcode '%s' (%d)", _opcodeList[opcode].name, opcode);

	va_list args;
	va_start(args, opcode);
	int returnValue = (this->*(_opcodeList[opcode].function))(args);
	va_end(args);
	unlock();
	return returnValue;
}

// Opcodes

int AdlibDriver::snd_ret0x100(va_list &list) {
	return 0x100;
}

int AdlibDriver::snd_ret0x1983(va_list &list) {
	return 0x1983;
}

int AdlibDriver::snd_initDriver(va_list &list) {
	_lastProcessed = _soundsPlaying = 0;
	resetAdlibState();
	return 0;
}

int AdlibDriver::snd_deinitDriver(va_list &list) {
	resetAdlibState();
	return 0;
}

int AdlibDriver::snd_setSoundData(va_list &list) {
	if (_soundData) {
		delete [] _soundData;
		_soundData = 0;
	}
	_soundData = va_arg(list, uint8*);
	return 0;
}

int AdlibDriver::snd_unkOpcode1(va_list &list) {
	warning("unimplemented snd_unkOpcode1");
	return 0;
}

int AdlibDriver::snd_startSong(va_list &list) {
	int songId = va_arg(list, int);
	_flags |= 8;
	_flagTrigger = 1;
	uint16 offset = READ_LE_UINT16(&_soundData[songId << 1]);
	uint8 firstByte = *(_soundData + offset);

	if ((songId << 1) != 0) {
		if (firstByte == 9) {
			if (_flags & 2)
				return 0;
		} else {
			if (_flags & 1)
				return 0;
		}
	}

	_soundIdTable[_soundsPlaying] = songId;
	++_soundsPlaying;
	_soundsPlaying &= 0x0F;

	return 0;
}

int AdlibDriver::snd_unkOpcode2(va_list &list) {
	warning("unimplemented snd_unkOpcode2");
	return 0;
}

int AdlibDriver::snd_unkOpcode3(va_list &list) {
	int value = va_arg(list, int);
	int loop = value;
	if (value < 0) {
		value = 0;
		loop = 9;
	}
	loop -= value;
	++loop;

	while (loop--) {
		_curTable = value;
		OutputState &table = _outputTables[_curTable];
		table.priority = 0;
		table.dataptr = 0;
		if (value != 9) {
			noteOff(table);
		}
		++value;
	}

	return 0;
}

int AdlibDriver::snd_readByte(va_list &list) {
	int a = va_arg(list, int);
	int b = va_arg(list, int);
	uint8 *ptr = _soundData + READ_LE_UINT16(&_soundData[a << 1]) + b;
	return *ptr;
}

int AdlibDriver::snd_writeByte(va_list &list) {
	int a = va_arg(list, int);
	int b = va_arg(list, int);
	int c = va_arg(list, int);
	uint8 *ptr = _soundData + READ_LE_UINT16(&_soundData[a << 1]) + b;
	uint8 oldValue = *ptr;
	*ptr = (uint8)c;
	return oldValue;
}

int AdlibDriver::snd_setUnk5(va_list &list) {
	warning("unimplemented snd_setUnk5");
	return 0;
}

int AdlibDriver::snd_unkOpcode4(va_list &list) {
	warning("unimplemented snd_unkOpcode4");
	return 0;
}

int AdlibDriver::snd_dummy(va_list &list) {
	return 0;
}

int AdlibDriver::snd_getNullvar4(va_list &list) {
	warning("unimplemented snd_getNullvar4");
	return 0;
}

int AdlibDriver::snd_setNullvar3(va_list &list) {
	warning("unimplemented snd_setNullvar3");
	return 0;
}

int AdlibDriver::snd_setFlag(va_list &list) {
	int oldFlags = _flags;
	_flags |= va_arg(list, int);
	return oldFlags;
}

int AdlibDriver::snd_clearFlag(va_list &list) {
	int oldFlags = _flags;
	_flags &= ~(va_arg(list, int));
	return oldFlags;
}

// timer callback

void AdlibDriver::callback() {
	lock();
	--_flagTrigger;
	if (_flagTrigger < 0)
		_flags &= ~8;
	callbackOutput();
	callbackProcess();

	_unkValue3 += _tempo;
	if (_unkValue3 < 0) {
		if (!(--_unkValue2)) {
			_unkValue2 = _unkValue1;
			++_unkValue4;
		}
	}
	unlock();
}

void AdlibDriver::callbackOutput() {
	while (_lastProcessed != _soundsPlaying) {
		uint8 *ptr = _soundData;

		ptr += READ_LE_UINT16(&ptr[_soundIdTable[_lastProcessed] << 1]);
		uint8 index = *ptr++;
		OutputState &table = _outputTables[index];

		uint8 priority = *ptr++;

		// Only start this sound if its priority is higher than the one
		// already playing.

		if (priority >= table.priority) {
			initTable(table);
			table.priority = priority;
			table.dataptr = ptr;
			table.unk1 = -1;
			table.unk4 = -1;
			table.unk5 = 1;
			if (index != 9) {
				unkOutput2(index);
			}
		}

		++_lastProcessed;
		_lastProcessed &= 0x0F;
	}
}

// A few words on opcode parsing and timing:
//
// We have a timer callback that is called every 13888 us.
//
// Each channel appears to have its own individual tempo, unk1, which is added
// to unk4. If this causes unk4 to wrap around to negative, something happens.
//
// When "something happens", unk5 is decreased. If unk5 is still non-zero,
// nothing much happens. Notes may be turned off depending on unk3 and unk7,
// but other than that no new commands are issued.
//
// If unk5 reaches zero, a new set of music opcodes are executed. In effect,
// unk5 is the duration of the channel's most recent opcode. Either a note or
// a pause, presumably.
//
// An opcode is one byte, followed by a variable number of parameters. Since
// most opcodes have at least one one-byte parameter, we read that as well. Any
// opcode that doesn't have that one parameter is responsible for backing the
// data pointer.
//
// If the most significant bit of the opcode is 1, it's a function; call it. If
// it returns something greater than zero, it's the last opcode in the current
// set of opcodes. An opcode can also make itself the last one by setting the
// data pointer to NULL.
//
// If the most significant bit of the opcode is 0, it's a note, and the first
// parameter is its duration. (There are cases where the duration is modified
// but that's an exception.) This duration is also assigned to _continueFlag,
// which affects the return value from several of the opcode functions. If the
// duration is non-zero, it's the last opcode in the current set of opcodes.
//
// Finally, most of the times that the callback is called, it will invoke the
// effects callbacks. The final opcode in a set can prevent this, if it's a
// function and it returns anything other than 1.
//
// To summarize:
//
// unk1 is the channel tempo, assisted by unk4
// unk5 is the duration of the last opcode (probably a note) of a set
// unk3 and unk7 can turn off notes prematurely; to implement staccatto?
//
// Some possible sources of tempo bugs:
//
// The timer has a 10 ms resolution. Over time it will be pretty accurate, but
// individual calls may be a bit off. The only notes that are this short are
// the ones played by the effects callbacks, so it shouldn't matter much.
//
// Adding unk1 to a negative unk4 does not necessarily take it out of the
// negative range, so it may trigger twice in a row. Again, this shouldn't
// matter much.
//
// The unk5 variable (duration) may be incorrectly set.
//
// An opcode function may be returning the wrong value.

void AdlibDriver::callbackProcess() {
	for (_curTable = 9; _curTable >= 0; --_curTable) {
		if (!_outputTables[_curTable].dataptr) {
			continue;
		}
	
		OutputState &table = _outputTables[_curTable];
		_curRegOffset = _outputTable[_curTable];

		if (table.unk6) {
			table.unk1 = _tempo;
		}

		table.unk4 += table.unk1;
		if (table.unk4 < 0) {
			if (--table.unk5) {
				if (table.unk5 == table.unk7)
					noteOff(table);
				if (table.unk5 == table.unk3 && _curTable != 9)
					noteOff(table);
			} else {
				int8 opcode = 0;
				while (table.dataptr) {
					uint16 command = READ_LE_UINT16(table.dataptr);
					table.dataptr += 2;
					if (command & 0x0080) {
						opcode = command & 0x7F;
						if (opcode > 0x4A)
							opcode = 0x4A;
						debugC(9, kDebugLevelSound, "Calling opcode '%s' (%d) (channel: %d)", _parserOpcodeTable[opcode].name, opcode, _curTable);
						opcode = (this->*(_parserOpcodeTable[opcode].function))(table.dataptr, table, (command & 0xFF00) >> 8);
						--opcode;
						if (opcode >= 0)
							break;
						continue;
					} else {
						debugC(9, kDebugLevelSound, "Note on opcode 0x%02X (duration: %d) (channel: %d)", command & 0xFF, (command >> 8) & 0xFF, _curTable);
						opcode = 0;
						setupNote(command & 0xFF, table);
						noteOn(table);
						setupDuration((command & 0xFF00) >> 8, table);
						if (!_continueFlag)
							continue;
						break;
					}
				}
				if (opcode)
					continue;
			}
		}

		if (table.callback1)
			(this->*(table.callback1))(table);
		if (table.callback2)
			(this->*(table.callback2))(table);
	}
}

// 

void AdlibDriver::resetAdlibState() {
	debugC(9, kDebugLevelSound, "resetAdlibState()");
	_rnd = 0x1234;

	// Authorize the control of the waveforms
	writeOPL(0x01, 0x20);

	// Select FM music mode
	writeOPL(0x08, 0x00);

	// I would guess the main purpose of this is to turn off the rhythm,
	// thus allowing us to use 9 melodic voices instead of 6.
	writeOPL(0xBD, 0x00);

	int loop = 10;
	while (loop--) {
		if (loop != 9) {
			// Silence the channel
			writeOPL(0x40 + _outputTable[loop], 0x3F);
			writeOPL(0x43 + _outputTable[loop], 0x3F);
		}
		initTable(_outputTables[loop]);
	}
}

// Old calling style: output0x388(0xABCD)
// New calling style: writeOPL(0xAB, 0xCD)

void AdlibDriver::writeOPL(byte reg, byte val) {
	OPLWriteReg(_adlib, reg, val);
}

void AdlibDriver::initTable(OutputState &table) {
	debugC(9, kDebugLevelSound, "initTable(%d)", &table - _outputTables);
	memset(&table.dataptr, 0, sizeof(OutputState) - ((char*)&table.dataptr - (char*)&table));

	table.unk1 = -1;
	table.priority = 0;
	// normally here are nullfuncs but we set 0 for now
	table.callback1 = 0;
	table.callback2 = 0;
	table.unk3 = 0x01;
}

void AdlibDriver::noteOff(OutputState &table) {
	debugC(9, kDebugLevelSound, "noteOff(%d)", &table - _outputTables);

	// I believe that 9 is the percussion channel.
	if (_curTable == 9)
		return;

	// I believe this has to do with channels 6, 7, and 8 being special
	// when Adlib's rhythm section is enabled.
	if (_unk4 && _curTable >= 6)
		return;

	// This means the "Key On" bit will always be 0
	table.regBx &= 0xDF;

	// Octave / F-Number / Key-On
	writeOPL(0xB0 + _curTable, table.regBx);
}

void AdlibDriver::unkOutput2(uint8 num) {
	debugC(9, kDebugLevelSound, "unkOutput2(%d)", num);

	// I believe this has to do with channels 6, 7, and 8 being special
	// when Adlib's rhythm section is enabled.
	if (_unk4 && num >= 6)
		return;

	uint8 value = _outputTable[num];

	// Clear the Attack Rate / Decay Rate for the channel
	writeOPL(0x60 + value, 0xFF);
	writeOPL(0x63 + value, 0xFF);

	// Clear the Sustain Level / Release Rate for the channel
	writeOPL(0x80 + value, 0xFF);
	writeOPL(0x83 + value, 0xFF);

	// Octave / F-Number / Key-On

	// Turn the note off, then turn it on again. This could be a "note on"
	// function, but it also clears the octave and the part of the
	// frequency (F-Number) stored in this register. Weird.

	writeOPL(0xB0 + num, 0x00);
	writeOPL(0xB0 + num, 0x20);
}

// I believe this is a random number generator. It actually does seem to
// generate an even distribution of almost all numbers from 0 through 65535,
// though in my tests some numbers were never generated.

uint16 AdlibDriver::getRandomNr() {
	_rnd += 0x9248;
	uint16 lowBits = _rnd & 7;
	_rnd >>= 3;
	_rnd |= (lowBits << 13);
	return _rnd;
}

void AdlibDriver::setupDuration(uint8 duration, OutputState &state) {
	debugC(9, kDebugLevelSound, "setupDuration(%d, %d)", duration, &state - _outputTables);
	_continueFlag = duration;
	if (state.unk11) {
		state.unk5 = duration + (getRandomNr() & state.unk11 & 0xFF);
		return;
	}
	if (state.unk12) {
		state.unk7 = (duration >> 3) * state.unk12;
	}
	state.unk5 = duration;
}

// This function may or may not play the note. It's usually followed by a call
// to noteOn(), which will always play the current note.

void AdlibDriver::setupNote(uint8 rawNote, OutputState &state, bool flag) {
	debugC(9, kDebugLevelSound, "setupNote(%d, %d)", rawNote, &state - _outputTables);

	state.rawNote = rawNote;

	int8 note = (rawNote & 0x0F) + state.baseNote;
	int8 octave = ((rawNote + state.baseOctave) >> 4) & 0x0F;

	// There are only twelve notes. If we go outside that, we have to
	// adjust the note and octave.

	if (note >= 12) {
		note -= 12;
		octave++;
	} else if (note < 0) {
		note += 12;
		octave--;
	}

	// The calculation of frequency looks quite different from the original
	// disassembly at a first glance, but when you consider that the
	// largest possible value would be 0x0246 + 0xFF + 0x47 (and that's if
	// baseFreq is unsigned), freq is still a 10-bit value, just as it
	// should be to fit in the Ax and Bx registers.
	//
	// If it were larger than that, it could have overflowed into the
	// octave bits, and that could possibly have been used in some sound.
	// But as it is now, I can't see any way it would happen.

	uint16 freq = _unkTable[note] + state.baseFreq;

	// When called from callback 41, the behaviour is slightly different:
	// We adjust the frequency, even when state.unk16 is 0.

	if (state.unk16 || flag) {
		const uint8 *table;

		if (state.unk16 >= 0) {
			table = _unkTables[(state.rawNote & 0x0F) + 2];
			freq += table[state.unk16];
		} else {
			table = _unkTables[state.rawNote & 0x0F];
			freq -= table[-state.unk16];
		}
	}

	state.regAx = freq & 0xFF;
	state.regBx = (state.regBx & 0x20) | (octave << 2) | ((freq >> 8) & 0x03);

	// Keep the note on or off
	writeOPL(0xA0 + _curTable, state.regAx);
	writeOPL(0xB0 + _curTable, state.regBx);
}

void AdlibDriver::setupInstrument(uint8 regOffset, uint8 *dataptr, OutputState &state) {
	debugC(9, kDebugLevelSound, "setupInstrument(%d, %p, %d)", regOffset, (const void *)dataptr, &state - _outputTables);
	// Amplitude Modulation / Vibrato / Envelope Generator Type /
	// Keyboard Scaling Rate / Modulator Frequency Multiple
	writeOPL(0x20 + regOffset, *dataptr++);
	writeOPL(0x23 + regOffset, *dataptr++);

	uint8 temp = *dataptr++;

	// Feedback / Algorithm

	// It is very likely that _curTable really does refer to the same
	// channel as regOffset, but there's only one Cx register per channel.

	writeOPL(0xC0 + _curTable, temp);

	// The algorithm bit. I don't pretend to understand this fully, but
	// "If set to 0, operator 1 modulates operator 2. In this case,
	// operator 2 is the only one producing sound. If set to 1, both
	// operators produce sound directly. Complex sounds are more easily
	// created if the algorithm is set to 0."

	state.twoChan = temp & 1;

	// Waveform Select
	writeOPL(0xE0 + regOffset, *dataptr++);
	writeOPL(0xE3 + regOffset, *dataptr++);

	state.opLevel1 = *dataptr++;
	state.opLevel2 = *dataptr++;

	// Level Key Scaling / Total Level
	writeOPL(0x40 + regOffset, calculateOpLevel1(state));
	writeOPL(0x43 + regOffset, calculateOpLevel2(state));

	// Attack Rate / Decay Rate
	writeOPL(0x60 + regOffset, *dataptr++);
	writeOPL(0x63 + regOffset, *dataptr++);

	// Sustain Level / Release Rate
	writeOPL(0x80 + regOffset, *dataptr++);
	writeOPL(0x83 + regOffset, *dataptr++);
}

// Apart from playing the note, this function also updates the variables for
// primary effect 2.

void AdlibDriver::noteOn(OutputState &state) {
	debugC(9, kDebugLevelSound, "noteOn(%d)", &state - _outputTables);

	// The "note on" bit is set, and the current note is played.

	state.regBx |= 0x20;
	writeOPL(0xB0 + _curTable, state.regBx);

	int8 shift = 9 - state.unk33;
	uint16 temp = state.regAx | (state.regBx << 8);
	state.unk37 = ((temp & 0x3FF) >> shift) & 0xFF;
	state.unk38 = state.unk36;
}

void AdlibDriver::adjustVolume(OutputState &state) {
	debugC(9, kDebugLevelSound, "adjustVolume(%d)", &state - _outputTables);
	// Level Key Scaling / Total Level

	writeOPL(0x43 + _outputTable[_curTable], calculateOpLevel2(state));
	if (state.twoChan)
		writeOPL(0x40 + _outputTable[_curTable], calculateOpLevel1(state));
}

// This is presumably only used for some sound effects, e.g. Malcolm blowing up
// the trees in the intro (but not the effect where he "booby-traps" the big
// tree) and turning Kallak to stone. Related functions and variables:
//
// update_setupPrimaryEffect1()
//    - Initialises unk29, unk30 and unk31
//    - unk29 is not further modified
//    - unk30 is not further modified, except by update_removePrimaryEffect1()
//
// update_removePrimaryEffect1()
//    - Deinitialises unk30
//
// unk29 - determines how often the notes are played
// unk30 - modifies the frequency
// unk31 - determines how often the notes are played

void AdlibDriver::primaryEffect1(OutputState &state) {
	debugC(9, kDebugLevelSound, "Calling primaryEffect1 (channel: %d)", _curTable);
	state.unk31 += state.unk29;
	if (state.unk31 >= 0)
		return;

	// Initialise unk1 to the current frequency
	uint16 unk1 = ((state.regBx & 3) << 8) | state.regAx;

	// This is presumably to shift the "note on" bit so far to the left
	// that it won't be affected by any of the calculations below.
	uint16 unk2 = ((state.regBx & 0x20) << 8) | (state.regBx & 0x1C);

	int16 unk3 = (int16)state.unk30;

	if (unk3 >= 0) {
		unk1 += unk3;
		if (unk1 >= 734) {
			// The new frequency is too high. Shift it down and go
			// up one octave.
			unk1 >>= 1;
			if (!(unk1 & 0x3FF))
				++unk1;
			unk2 += 4;
			unk2 &= 0xFF1C;
		}
	} else {
		unk1 += unk3;
		if (unk1 < 388) {
			// The new frequency is too low. Shift it up and go
			// down one octave.
			unk1 <<= 1;
			if (!(unk1 & 0x3FF))
				--unk1;
			unk2 -= 4;
			unk2 &= 0xFF1C;
		}
	}

	// Make sure that the new frequency is still a 10-bit value.
	unk1 &= 0x3FF;

	writeOPL(0xA0 + _curTable, unk1 & 0xFF);
	state.regAx = unk1 & 0xFF;

	// Shift down the "note on" bit again.
	uint8 value = unk1 >> 8;
	value |= (unk2 >> 8) & 0xFF;
	value |= unk2 & 0xFF;

	writeOPL(0xB0 + _curTable, value);
	state.regBx = value;
}

// This is presumably only used for some sound effects, e.g. Malcolm entering
// and leaving Kallak's hut. Related functions and variables:
//
// update_setupPrimaryEffect2()
//    - Initialises unk32, unk33, unk34, unk35 and unk36
//    - unk32 is not further modified
//    - unk33 is not further modified
//    - unk34 is a countdown that gets reinitialised to unk35 on zero
//    - unk35 is based on unk34 and not further modified
//    - unk36 is not further modified
//
// noteOn()
//    - Plays the current note
//    - Updates unk37 with a new (lower?) frequency
//    - Copies unk36 to unk38. The unk38 variable is a countdown.
//
// unk32 - determines how often the notes are played
// unk33 - modifies the frequency
// unk34 - countdown, updates frequency on zero
// unk35 - initialiser for unk34 countdown
// unk36 - initialiser for unk38 countdown
// unk37 - frequency
// unk38 - countdown, begins playing on zero
// unk41 - determines how often the notes are played
//
// Note that unk41 is never initialised. Not that it should matter much, but it
// is a bit sloppy.

void AdlibDriver::primaryEffect2(OutputState &state) {
	debugC(9, kDebugLevelSound, "Calling primaryEffect2 (channel: %d)", _curTable);
	if (state.unk38) {
		--state.unk38;
		return;
	}

	state.unk41 += state.unk32;
	if ((int8)state.unk41 < 0) {
		uint16 temp2 = state.unk37;
		if (!(--state.unk34)) {
			temp2 ^= 0xFFFF;
			++temp2;
			state.unk37 = temp2;
			state.unk34 = state.unk35;
		}

		uint16 temp3 = state.regAx | (state.regBx << 8);
		temp2 += temp3 & 0x3FF;
		state.regAx = temp2 & 0xFF;

		state.regBx = (state.regBx & 0xFC) | (temp3 >> 8);

		// Octave / F-Number / Key-On
		writeOPL(0xA0 + _curTable, state.regBx);
		writeOPL(0xB0 + _curTable, state.regBx);
	}
}

// I don't know where this is used. The same operation is performed several
// times on the current channel, using a chunk of the _soundData[] buffer for
// parameters. The parameters are used starting at the end of the chunk.
//
// Since we use _curRegOffset to specify the final register, it's quite
// unlikely that this function is ever used to play notes. It's probably only
// used to modify the sound. Another thing that supports this idea is that it
// can be combined with any of the effects callbacks above.
//
// Related functions and variables:
//
// update_setupSecondaryEffect1()
//    - Initialies unk18, unk19, unk20, unk21, unk22 and offset
//    - unk19 is not further modified
//    - unk20 is not further modified
//    - unk22 is not further modified
//    - offset is not further modified
//
// unk18 -  determines how often the operation is performed
// unk19 -  determines how often the operation is performed
// unk20 -  the start index into the data chunk
// unk21 -  the current index into the data chunk
// unk22 -  the operation to perform
// offset - the offset to the data chunk

void AdlibDriver::secondaryEffect1(OutputState &state) {
	debugC(9, kDebugLevelSound, "Calling secondaryEffect1 (channel: %d)", _curTable);
	state.unk18 += state.unk19;
	if (state.unk18 < 0) {
		if (--state.unk21 < 0) {
			state.unk21 = state.unk20;
		}
		writeOPL(state.unk22 + _curRegOffset, _soundData[state.offset + state.unk21]);
	}
}

uint8 AdlibDriver::calculateOpLevel1(OutputState &state) {
	int8 value = state.opLevel1 & 0x3F;

	if (state.twoChan) {
		value += state.opExtraLevel1;
		value += state.opExtraLevel2;
		value += state.opExtraLevel3;
	}

	// Don't allow the total level to overflow into the scaling level bits.

	if (value > 0x3F) {
		value = 0x3F;
	} else if (value < 0)
		value = 0;

	// Preserve the scaling level bits from opLevel1

	return value | (state.opLevel1 & 0xC0);
}

uint8 AdlibDriver::calculateOpLevel2(OutputState &state) {
	int8 value = state.opLevel2 & 0x3F;

	value += state.opExtraLevel1;
	value += state.opExtraLevel2;
	value += state.opExtraLevel3;

	// Don't allow the total level to overflow into the scaling level bits.

	if (value > 0x3F) {
		value = 0x3F;
	} else if (value < 0)
		value = 0;

	// Preserve the scaling level bits from opLevel2

	return value | (state.opLevel2 & 0xC0);
}

// parser opcodes

int AdlibDriver::update_setRepeat(uint8 *&dataptr, OutputState &state, uint8 value) {
	state.repeatCounter = value;
	return 0;
}

int AdlibDriver::update_checkRepeat(uint8 *&dataptr, OutputState &state, uint8 value) {
	++dataptr;
	if (--state.repeatCounter) {
		int16 add = READ_LE_UINT16(dataptr - 2);
		dataptr += add;
	}
	return 0;
}

// This is similar to callbackOutput()

int AdlibDriver::updateCallback3(uint8 *&dataptr, OutputState &state, uint8 value) {
	if (value >= 0xFF)
		return 0;

	uint16 add = value << 1;
	uint8 *ptr = _soundData + READ_LE_UINT16(_soundData + add);
	uint8 table = *ptr++;
	OutputState &state2 = _outputTables[table];
	uint8 priority = *ptr++;
	if (priority >= state2.priority) {
		_flagTrigger = 1;
		_flags |= 8;
		initTable(state2);
		state2.priority = priority;
		state2.dataptr = ptr;
		state2.unk1 = -1;
		state2.unk4 = -1;
		state2.unk5 = 1;
		unkOutput2(table);
	}
	return 0;
}

int AdlibDriver::updateCallback4(uint8 *&dataptr, OutputState &state, uint8 value) {
	state.unk3 = value;
	return 0;
}

int AdlibDriver::update_jump(uint8 *&dataptr, OutputState &state, uint8 value) {
	--dataptr;
	int16 add = READ_LE_UINT16(dataptr); dataptr += 2;
	dataptr += add;
	return 0;
}

int AdlibDriver::update_jumpToSubroutine(uint8 *&dataptr, OutputState &state, uint8 value) {
	--dataptr;
	int16 add = READ_LE_UINT16(dataptr); dataptr += 2;
	state.dataptrStack[state.dataptrStackPos++] = dataptr;
	dataptr += add;
	return 0;
}

int AdlibDriver::update_returnFromSubroutine(uint8 *&dataptr, OutputState &state, uint8 value) {
	dataptr = state.dataptrStack[--state.dataptrStackPos];
	return 0;
}

int AdlibDriver::update_setBaseOctave(uint8 *&dataptr, OutputState &state, uint8 value) {
	state.baseOctave = value;
	return 0;
}

int AdlibDriver::updateCallback9(uint8 *&dataptr, OutputState &state, uint8 value) {
	state.priority = 0;
	if (_curTable != 9) {
		noteOff(state);
	}
	dataptr = 0;
	return 2;
}

int AdlibDriver::update_playRest(uint8 *&dataptr, OutputState &state, uint8 value) {
	setupDuration(value, state);
	noteOff(state);
	return (_continueFlag != 0);
}

int AdlibDriver::update_writeAdlib(uint8 *&dataptr, OutputState &state, uint8 value) {
	writeOPL(value, *dataptr++);
	return 0;
}

int AdlibDriver::updateCallback12(uint8 *&dataptr, OutputState &state, uint8 value) {
	setupNote(value, state);
	value = *dataptr++;
	setupDuration(value, state);
	return (_continueFlag != 0);
}

int AdlibDriver::update_setBaseNote(uint8 *&dataptr, OutputState &state, uint8 value) {
	state.baseNote = value;
	return 0;
}

int AdlibDriver::update_setupSecondaryEffect1(uint8 *&dataptr, OutputState &state, uint8 value) {
	state.unk18 = value;
	state.unk19 = value;
	state.unk20 = state.unk21 = *dataptr++;
	state.unk22 = *dataptr++;
	state.offset = READ_LE_UINT16(dataptr); dataptr += 2;
	state.callback2 = &AdlibDriver::secondaryEffect1;
	return 0;
}

int AdlibDriver::updateCallback15(uint8 *&dataptr, OutputState &state, uint8 value) {
	OutputState &state2 = _outputTables[value];
	state2.unk5 = 0;
	state2.priority = 0;
	state2.dataptr = 0;
	return 0;
}

int AdlibDriver::updateCallback16(uint8 *&dataptr, OutputState &state, uint8 value) {
	uint8 *ptr = _soundData;
	ptr += READ_LE_UINT16(&_soundData[value << 1]);
	OutputState &state2 = _outputTables[*ptr];
	if (!state2.dataptr) {
		return 0;
	}
	dataptr -= 2;
	return 2;
}

int AdlibDriver::updateCallback17(uint8 *&dataptr, OutputState &state, uint8 value) {
	uint8 *ptr = _soundData;
	ptr += READ_LE_UINT16(_soundData + (value << 1) + 0x1F4);
	setupInstrument(_curRegOffset, ptr, state);
	return 0;
}

int AdlibDriver::update_setupPrimaryEffect1(uint8 *&dataptr, OutputState &state, uint8 value) {
	state.unk29 = value;
	state.unk30 = READ_BE_UINT16(dataptr);
	dataptr += 2;
	state.callback1 = &AdlibDriver::primaryEffect1;
	state.unk31 = -1;
	return 0;
}

int AdlibDriver::update_removePrimaryEffect1(uint8 *&dataptr, OutputState &state, uint8 value) {
	--dataptr;
	state.callback1 = 0;
	state.unk30 = 0;
	return 0;
}

int AdlibDriver::update_setBaseFreq(uint8 *&dataptr, OutputState &state, uint8 value) {
	state.baseFreq = value;
	return 0;
}

int AdlibDriver::update_setupPrimaryEffect2(uint8 *&dataptr, OutputState &state, uint8 value) {
	state.unk32 = value;
	state.unk33 = *dataptr++;
	uint8 temp = *dataptr++;
	state.unk34 = temp + 1;
	state.unk35 = temp << 1;
	state.unk36 = *dataptr++;
	state.callback1 = &AdlibDriver::primaryEffect2;
	return 0;
}

int AdlibDriver::update_setPriority(uint8 *&dataptr, OutputState &state, uint8 value) {
	state.priority = value;
	return 0;
}

int AdlibDriver::updateCallback23(uint8 *&dataptr, OutputState &state, uint8 value) {
	value >>= 1;
	_unkValue1 = _unkValue2 = value;
	_unkValue3 = -1;
	_unkValue4 = _unkValue5 = 0;
	return 0;
}

int AdlibDriver::updateCallback24(uint8 *&dataptr, OutputState &state, uint8 value) {
	if (_unkValue5) {
		if (_unkValue4 & value) {
			_unkValue5 = 0;
			return 0;
		}
	}

	if (!(value & _unkValue4)) {
		++_unkValue5;
	}

	dataptr -= 2;
	state.unk5 = 1;
	return 2;
}

int AdlibDriver::update_setExtraLevel1(uint8 *&dataptr, OutputState &state, uint8 value) {
	state.opExtraLevel1 = value;
	adjustVolume(state);
	return 0;
}

int AdlibDriver::updateCallback26(uint8 *&dataptr, OutputState &state, uint8 value) {
	setupDuration(value, state);
	return (_continueFlag != 0);
}

int AdlibDriver::update_playNote(uint8 *&dataptr, OutputState &state, uint8 value) {
	setupDuration(value, state);
	noteOn(state);
	return (_continueFlag != 0);
}

int AdlibDriver::updateCallback28(uint8 *&dataptr, OutputState &state, uint8 value) {
	state.unk12 = value & 7;
	return 0;
}

int AdlibDriver::update_setTempo(uint8 *&dataptr, OutputState &state, uint8 value) {
	_tempo = (int8)value;
	return 0;
}

int AdlibDriver::update_removeSecondaryEffect1(uint8 *&dataptr, OutputState &state, uint8 value) {
	--dataptr;
	state.callback2 = 0;
	return 0;
}

int AdlibDriver::updateCallback31(uint8 *&dataptr, OutputState &state, uint8 value) {
	state.unk1 = (int8)value;
	return 0;
}

int AdlibDriver::update_setExtraLevel3(uint8 *&dataptr, OutputState &state, uint8 value) {
	state.opExtraLevel3 = value;
	return 0;
}

int AdlibDriver::update_setExtraLevel2(uint8 *&dataptr, OutputState &state, uint8 value) {
	int tableBackup = _curTable;

	_curTable = value;
	OutputState &state2 = _outputTables[value];
	state2.opExtraLevel2 = *dataptr++;
	adjustVolume(state2);

	_curTable = tableBackup;
	return 0;
}

int AdlibDriver::update_changeExtraLevel2(uint8 *&dataptr, OutputState &state, uint8 value) {
	int tableBackup = _curTable;

	_curTable = value;
	OutputState &state2 = _outputTables[value];
	state2.opExtraLevel2 += *dataptr++;
	adjustVolume(state2);

	_curTable = tableBackup;
	return 0;
}

int AdlibDriver::update_setAMDepth(uint8 *&dataptr, OutputState &state, uint8 value) {
	if (value & 1)
		_unkOutputByte2 |= 0x80;
	else
		_unkOutputByte2 &= 0x7F;

	// The AM Depth bit is set or cleared, the others remain unchanged
	writeOPL(0xBD, _unkOutputByte2);
	return 0;
}

int AdlibDriver::update_setVibratoDepth(uint8 *&dataptr, OutputState &state, uint8 value) {
	if (value & 1)
		_unkOutputByte2 |= 0x40;
	else
		_unkOutputByte2 &= 0xBF;

	// The Vibrato Depth bit is set or cleared, the others remain unchanged
	writeOPL(0xBD, _unkOutputByte2);
	return 0;
}

int AdlibDriver::update_changeExtraLevel1(uint8 *&dataptr, OutputState &state, uint8 value) {
	state.opExtraLevel1 += value;
	adjustVolume(state);
	return 0;
}

int AdlibDriver::updateCallback38(uint8 *&dataptr, OutputState &state, uint8 value) {
	int tableBackUp = _curTable;

	_curTable = value;
	OutputState &state2 = _outputTables[value];
	state2.unk5 = state2.priority = 0;
	state2.dataptr = 0;
	state2.opExtraLevel2 = 0;

	if (value != 9) {
		uint8 outValue = _outputTable[value];

		// Feedback strength / Connection type
		writeOPL(0xC0 + _curTable, 0x00);

		// Key scaling level / Operator output level
		writeOPL(0x43 + outValue, 0x3F);

		// Sustain Level / Release Rate
		writeOPL(0x83 + outValue, 0xFF);

		// Key On / Octave / Frequency
		writeOPL(0xB0 + _curTable, 0x00);
	}

	_curTable = tableBackUp;
	return 0;
}

int AdlibDriver::updateCallback39(uint8 *&dataptr, OutputState &state, uint8 value) {
	uint16 unk = *dataptr++;
	unk |= value << 8;
	unk &= getRandomNr();

	uint16 unk2 = ((state.regBx & 0x1F) << 8) | state.regAx;
	unk2 += unk;
	unk2 |= ((state.regBx & 0x20) << 8);

	// Frequency
	writeOPL(0xA0 + _curTable, unk2 & 0xFF);

	// Key On / Octave / Frequency
	writeOPL(0xB0 + _curTable, (unk2 & 0xFF00) >> 8);

	return 0;
}

int AdlibDriver::update_removePrimaryEffect2(uint8 *&dataptr, OutputState &state, uint8 value) {
	--dataptr;
	state.callback1 = 0;
	return 0;
}

int AdlibDriver::updateCallback41(uint8 *&dataptr, OutputState &state, uint8 value) {
	state.unk16 = value;
	setupNote(state.rawNote, state, true);
	return 0;
}

int AdlibDriver::updateCallback42(uint8 *&dataptr, OutputState &state, uint8 value) {
	--dataptr;
	state.unk1 = _tempo;
	return 0;
}

int AdlibDriver::updateCallback43(uint8 *&dataptr, OutputState &state, uint8 value) {
	--dataptr;
	return 0;
}

int AdlibDriver::updateCallback44(uint8 *&dataptr, OutputState &state, uint8 value) {
	state.unk11 = value;
	return 0;
}

int AdlibDriver::updateCallback45(uint8 *&dataptr, OutputState &state, uint8 value) {
	if (value & 0x80) {
		value += state.unk1;
		if ((int8)value >= (int8)state.unk1)
			value = 1;
	} else {
		value += state.unk1;
		if ((int8)value < 0)
			value = (uint8)-1;
	}
	state.unk1 = (int8)value;
	return 0;
}

int AdlibDriver::updateCallback46(uint8 *&dataptr, OutputState &state, uint8 value) {
	uint8 entry = *dataptr++;
	_tablePtr1 = _unkTable2[entry++];
	_tablePtr2 = _unkTable2[entry];
	if (value == 2) {
		// Frequency
		writeOPL(0xA0, _tablePtr2[0]);
	}
	return 0;
}

int AdlibDriver::updateCallback47(uint8 *&dataptr, OutputState &state, uint8 value) {
	--dataptr;
	return 0;
}

int AdlibDriver::updateCallback48(uint8 *&dataptr, OutputState &state, uint8 value) {
	int tableBackUp = _curTable;
	int regOffsetBackUp = _curRegOffset;

	uint8 entry = value << 1;
	uint8 *ptr = _soundData + READ_LE_UINT16(_soundData + entry + 0x1F4);

	_curTable = 6;
	_curRegOffset = _outputTable[6];

	_unkValue6 = *(ptr + 6);
	setupInstrument(_curRegOffset, ptr, state);

	entry = *dataptr++ << 1;
	ptr = _soundData + READ_LE_UINT16(_soundData + entry + 0x1F4);

	_curTable = 7;
	_curRegOffset = _outputTable[7];

	_unkValue7 = entry = *(ptr + 5);
	_unkValue8 = entry = *(ptr + 6);
	setupInstrument(_curRegOffset, ptr, state);

	entry = *dataptr++ << 1;
	ptr = _soundData + READ_LE_UINT16(_soundData + entry + 0x1F4);

	_curTable = 8;
	_curRegOffset = _outputTable[8];

	_unkValue9 = entry = *(ptr + 5);
	_unkValue10 = entry = *(ptr + 6);
	setupInstrument(_curRegOffset, ptr, state);

	// Octave / F-Number / Key-On for channels 6, 7 and 8

	_outputTables[6].regBx = *dataptr++ & 0x2F;
	writeOPL(0xB6, _outputTables[6].regBx);
	writeOPL(0xA6, *dataptr++);

	_outputTables[7].regBx = *dataptr++ & 0x2F;
	writeOPL(0xB7, _outputTables[7].regBx);
	writeOPL(0xA7, *dataptr++);

	_outputTables[8].regBx = *dataptr++ & 0x2F;
	writeOPL(0xB8, _outputTables[8].regBx);
	writeOPL(0xA8, *dataptr++);

	_unk4 = 0x20;

	_curRegOffset = regOffsetBackUp;
	_curTable = tableBackUp;
	return 0;
}

int AdlibDriver::updateCallback49(uint8 *&dataptr, OutputState &state, uint8 value) {
	// Amplitude Modulation Depth / Vibrato Depth / Rhythm
	writeOPL(0xBD, (((value & 0x1F) ^ 0xFF) & _unk4) | 0x20);

	value |= _unk4;
	_unk4 = value;

	value |= _unkOutputByte2;
	value |= 0x20;

	// FIXME: This could probably be replaced with writeOPL(0xBD, value),
	//        but to make it easier to compare the output to DOSbox, write
	//        directly to the data port and do the probably unnecessary
	//        delay loop.

	OPLWrite(_adlib, 0x389, value);

	for (int i = 0; i < 23;  i++)
		OPLRead(_adlib, 0x388);

	return 0;
}

int AdlibDriver::updateCallback50(uint8 *&dataptr, OutputState &state, uint8 value) {
	--dataptr;
	_unk4 = 0;

	// Amplitude Modulation Depth / Vibrato Depth / Rhythm
	writeOPL(0xBD, value & _unkOutputByte2);

	return 0;
}

int AdlibDriver::updateCallback51(uint8 *&dataptr, OutputState &state, uint8 value) {
	uint16 temp = (value << 8) | *dataptr++;

	if (value & 1) {
		uint8 val = temp & 0xFF;
		_unkValue12 = val;
		val += _unkValue7;
		val += _unkValue11;
		val += _unkValue12;

		// Channel 2: Level Key Scaling / Total Level
		writeOPL(0x51, checkValue(val));
	}

	if (value & 2) {
		uint8 val = temp & 0xFF;
		_unkValue14 = val;
		val += _unkValue10;
		val += _unkValue13;
		val += _unkValue14;

		// Channel 3: Level Key Scaling / Total Level
		writeOPL(0x55, checkValue(val));
	}

	if (value & 4) {
		uint8 val = temp & 0xFF;
		_unkValue15 = val;
		val += _unkValue9;
		val += _unkValue16;
		val += _unkValue15;

		// Channel 3: Level Key Scaling / Total Level
		writeOPL(0x52, checkValue(val));
	}

	if (value & 8) {
		uint8 val = temp & 0xFF;
		_unkValue18 = val;
		val += _unkValue8;
		val += _unkValue17;
		val += _unkValue18;

		// Channel 2: Level Key Scaling / Total Level
		writeOPL(0x54, checkValue(val));
	}

	if (value & 16) {
		uint8 val = temp & 0xFF;
		_unkValue20 = val;
		val += _unkValue6;
		val += _unkValue19;
		val += _unkValue20;

		// Channel 1: Level Key Scaling / Total Level
		writeOPL(0x53, checkValue(val));
	}

	return 0;
}

int AdlibDriver::updateCallback52(uint8 *&dataptr, OutputState &state, uint8 value) {
	uint16 temp = (value << 8) | *dataptr++;

	if (value & 1) {
		uint8 val = temp & 0xFF;
		val += _unkValue7;
		val += _unkValue11;
		val += _unkValue12;

		// Channel 2: Level Key Scaling / Total Level
		writeOPL(0x51, checkValue(val));
	}

	if (value & 2) {
		uint8 val = temp & 0xFF;
		val += _unkValue10;
		val += _unkValue13;
		val += _unkValue14;

		// Channel 3: Level Key Scaling / Total Level
		writeOPL(0x55, checkValue(val));
	}

	if (value & 4) {
		uint8 val = temp & 0xFF;
		val += _unkValue9;
		val += _unkValue16;
		val += _unkValue15;

		// Channel 3: Level Key Scaling / Total Level
		writeOPL(0x52, checkValue(val));
	}

	if (value & 8) {
		uint8 val = temp & 0xFF;
		val += _unkValue8;
		val += _unkValue17;
		val += _unkValue18;

		// Channel 2: Level Key Scaling / Total Level
		writeOPL(0x54, checkValue(val));
	}

	if (value & 16) {
		uint8 val = temp & 0xFF;
		val += _unkValue6;
		val += _unkValue19;
		val += _unkValue20;

		// Channel 1: Level Key Scaling / Total Level
		writeOPL(0x53, checkValue(val));
	}

	return 0;
}

int AdlibDriver::updateCallback53(uint8 *&dataptr, OutputState &state, uint8 value) {
	uint16 temp = (value << 8) | *dataptr++;

	if (value & 1) {
		uint8 val = temp & 0xFF;
		_unkValue11 = val;
		val += _unkValue7;
		val += _unkValue12;

		// Channel 2: Level Key Scaling / Total Level
		writeOPL(0x51, checkValue(val));
	}

	if (value & 2) {
		uint8 val = temp & 0xFF;
		_unkValue13 = val;
		val += _unkValue10;
		val += _unkValue14;

		// Channel 3: Level Key Scaling / Total Level
		writeOPL(0x55, checkValue(val));
	}

	if (value & 4) {
		uint8 val = temp & 0xFF;
		_unkValue16 = val;
		val += _unkValue9;
		val += _unkValue15;

		// Channel 3: Level Key Scaling / Total Level
		writeOPL(0x52, checkValue(val));
	}

	if (value & 8) {
		uint8 val = temp & 0xFF;
		_unkValue17 = val;
		val += _unkValue8;
		val += _unkValue18;

		// Channel 2: Level Key Scaling / Total Level
		writeOPL(0x54, checkValue(val));
	}

	if (value & 16) {
		uint8 val = temp & 0xFF;
		_unkValue19 = val;
		val += _unkValue6;
		val += _unkValue20;

		// Channel 1: Level Key Scaling / Total Level
		writeOPL(0x53, checkValue(val));
	}

	return 0;
}

int AdlibDriver::updateCallback54(uint8 *&dataptr, OutputState &state, uint8 value) {
	_unk5 = value;
	return 0;
}

int AdlibDriver::updateCallback55(uint8 *&dataptr, OutputState &state, uint8 value) {
	state.unk6 = value;
	return 0;
}

int AdlibDriver::updateCallback56(uint8 *&dataptr, OutputState &state, uint8 value) {
	state.unk39 = value;
	state.unk40 = *dataptr++;
	return 0;
}

// static res

#define COMMAND(x) { &AdlibDriver::x, #x }
const AdlibDriver::OpcodeEntry AdlibDriver::_opcodeList[] = {
	COMMAND(snd_ret0x100),
	COMMAND(snd_ret0x1983),
	COMMAND(snd_initDriver),
	COMMAND(snd_deinitDriver),
	COMMAND(snd_setSoundData),
	COMMAND(snd_unkOpcode1),
	COMMAND(snd_startSong),
	COMMAND(snd_unkOpcode2),
	COMMAND(snd_unkOpcode3),
	COMMAND(snd_readByte),
	COMMAND(snd_writeByte),
	COMMAND(snd_setUnk5),
	COMMAND(snd_unkOpcode4),
	COMMAND(snd_dummy),
	COMMAND(snd_getNullvar4),
	COMMAND(snd_setNullvar3),
	COMMAND(snd_setFlag),
	COMMAND(snd_clearFlag)
};

const AdlibDriver::ParserOpcode AdlibDriver::_parserOpcodeTable[] = {
	// 0
	COMMAND(update_setRepeat),
	COMMAND(update_checkRepeat),
	COMMAND(updateCallback3),
	COMMAND(updateCallback4),

	// 4
	COMMAND(update_jump),
	COMMAND(update_jumpToSubroutine),
	COMMAND(update_returnFromSubroutine),
	COMMAND(update_setBaseOctave),

	// 8
	COMMAND(updateCallback9),
	COMMAND(update_playRest),
	COMMAND(update_writeAdlib),
	COMMAND(updateCallback12),

	// 12
	COMMAND(update_setBaseNote),
	COMMAND(update_setupSecondaryEffect1),
	COMMAND(updateCallback15),
	COMMAND(updateCallback16),

	// 16
	COMMAND(updateCallback17),
	COMMAND(update_setupPrimaryEffect1),
	COMMAND(update_removePrimaryEffect1),
	COMMAND(update_setBaseFreq),

	// 20
	COMMAND(updateCallback9),
	COMMAND(update_setupPrimaryEffect2),
	COMMAND(updateCallback9),
	COMMAND(updateCallback9),

	// 24
	COMMAND(updateCallback9),
	COMMAND(updateCallback9),
	COMMAND(update_setPriority),
	COMMAND(updateCallback9),

	// 28
	COMMAND(updateCallback23),
	COMMAND(updateCallback24),
	COMMAND(update_setExtraLevel1),
	COMMAND(updateCallback9),

	// 32
	COMMAND(updateCallback26),
	COMMAND(update_playNote),
	COMMAND(updateCallback9),
	COMMAND(updateCallback9),

	// 36
	COMMAND(updateCallback28),
	COMMAND(updateCallback9),
	COMMAND(update_setTempo),
	COMMAND(update_removeSecondaryEffect1),

	// 40
	COMMAND(updateCallback9),
	COMMAND(updateCallback31),
	COMMAND(updateCallback9),
	COMMAND(update_setExtraLevel3),

	// 44
	COMMAND(update_setExtraLevel2),
	COMMAND(update_changeExtraLevel2),
	COMMAND(update_setAMDepth),
	COMMAND(update_setVibratoDepth),

	// 48
	COMMAND(update_changeExtraLevel1),
	COMMAND(updateCallback9),
	COMMAND(updateCallback9),
	COMMAND(updateCallback38),

	// 52
	COMMAND(updateCallback9),
	COMMAND(updateCallback39),
	COMMAND(update_removePrimaryEffect2),
	COMMAND(updateCallback9),

	// 56
	COMMAND(updateCallback9),
	COMMAND(updateCallback41),
	COMMAND(updateCallback42),
	COMMAND(updateCallback43),

	// 60
	COMMAND(updateCallback44),
	COMMAND(updateCallback45),
	COMMAND(updateCallback9),
	COMMAND(updateCallback46),

	// 64
	COMMAND(updateCallback47),
	COMMAND(updateCallback48),
	COMMAND(updateCallback49),
	COMMAND(updateCallback50),

	// 68
	COMMAND(updateCallback51),
	COMMAND(updateCallback52),
	COMMAND(updateCallback53),
	COMMAND(updateCallback54),

	// 72
	COMMAND(updateCallback55),
	COMMAND(updateCallback56),
	COMMAND(updateCallback9)
};
#undef COMMAND

const int AdlibDriver::_opcodesEntries = ARRAYSIZE(AdlibDriver::_opcodeList);
const int AdlibDriver::_parserOpcodeTableSize = ARRAYSIZE(AdlibDriver::_parserOpcodeTable);

// This table holds the register offset for operator 1 for each of the nine
// channels. To get the register offset for operator 2, simply add 3.

const uint8 AdlibDriver::_outputTable[] = {
	0x00, 0x01, 0x02, 0x08, 0x09, 0x0A, 0x10, 0x11,
	0x12
};

// Given the size of this table, and the range of its values, it's probably the
// F-Numbers (10 bits) for the notes of the 12-tone scale. However, it does not
// match the table in the Adlib documentation I've seen.

const uint16 AdlibDriver::_unkTable[] = {
	0x0134, 0x0147, 0x015A, 0x016F, 0x0184, 0x019C, 0x01B4, 0x01CE, 0x01E9,
	0x0207, 0x0225, 0x0246
};

// These tables are currently only used by updateCallback46(), which only ever
// uses the first element of one of the sub-tables.

const uint8 *AdlibDriver::_unkTable2[] = {
	AdlibDriver::_unkTable2_1,
	AdlibDriver::_unkTable2_2,
	AdlibDriver::_unkTable2_1,
	AdlibDriver::_unkTable2_2,
	AdlibDriver::_unkTable2_3,
	AdlibDriver::_unkTable2_2
};

const uint8 AdlibDriver::_unkTable2_1[] = {
	0x50, 0x50, 0x4F, 0x4F, 0x4E, 0x4E, 0x4D, 0x4D,
	0x4C, 0x4C, 0x4B, 0x4B, 0x4A, 0x4A, 0x49, 0x49,
	0x48, 0x48, 0x47, 0x47, 0x46, 0x46, 0x45, 0x45,
	0x44, 0x44, 0x43, 0x43, 0x42, 0x42, 0x41, 0x41,
	0x40, 0x40, 0x3F, 0x3F, 0x3E, 0x3E, 0x3D, 0x3D,
	0x3C, 0x3C, 0x3B, 0x3B, 0x3A, 0x3A, 0x39, 0x39,
	0x38, 0x38, 0x37, 0x37, 0x36, 0x36, 0x35, 0x35,
	0x34, 0x34, 0x33, 0x33, 0x32, 0x32, 0x31, 0x31,
	0x30, 0x30, 0x2F, 0x2F, 0x2E, 0x2E, 0x2D, 0x2D,
	0x2C, 0x2C, 0x2B, 0x2B, 0x2A, 0x2A, 0x29, 0x29,
	0x28, 0x28, 0x27, 0x27, 0x26, 0x26, 0x25, 0x25,
	0x24, 0x24, 0x23, 0x23, 0x22, 0x22, 0x21, 0x21,
	0x20, 0x20, 0x1F, 0x1F, 0x1E, 0x1E, 0x1D, 0x1D,
	0x1C, 0x1C, 0x1B, 0x1B, 0x1A, 0x1A, 0x19, 0x19,
	0x18, 0x18, 0x17, 0x17, 0x16, 0x16, 0x15, 0x15,
	0x14, 0x14, 0x13, 0x13, 0x12, 0x12, 0x11, 0x11,
	0x10, 0x10
};

// no don't ask me WHY this table exsits!
const uint8 AdlibDriver::_unkTable2_2[] = {
	0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
	0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
	0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
	0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F,
	0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27,
	0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F,
	0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
	0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F,
	0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47,
	0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F,
	0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57,
	0x58, 0x59, 0x5A, 0x5B, 0x5C, 0x5D, 0x5E, 0x6F,
	0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67,
	0x68, 0x69, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F,
	0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77,
	0x78, 0x79, 0x7A, 0x7B, 0x7C, 0x7D, 0x7E, 0x7F
};

const uint8 AdlibDriver::_unkTable2_3[] = {
	0x40, 0x40, 0x40, 0x3F, 0x3F, 0x3F, 0x3E, 0x3E,
	0x3E, 0x3D, 0x3D, 0x3D, 0x3C, 0x3C, 0x3C, 0x3B,
	0x3B, 0x3B, 0x3A, 0x3A, 0x3A, 0x39, 0x39, 0x39,
	0x38, 0x38, 0x38, 0x37, 0x37, 0x37, 0x36, 0x36,
	0x36, 0x35, 0x35, 0x35, 0x34, 0x34, 0x34, 0x33,
	0x33, 0x33, 0x32, 0x32, 0x32, 0x31, 0x31, 0x31,
	0x30, 0x30, 0x30, 0x2F, 0x2F, 0x2F, 0x2E, 0x2E,
	0x2E, 0x2D, 0x2D, 0x2D, 0x2C, 0x2C, 0x2C, 0x2B,
	0x2B, 0x2B, 0x2A, 0x2A, 0x2A, 0x29, 0x29, 0x29,
	0x28, 0x28, 0x28, 0x27, 0x27, 0x27, 0x26, 0x26,
	0x26, 0x25, 0x25, 0x25, 0x24, 0x24, 0x24, 0x23,
	0x23, 0x23, 0x22, 0x22, 0x22, 0x21, 0x21, 0x21,
	0x20, 0x20, 0x20, 0x1F, 0x1F, 0x1F, 0x1E, 0x1E,
	0x1E, 0x1D, 0x1D, 0x1D, 0x1C, 0x1C, 0x1C, 0x1B,
	0x1B, 0x1B, 0x1A, 0x1A, 0x1A, 0x19, 0x19, 0x19,
	0x18, 0x18, 0x18, 0x17, 0x17, 0x17, 0x16, 0x16,
	0x16, 0x15
};

// This table is used to modify the frequency of the notes, depending on the
// note value and unk16. In theory, we could very well try to access memory
// outside this table, but in reality that probably won't happen.
//
// This could be some sort of pitch bend, but I have yet to see it used for
// anything so it's hard to say.

// TODO: format this
const uint8 AdlibDriver::_unkTables[][32] = {
	{    0x00,  0x01,  0x02,  0x03,  0x04,  0x05,  0x06,  0x08,
	     0x09,  0x0A,  0x0B,  0x0C,  0x0D,  0x0E,  0x0F,  0x10,
	     0x11,  0x12,  0x13,  0x14,  0x15,  0x16,  0x17,  0x19,
	     0x1A,  0x1B,  0x1C,  0x1D,  0x1E,  0x1F,  0x20,  0x21 },
	{    0x00,  0x01,  0x02,  0x03,  0x04,  0x06,  0x07,  0x09,
	     0x0A,  0x0B,  0x0C,  0x0D,  0x0E,  0x0F,  0x10,  0x11,
	     0x12,  0x13,  0x14,  0x15,  0x16,  0x17,  0x18,  0x1A,
	     0x1B,  0x1C,  0x1D,  0x1E,  0x1F,  0x20,  0x22,  0x24 },
	{    0x00,  0x01,  0x02,  0x03,  0x04,  0x06,  0x08,  0x09,
	     0x0A,  0x0C,  0x0D,  0x0E,  0x0F,  0x11,  0x12,  0x13,
	     0x14,  0x15,  0x16,  0x17,  0x19,  0x1A,  0x1C,  0x1D,
	     0x1E,  0x1F,  0x20,  0x21,  0x22,  0x24,  0x25,  0x26 },
	{    0x00,  0x01,  0x02,  0x03,  0x04,  0x06,  0x08,  0x0A,
	     0x0B,  0x0C,  0x0D,  0x0E,  0x0F,  0x11,  0x12,  0x13,
	     0x14,  0x15,  0x16,  0x17,  0x18,  0x1A,  0x1C,  0x1D,
	     0x1E,  0x1F,  0x20,  0x21,  0x23,  0x25,  0x27,  0x28 },
	{    0x00,  0x01,  0x02,  0x03,  0x04,  0x06,  0x08,  0x0A,
	     0x0B,  0x0C,  0x0D,  0x0E,  0x0F,  0x11,  0x13,  0x15,
	     0x16,  0x17,  0x18,  0x19,  0x1B,  0x1D,  0x1F,  0x20,
	     0x21,  0x22,  0x23,  0x24,  0x25,  0x26,  0x28,  0x2A },
	{    0x00,  0x01,  0x02,  0x03,  0x05,  0x07,  0x09,  0x0B,
	     0x0C,  0x0D,  0x0E,  0x0F,  0x10,  0x11,  0x13,  0x15,
	     0x16,  0x17,  0x18,  0x19,  0x1B,  0x1D,  0x1F,  0x20,
	     0x21,  0x22,  0x23,  0x24,  0x25,  0x26,  0x28,  0x2A },
	{    0x00,  0x01,  0x02,  0x03,  0x05,  0x07,  0x09,  0x0B,
	     0x0C,  0x0D,  0x0E,  0x0F,  0x10,  0x11,  0x13,  0x15,
	     0x16,  0x17,  0x18,  0x19,  0x1B,  0x1D,  0x1F,  0x20,
	     0x21,  0x22,  0x23,  0x25,  0x27,  0x29,  0x2B,  0x2D },
	{    0x00,  0x01,  0x02,  0x03,  0x05,  0x07,  0x09,  0x0B,
	     0x0C,  0x0D,  0x0E,  0x0F,  0x10,  0x11,  0x13,  0x15,
	     0x16,  0x17,  0x18,  0x1A,  0x1C,  0x1E,  0x21,  0x24,
	     0x25,  0x26,  0x27,  0x29,  0x2B,  0x2D,  0x2F,  0x30 },
	{    0x00,  0x01,  0x02,  0x04,  0x06,  0x08,  0x0A,  0x0C,
	     0x0D,  0x0E,  0x0F,  0x10,  0x11,  0x13,  0x15,  0x18,
	     0x19,  0x1A,  0x1C,  0x1D,  0x1F,  0x21,  0x23,  0x25,
	     0x26,  0x27,  0x29,  0x2B,  0x2D,  0x2F,  0x30,  0x32 },
	{    0x00,  0x01,  0x02,  0x04,  0x06,  0x08,  0x0A,  0x0D,
	     0x0E,  0x0F,  0x10,  0x11,  0x12,  0x14,  0x17,  0x1A,
	     0x19,  0x1A,  0x1C,  0x1E,  0x20,  0x22,  0x25,  0x28,
	     0x29,  0x2A,  0x2B,  0x2D,  0x2F,  0x31,  0x33,  0x35 },
	{    0x00,  0x01,  0x03,  0x05,  0x07,  0x09,  0x0B,  0x0E,
	     0x0F,  0x10,  0x12,  0x14,  0x16,  0x18,  0x1A,  0x1B,
	     0x1C,  0x1D,  0x1E,  0x20,  0x22,  0x24,  0x26,  0x29,
	     0x2A,  0x2C,  0x2E,  0x30,  0x32,  0x34,  0x36,  0x39 },
	{    0x00,  0x01,  0x03,  0x05,  0x07,  0x09,  0x0B,  0x0E,
	     0x0F,  0x10,  0x12,  0x14,  0x16,  0x19,  0x1B,  0x1E,
	     0x1F,  0x21,  0x23,  0x25,  0x27,  0x29,  0x2B,  0x2D,
	     0x2E,  0x2F,  0x31,  0x32,  0x34,  0x36,  0x39,  0x3C },
	{    0x00,  0x01,  0x03,  0x05,  0x07,  0x0A,  0x0C,  0x0F,
	     0x10,  0x11,  0x13,  0x15,  0x17,  0x19,  0x1B,  0x1E,
	     0x1F,  0x20,  0x22,  0x24,  0x26,  0x28,  0x2B,  0x2E,
	     0x2F,  0x30,  0x32,  0x34,  0x36,  0x39,  0x3C,  0x3F },
	{    0x00,  0x02,  0x04,  0x06,  0x08,  0x0B,  0x0D,  0x10,
	     0x11,  0x12,  0x14,  0x16,  0x18,  0x1B,  0x1E,  0x21,
	     0x22,  0x23,  0x25,  0x27,  0x29,  0x2C,  0x2F,  0x32,
	     0x33,  0x34,  0x36,  0x38,  0x3B,  0x34,  0x41,  0x44 },
	{    0x00,  0x02,  0x04,  0x06,  0x08,  0x0B,  0x0D,  0x11,
	     0x12,  0x13,  0x15,  0x17,  0x1A,  0x1D,  0x20,  0x23,
	     0x24,  0x25,  0x27,  0x29,  0x2C,  0x2F,  0x32,  0x35,
	     0x36,  0x37,  0x39,  0x3B,  0x3E,  0x41,  0x44,  0x47 }
};

#pragma mark -

SoundAdlibPC::SoundAdlibPC(Audio::Mixer *mixer, KyraEngine *engine)
	: Sound(engine, mixer), _driver(0), _trackEntries(), _soundDataPtr(0) {
	memset(_trackEntries, 0, sizeof(_trackEntries));
	_driver = new AdlibDriver(mixer);
	assert(_driver);

	_sfxPlayingSound = -1;
	_soundFileLoaded = "";
}

SoundAdlibPC::~SoundAdlibPC() {
	delete [] _soundDataPtr;
	delete _driver;
}

bool SoundAdlibPC::init() {
	_driver->callback(2);
	_driver->callback(16, int(4));
	return true;
}

void SoundAdlibPC::setVolume(int volume) {
}

int SoundAdlibPC::getVolume() {
	return 0;
}

void SoundAdlibPC::playMusic(const char *file) {
	loadSoundFile(file);
}

void SoundAdlibPC::stopMusic() {
	//playSoundEffect(0);
}

void SoundAdlibPC::playTrack(uint8 track, bool looping) {
	playSoundEffect(track);
}

void SoundAdlibPC::haltTrack() {
	unk1();
	unk2();
	_engine->_system->delayMillis(3 * 60);
}

void SoundAdlibPC::startTrack() {
}

void SoundAdlibPC::loadSoundEffectFile(const char *file) {
	loadSoundFile(file);
}

void SoundAdlibPC::stopSoundEffect() {
}

void SoundAdlibPC::playSoundEffect(uint8 track) {
	uint8 soundId = _trackEntries[track];
	if ((int8)soundId == -1 || !_soundDataPtr)
		return;
	soundId &= 0xFF;
	while ((_driver->callback(16, 0) & 8)) {
		// We call the system delay and not the game delay to avoid concurrency issues.
		_engine->_system->delayMillis(10);
	}
	if (_sfxPlayingSound != -1) {
		_driver->callback(10, _sfxPlayingSound, int(1), int(_sfxSecondByteOfSong));
		_driver->callback(10, _sfxPlayingSound, int(3), int(_sfxFourthByteOfSong));
		_sfxPlayingSound = -1;
	}

	int firstByteOfSong = _driver->callback(9, soundId, int(0));

	if (firstByteOfSong != 9) {
		_sfxPlayingSound = soundId;
		_sfxSecondByteOfSong = _driver->callback(9, soundId, int(1));
		_sfxFourthByteOfSong = _driver->callback(9, soundId, int(3));

		int newVal = ((((-_sfxFourthByteOfSong) + 63) * 0xFF) >> 8) & 0xFF;
		newVal = -newVal + 63;
		_driver->callback(10, soundId, int(3), newVal);
		newVal = ((_sfxSecondByteOfSong * 0xFF) >> 8) & 0xFF;
		_driver->callback(10, soundId, int(1), newVal);
	}

	_driver->callback(6, soundId);
}

void SoundAdlibPC::beginFadeOut() {
	playSoundEffect(1);
}

bool SoundAdlibPC::fadeOut() {
	return false;
}

void SoundAdlibPC::loadSoundFile(const char *file) {
	if (_soundFileLoaded == file)
		return;

	if (_soundDataPtr) {
		haltTrack();
	}

	uint8 *file_data = 0; uint32 file_size = 0;

	char filename[25];
	sprintf(filename, "%s.ADL", file);

	file_data = _engine->resource()->fileData(filename, &file_size);
	if (!file_data) {
		warning("Couldn't find music file: '%s'", filename);
		return;
	}

	unk2();
	unk1();

	_driver->callback(8, int(-1));
	_soundDataPtr = 0;

	uint8 *p = file_data;
	memcpy(_trackEntries, p, 120*sizeof(uint8));
	p += 120;

	int soundDataSize = file_size - 120;

	_soundDataPtr = new uint8[soundDataSize];
	assert(_soundDataPtr);

	memcpy(_soundDataPtr, p, soundDataSize*sizeof(uint8));

	delete [] file_data;
	file_data = p = 0;
	file_size = 0;

	_driver->callback(4, _soundDataPtr);

	_soundFileLoaded = file;
}

void SoundAdlibPC::unk1() {
	playSoundEffect(0);
	//_engine->_system->delayMillis(5 * 60);
}

void SoundAdlibPC::unk2() {
	playSoundEffect(0);
}

} // end of namespace Kyra

