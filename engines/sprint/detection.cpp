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

#include "engines/advancedDetector.h"

#include "sprint/sprint.h"

#include "common/savefile.h"

namespace Sprint {

struct SprintGameDescription {
	ADGameDescription desc;
	const ExecutableVersion *version;
};

const ExecutableVersion *SprintEngine::getExecutableVersion() const {
	return _gameDescription->version;
}

static const PlainGameDescriptor sprintGames[] = {
	{ "sprint", "Sprint game" },
	{ "myst", "Myst" },
	{ 0, 0 }
};

static const ExecutableVersion enDVDVersion = { "DVD", kFlagDVD, 0x2598, 0x2678, 0x2DA8, 0x38FAC, 0x35B8 };

static const SprintGameDescription gameDescriptions[] = {
	// Myst 10th Anniversary DVD
	{
		{
			"myst",
			"Masterpiece Edition DVD",
			AD_ENTRY1("Myst Masterpiece", "ccc930cb7feae82383ebad3d2759f995"),
			Common::EN_ANY,
			Common::kPlatformMacintosh,
			ADGF_MACRESFORK,
			GUIO_NONE
		},
		&enDVDVersion,																	\
	},

	{ AD_TABLE_END_MARKER, 0 }
};

class SprintMetaEngine : public AdvancedMetaEngine {
public:
	SprintMetaEngine() : AdvancedMetaEngine(gameDescriptions, sizeof(SprintGameDescription), sprintGames) {
		_singleid = "sprint";
	}

	const char *getName() const {
		return "Sprint Engine";
	}

	const char *getOriginalCopyright() const {
		return "Sprint Engine (C) Presto Studios";
	}

	bool createInstance(OSystem *syst, Engine **engine, const ADGameDescription *desc) const;
};

bool SprintMetaEngine::createInstance(OSystem *syst, Engine **engine, const ADGameDescription *desc) const {
	const SprintGameDescription *gd = (const SprintGameDescription *)desc;

	if (gd)
		*engine = new SprintEngine(syst, gd);

	return gd != 0;
}

} // End of namespace Sprint

#if PLUGIN_ENABLED_DYNAMIC(SPRINT)
	REGISTER_PLUGIN_DYNAMIC(SPRINT, PLUGIN_TYPE_ENGINE, Sprint::SprintMetaEngine);
#else
	REGISTER_PLUGIN_STATIC(SPRINT, PLUGIN_TYPE_ENGINE, Sprint::SprintMetaEngine);
#endif
