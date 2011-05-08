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
	{"turbo", "The Journeyman Project Turbo!"},
	{"journey", "The Journey (Making of The Journeyman Project)"},
	{"bit", "The Journeyman Project 2: Buried in Time"},
	{0, 0}
};


namespace JMP {

static const JMPGameDescription gameDescriptions[] = {
	{
		{
			"turbo",
			"",
			AD_ENTRY1("MAINMNU1.BMP", "0eed699e30b7563d9791d63443739f8b"),
			Common::EN_ANY,
			Common::kPlatformWindows,
			ADGF_NO_FLAGS,
			Common::GUIO_NONE
		},
		GType_JMAN,
		0,
		0,
	},

	// From jvprat
	{
		{
			"turbo",
			"",
			AD_ENTRY1("MAINMNU1.BMP", "d75b9682be004aae0c28164ef492e603"),
			Common::ES_ESP,
			Common::kPlatformWindows,
			ADGF_NO_FLAGS,
			Common::GUIO_NONE
		},
		GType_JMAN,
		0,
		0,
	},

	// From Turbo! CD

	{
		{
			"journey",
			"",
			AD_ENTRY1("RAWMENU.BMP", "3dca52be206997aef270d4b1e6fc66c5"),
			Common::EN_ANY,
			Common::kPlatformWindows,
			ADGF_NO_FLAGS,
			Common::GUIO_NONE
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
			Common::GUIO_NONE
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
			Common::GUIO_NONE
		},
		GType_BIT,
		GF_DEMO,
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
			Common::GUIO_NONE
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
			Common::GUIO_NONE
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
			Common::GUIO_NONE
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
			Common::GUIO_NONE
		},
		GType_BIT,
		GF_TRAILER,
		0
	},


	{ AD_TABLE_END_MARKER, 0, 0, 0 }
};

//////////////////////////////
//Fallback detection
//////////////////////////////

static const JMPGameDescription fallbackDescs[] = {
	{
		{
			"turbo",
			"unknown",
			AD_ENTRY1(0, 0),
			Common::UNK_LANG,
			Common::kPlatformWindows,
			ADGF_NO_FLAGS,
			Common::GUIO_NONE
		},
		GType_JMAN,
		0,
		0
	}
};

static const ADFileBasedFallback fileBased[] = {
	{ &fallbackDescs[0], { "MAINMNU1.BMP", 0 } },
	{ 0, { 0 } }
};

} // End of namespace JMP

static const ADParams detectionParams = {
	// Pointer to ADGameDescription or its superset structure
	(const byte *)JMP::gameDescriptions,
	// Size of that superset structure
	sizeof(JMP::JMPGameDescription),
	// Number of bytes to compute MD5 sum for
	5000,
	// List of all engine targets
	jmpGames,
	// Structure for autoupgrading obsolete targets
	0,
	// Name of single gameid (optional)
	"jmp",
	// List of files for file-based fallback detection (optional)
	JMP::fileBased,
	// Flags
	0,
	// Additional GUI options (for every game)
	Common::GUIO_NONE,
	// Maximum directory depth
	1,
	// List of directory globs
	0
};

class JMPMetaEngine : public AdvancedMetaEngine {
public:
	JMPMetaEngine() : AdvancedMetaEngine(detectionParams) {}

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

