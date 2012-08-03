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

#ifndef SPRINT_H
#define SPRINT_H

#include "engines/engine.h"

#include "sprint/database.h"

namespace Common {
class MacResManager;
}

namespace Sprint {

enum GameVersionFlags {
	kFlagNone = 0,
	kFlagDVD = (1 << 2)  // DVD version
};

struct ExecutableVersion {
	const char *description;
	int flags;
	uint32 ageTableOffset;
	uint32 soundTableOffset;
	uint32 movieTableOffset;
	uint32 helpTableOffset;
	uint32 urlTableOffset;
};

class GraphicsManager;
class ScriptManager;
struct SprintGameDescription;

class SprintEngine : public Engine {
public:
	SprintEngine(OSystem *syst, const SprintGameDescription *version);
	virtual ~SprintEngine();

	Common::Error run();

	const ExecutableVersion *getExecutableVersion() const;

	Database *_database;

	void changeToNode(uint age, uint node);
	void changeToNode(uint age, uint room, uint node);

private:
	const SprintGameDescription *_gameDescription;

	Common::MacResManager *_resFork;
	GraphicsManager *_gfx;
	ScriptManager *_script;

	Node _curNodeData;
	uint _curAge, _curNode;
};

} // End of namespace Sprint

#endif
