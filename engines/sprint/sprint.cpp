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
#include "sprint/sprint.h"

namespace Sprint {

SprintEngine::SprintEngine(OSystem *syst, const SprintGameDescription *desc) : Engine(syst), _gameDescription(desc) {
	_resFork = 0;
	_database = 0;

	// Add subdirectories to the search path
	const Common::FSNode gameDataDir(ConfMan.get("path"));
	SearchMan.addSubDirectoryMatching(gameDataDir, "CD Data");
	SearchMan.addSubDirectoryMatching(gameDataDir, "CD Data/m");
	SearchMan.addSubDirectoryMatching(gameDataDir, "Myst ME Data");
}

SprintEngine::~SprintEngine() {
	delete _resFork;
	delete _database;
}

Common::Error SprintEngine::run() {
	_resFork = new Common::MacResManager();

	if (!_resFork->open("Myst Masterpiece") || !_resFork->hasDataFork() || !_resFork->hasResFork())
		error("Could not open resource fork");

	_database = new Database(getExecutableVersion(), _resFork);

	return Common::kNoError;
}

} // end of namespace Sprint
