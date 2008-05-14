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
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 * $URL$
 * $Id$
 */

#include "sound/midiplugin.h"
#include "sound/mpu401.h"

/* NULL driver */
class MidiDriver_NULL : public MidiDriver_MPU401 {
public:
	int open() { return 0; }
	void send(uint32 b) { }
};


// Plugin interface

class NullMidiPlugin : public MidiPluginObject {
public:
	virtual const char *getName() const {
		return "No music";
	}

	virtual const char *getId() const {
		return "null";
	}

	virtual int getCapabilities() const {
		return MDT_MIDI | MDT_PCSPK | MDT_ADLIB | MDT_TOWNS;
	}

	virtual PluginError createInstance(Audio::Mixer *mixer, MidiDriver **mididriver) const;
};

PluginError NullMidiPlugin::createInstance(Audio::Mixer *mixer, MidiDriver **mididriver) const {
	*mididriver = new MidiDriver_NULL();

	return kNoError;
}

MidiDriver *MidiDriver_NULL_create(Audio::Mixer *mixer) {
	MidiDriver *mididriver;

	NullMidiPlugin p;
	p.createInstance(mixer, &mididriver);

	return mididriver;
}

#ifdef DISABLE_ADLIB
MidiDriver *MidiDriver_ADLIB_create(Audio::Mixer *mixer) {
	return MidiDriver_NULL_create(mixer);
}
#endif

//#if PLUGIN_ENABLED_DYNAMIC(NULL)
	//REGISTER_PLUGIN_DYNAMIC(NULL, PLUGIN_TYPE_MIDI, NullMidiPlugin);
//#else
	REGISTER_PLUGIN_STATIC(NULL, PLUGIN_TYPE_MIDI, NullMidiPlugin);
//#endif
