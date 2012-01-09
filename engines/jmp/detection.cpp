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

#include "base/plugins.h"

#include "engines/advancedDetector.h"
#include "common/config-manager.h"
#include "common/file.h"
#include "common/textconsole.h"

#include "jmp/jmp.h"
#include "jmp/jman.h"
#include "jmp/journey.h"
#include "jmp/bit.h"

namespace JMP {

struct JMPGameDescription {
	ADGameDescription desc;

	uint8 gameType;
	uint32 features;
	uint16 version;
};

uint32 JMPEngine::getFeatures() const {
	return _gameDescription->features;
}

Common::Platform JMPEngine::getPlatform() const {
	return _gameDescription->desc.platform;
}

uint16 JMPEngine::getVersion() const {
	return _gameDescription->version;
}

uint8 JMPEngine::getGameType() {
	return _gameDescription->gameType;
}

Common::Language JMPEngine::getLanguage() {
	return _gameDescription->desc.language;
}

const Common::String JMPEngine_BITTrailer::getEXEFileName() const {
	return _gameDescription->desc.filesDescriptions[0].fileName;
}

const Common::String JMPEngine_BITTrailer::getAVIFileName() const {
	return _gameDescription->desc.filesDescriptions[1].fileName;
}

bool JMPEngine::hasFeature(EngineFeature f) const {
	return
		(f == kSupportsRTL);
}

} // End of Namespace JMP

static const PlainGameDescriptor jmpGames[] = {
	{"jmp", "Journeyman Project game"},
	{"jman", "The Journeyman Project"},
	{"jmanturbo", "The Journeyman Project Turbo!"},
	{"journey", "The Journey (Making of The Journeyman Project)"},
	{"bit", "The Journeyman Project 2: Buried in Time"},
	{0, 0}
};


namespace JMP {

static const JMPGameDescription gameDescriptions[] = {
	// From the Turbo! CD
	{
		{
			"jman",
			"Trailer",
			AD_ENTRY1("JM1DEMO.AVI", "22ded699870850886163e718246c4845"),
			Common::EN_ANY,
			Common::kPlatformWindows,
			ADGF_NO_FLAGS,
			GUIO0()
		},
		GType_JMAN,
		GF_TRAILER,
		0,
	},

	{
		{
			"jmanturbo",
			"",
			AD_ENTRY1("JMAN.EXE", "4e557b8864b0eec060be6d31ce457858"),
			Common::EN_ANY,
			Common::kPlatformWindows,
			ADGF_NO_FLAGS,
			GUIO0()
		},
		GType_JMAN,
		0,
		0,
	},

#if 0
	// FIXME: Update to detection based on executable
	// From jvprat
	{
		{
			"jmanturbo",
			"",
			AD_ENTRY1("JMAN.EXE", ""),
			Common::ES_ESP,
			Common::kPlatformWindows,
			ADGF_NO_FLAGS,
			GUIO0()
		},
		GType_JMAN,
		0,
		0,
	},
#endif

	// From Turbo! CD
	{
		{
			"journey",
			"",
			AD_ENTRY1("RAWMENU.BMP", "3dca52be206997aef270d4b1e6fc66c5"),
			Common::EN_ANY,
			Common::kPlatformWindows,
			ADGF_NO_FLAGS,
			GUIO0()
		},
		GType_JOURNEY,
		0,
		0,
	},
		
	// TODO: Use non-compressed version :P
	{
		{
			"bit",
			"",
			AD_ENTRY1("BIT2416.EX_", "a9ac76610ba614b59235a7d5e00e4a62"),
			Common::EN_ANY,
			Common::kPlatformWindows,
			ADGF_NO_FLAGS,
			GUIO0()
		},
		GType_BIT,
		0,
		0,
	},
	
	// Demo from CD-ROM Today (from clone2727)
	{
		{
			"bit",
			"Demo",
			AD_ENTRY1("BIT2416.EXE", "9857e2d2b7a63b1304058dabc5098249"),
			Common::EN_ANY,
			Common::kPlatformWindows,
			ADGF_DEMO,
			GUIO0()
		},
		GType_BIT,
		GF_DEMO,
		0,
	},

	// From the Turbo! CD
	{
		{
			"bit",
			"Old Trailer",
			AD_ENTRY1("DEMOMV.AVI", "577d2f394e37266cba8dc14cea4b2df6"),
			Common::EN_ANY,
			Common::kPlatformWindows,
			ADGF_NO_FLAGS,
			GUIO0()
		},
		GType_BIT,
		GF_TRAILER,
		0,
	},

	{
		{
			"bit",
			"Gallery",
			{
				{ "BITGALRY.EXE", 0, "60a13a61d2d443b05f0ed0efd3f5922f", 1268736 },
				{ "BITGALRY.AVI", 0, "24026844d5dd77ff545f353d6eabbd46", 23331162 },
				AD_LISTEND
			},
			Common::EN_ANY,
			Common::kPlatformWindows,
			ADGF_NO_FLAGS,
			GUIO0()
		},
		GType_BIT,
		GF_TRAILER,
		0
	},
	
	{
		{
			"bit",
			"Overview",
			{
				{ "BITOVRVW.EXE", 0, "78c828748a2864d1bee9dd95a4fd3dc6", 1268736 },
				{ "BITOVRVW.AVI", 0, "64ff05d9276b8ddf488a5d8ac4ab4e48", 20756302 },
				AD_LISTEND
			},
			Common::EN_ANY,
			Common::kPlatformWindows,
			ADGF_NO_FLAGS,
			GUIO0()
		},
		GType_BIT,
		GF_TRAILER,
		0
	},

	{
		{
			"bit",
			"Trailer",
			{
				{ "BITTRAIL.EXE", 0, "1c91c981479689326431471bea3e72d0", 1268736 },
				{ "BITTRAIL.AVI", 0, "932ddf50f20fe58b0b55fcfc893b4a66", 24004940 },
				AD_LISTEND
			},
			Common::EN_ANY,
			Common::kPlatformWindows,
			ADGF_NO_FLAGS,
			GUIO0()
		},
		GType_BIT,
		GF_TRAILER,
		0
	},

	{
		{
			"bit",
			"DC Trailer",
			{
				{ "JDCDEMO.EXE", 0, "9945c9c101b962945f502559850f11b0", 329088 },
				{ "JMDCDEMO.AVI", 0, "c04b887f5e05846dc9a4c8e3b9515abf", 48475032 },
				AD_LISTEND
			},
			Common::EN_ANY,
			Common::kPlatformWindows,
			ADGF_NO_FLAGS,
			GUIO0()
		},
		GType_BIT,
		GF_TRAILER,
		0
	},


	{ AD_TABLE_END_MARKER, 0, 0, 0 }
};

} // End of namespace JMP

