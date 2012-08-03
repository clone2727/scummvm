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

#include "common/archive.h"
#include "common/config-manager.h"
#include "common/error.h"
#include "common/fs.h"
#include "common/macresman.h"

#include "sprint/database.h"
#include "sprint/graphics.h"
#include "sprint/script.h"
#include "sprint/sprint.h"

namespace Sprint {

SprintEngine::SprintEngine(OSystem *syst, const SprintGameDescription *desc) : Engine(syst), _gameDescription(desc) {
	_resFork = 0;
	_database = 0;
	_gfx = 0;
	_script = 0;
	_curAge = 0;
	_curNode = 0;

	// Add subdirectories to the search path
	const Common::FSNode gameDataDir(ConfMan.get("path"));
	SearchMan.addSubDirectoryMatching(gameDataDir, "CD Data");
	SearchMan.addSubDirectoryMatching(gameDataDir, "CD Data/m");
	SearchMan.addSubDirectoryMatching(gameDataDir, "Myst ME Data");
}

SprintEngine::~SprintEngine() {
	delete _resFork;
	delete _database;
	delete _gfx;
}

Common::Error SprintEngine::run() {
	_resFork = new Common::MacResManager();

	if (!_resFork->open("Myst Masterpiece") || !_resFork->hasDataFork() || !_resFork->hasResFork())
		error("Could not open resource fork");

	_database = new Database(getExecutableVersion(), _resFork);
	_gfx = new GraphicsManager(_resFork);
	_script = new ScriptManager(this);

	// Start us off on the init script
	changeToNode(1, 1);

	// TODO: Main loop

	return Common::kNoError;
}

void SprintEngine::changeToNode(uint age, uint room, uint node) {
	if (room != 1)
		error("Myst ME Mac has only one room per age");

	_curAge = age;
	_curNode = node;
	_curNodeData = _database->getNode(age, node);

	Node nodeData = _curNodeData;

	for (uint i = 0; i < nodeData.mainScripts.size(); i++)
		if (true) // TODO: Evaluate conditions
			_script->execute(nodeData.mainScripts[i].script);
}

void SprintEngine::changeToNode(uint age, uint node) {
	changeToNode(age, 1, node);
}

} // end of namespace Sprint
