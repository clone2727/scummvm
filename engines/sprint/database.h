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

#ifndef SPRINT_DATABASE_H
#define SPRINT_DATABASE_H

#include "common/array.h"
#include "common/hashmap.h"
#include "common/str.h"

namespace Common {
class MacResManager;
class SeekableReadStream;
}

namespace Sprint {

class SprintEngine;
struct ExecutableVersion;

struct ScriptOpcode {
	byte op;
	Common::Array<int16> args;
};

typedef Common::Array<ScriptOpcode> Script;

struct ConditionalScript {
	uint16 condition;
	Script script;
};

typedef Common::Array<ConditionalScript> ConditionalScriptList;

class Database {
public:
	Database(const ExecutableVersion *executableVersion, Common::MacResManager *resFork);

private:
	Common::MacResManager *_resFork;
	const ExecutableVersion *_executableVersion;

	struct AgeData {
		uint16 id;
		uint16 disk;
		uint32 u0;
		uint32 ageDataOffset;
		uint32 u1;
		uint32 u2;
		uint32 u3;
		uint32 ageNameOffset;

		Common::String name;

		uint16 prefix;
		uint32 mainScriptOffset;
		uint32 soundScriptOffset;
	};

	typedef Common::HashMap<uint16, Common::String> StringTable;

	Common::Array<AgeData> _ages;
	StringTable _soundNames, _movieNames, _helpTable, _urlTable;

	void loadAges(Common::SeekableReadStream &s);
	void loadSoundNames(Common::SeekableReadStream &s);
	void loadMovieNames(Common::SeekableReadStream &s);
	void loadAgeNames(Common::SeekableReadStream &s);
	void loadAgeScriptOffsets(Common::SeekableReadStream &s);
	void loadHelpTable(Common::SeekableReadStream &s);
	void loadURLTable(Common::SeekableReadStream &s);
	void loadAgeMainScripts(Common::SeekableReadStream &s);
	void loadAgeSoundScripts(Common::SeekableReadStream &s);

	Script readScript(Common::SeekableReadStream &s);
	ConditionalScriptList readConditionalScripts(Common::SeekableReadStream &s);

	Common::SeekableReadStream *decompressPEFDataSegment(Common::SeekableReadStream *stream, uint segmentID) const;
};

} // End of namespace Sprint

#endif