class JMPMetaEngine : public AdvancedMetaEngine {
public:
	JMPMetaEngine() : AdvancedMetaEngine(JMP::gameDescriptions, sizeof(JMP::JMPGameDescription), jmpGames) {
		_singleid = "jmp";
	}

	virtual const char *getName() const {
		return "JMP Engine";
	}

	virtual const char *getOriginalCopyright() const {
		return "The Journeyman Project (C) Presto Studios";
	}

	virtual bool createInstance(OSystem *syst, Engine **engine, const ADGameDescription *desc) const;
};

bool JMPMetaEngine::createInstance(OSystem *syst, Engine **engine, const ADGameDescription *desc) const {
	const JMP::JMPGameDescription *gd = (const JMP::JMPGameDescription *)desc;
	if (gd) {
		switch (gd->gameType) {
		case JMP::GType_JMAN:
			if (gd->features & JMP::GF_TRAILER)
				*engine = new JMP::JMPEngine_JMANTrailer(syst, gd);
			else
				*engine = new JMP::JMPEngine_JMAN(syst, gd);
			break;
		case JMP::GType_JOURNEY:
			*engine = new JMP::JMPEngine_Journey(syst, gd);
			break;
		case JMP::GType_BIT:
			if (gd->features & JMP::GF_DEMO)
				*engine = new JMP::JMPEngine_BITDemo(syst, gd);
			else if (gd->features & JMP::GF_TRAILER)
				*engine = new JMP::JMPEngine_BITTrailer(syst, gd);
			else
				*engine = new JMP::JMPEngine_BIT(syst, gd);
			break;
		default:
			error ("Unknown/Unsupported JMP Engine!");		
		}
	}
	return (gd != 0);
}

#if PLUGIN_ENABLED_DYNAMIC(JMP)
	REGISTER_PLUGIN_DYNAMIC(JMP, PLUGIN_TYPE_ENGINE, JMPMetaEngine);
#else
	REGISTER_PLUGIN_STATIC(JMP, PLUGIN_TYPE_ENGINE, JMPMetaEngine);
#endif

