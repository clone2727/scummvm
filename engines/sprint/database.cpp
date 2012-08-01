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

#include "common/debug.h"
#include "common/macresman.h"
#include "common/memstream.h"
#include "common/stream.h"

#include "sprint/database.h"
#include "sprint/sprint.h"

namespace Sprint {

Database::Database(const ExecutableVersion *executableVersion, Common::MacResManager *resFork) : _executableVersion(executableVersion), _resFork(resFork) {
	Common::SeekableReadStream *dataFork = _resFork->getDataFork();
	Common::SeekableReadStream *segment = decompressPEFDataSegment(dataFork, 1);
	delete dataFork;

	loadAges(*segment);
	loadSoundNames(*segment);
	loadMovieNames(*segment);
	loadHelpTable(*segment);
	loadURLTable(*segment);
	loadAgeSoundScripts(*segment);

	delete segment;
}

void Database::loadAges(Common::SeekableReadStream &s) {
	s.seek(_executableVersion->ageTableOffset);

	for (uint i = 0; i < 8; i++) {
		AgeData age;

		age.id = s.readUint16BE();
		age.disk = s.readUint16BE();
		age.u0 = s.readUint32BE(); // always 0 or 0x10000
		age.ageDataOffset = s.readUint32BE();
		age.u1 = s.readUint32BE(); // pointer (can be zero)
		age.u2 = s.readUint32BE(); // pointer (can be zero)
		age.u3 = s.readUint32BE(); // pointer (can be zero)
		age.ageNameOffset = s.readUint32BE();

		_ages.push_back(age);
	}

	loadAgeNames(s);
	loadAgeScriptOffsets(s);

	for (uint i = 0; i < 8; i++) {
		AgeData &age = _ages[i];
		debug("Age: %d, %d, %x, %x", age.id, age.disk, age.ageDataOffset, age.ageNameOffset);
		debug("\tMain Script Offset: %x", age.mainScriptOffset);
		debug("\tSound Script Offset: %x", age.soundScriptOffset);
		debug("\tUnks: %x, %x, %x, %x", age.u0, age.u1, age.u2, age.u3);

		if (!age.name.empty()) // NOTE: Intro/Credits don't have a name
			debug("\tName: %s", age.name.c_str());

		debug("\tPrefix: %c%c", age.prefix >> 8, age.prefix & 0xFF);
	}
}

static Common::String readCString(Common::SeekableReadStream &s) {
	Common::String string;

	for (char c = s.readByte(); c != 0; c = s.readByte())
		string += c;

	return string;
}

void Database::loadAgeNames(Common::SeekableReadStream &s) {
	for (uint i = 0; i < 8; i++) {
		s.seek(_ages[i].ageNameOffset);
		_ages[i].name = readCString(s);
	}
}

void Database::loadAgeScriptOffsets(Common::SeekableReadStream &s) {
	for (uint i = 0; i < 8; i++) {
		s.seek(_ages[i].ageDataOffset);
		s.seek(s.readUint32BE() + 2); // first two bytes are always 1

		_ages[i].prefix = s.readUint16BE();
		_ages[i].mainScriptOffset = s.readUint32BE();
		_ages[i].soundScriptOffset = s.readUint32BE();
	}
}

void Database::loadSoundNames(Common::SeekableReadStream &s) {
	s.seek(_executableVersion->soundTableOffset);

	for (;;) {
		uint32 id = s.readUint16BE();

		if (!id)
			break;

		char name[9];
		name[8] = 0;
		s.read(name, 8);
		_soundNames[id] = Common::String(name) + ".aif";

		debug("Sound: %d -> '%s'", id, _soundNames[id].c_str());
	}
}

void Database::loadMovieNames(Common::SeekableReadStream &s) {
	s.seek(_executableVersion->movieTableOffset);

	for (;;) {
		uint32 id = s.readUint16BE();

		if (!id)
			break;

		char name[9];
		name[8] = 0;
		s.read(name, 8);
		_movieNames[id] = Common::String(name) + ".mov";

		debug("Movie: %d -> '%s'", id, _movieNames[id].c_str());
	}
}

void Database::loadHelpTable(Common::SeekableReadStream &s) {
	s.seek(_executableVersion->helpTableOffset);

	for (;;) {
		uint32 id = s.readUint16BE();

		if (!id)
			break;

		s.readUint16BE(); // always 0
		uint32 offset = s.readUint32BE();
		uint32 pos = s.pos();
		s.seek(offset);

		_helpTable[id] = readCString(s);

		debug(0, "Help: %d -> '%s'", id, _helpTable[id].c_str());

		s.seek(pos);
	}
}

void Database::loadURLTable(Common::SeekableReadStream &s) {
	s.seek(_executableVersion->urlTableOffset);

	for (;;) {
		uint32 id = s.readUint16BE();

		if (!id)
			break;

		s.readUint16BE(); // always 0
		uint32 offset = s.readUint32BE();
		uint32 pos = s.pos();
		s.seek(offset);

		_urlTable[id] = readCString(s);

		debug("URL: %d -> '%s'", id, _urlTable[id].c_str());

		s.seek(pos);
	}
}

void Database::loadAgeSoundScripts(Common::SeekableReadStream &s) {
	for (uint i = 0; i < 8; i++) {
		if (_ages[i].soundScriptOffset == 0) // Nothing for 'XX'
			continue;

		s.seek(_ages[i].soundScriptOffset);

		for (;;) {
			int16 id = s.readSint16BE();

			Common::Array<uint16> scriptIDs;

			if (id == 0) {
				// Done with sound scripts
				break;
			} else if (id < -10) {
				// Can't be less than -10
				error("Invalid sound script id %d", id);
			} else if (id == -10) {
				// Complex list of id's
				for (;;) {
					id = s.readSint16BE();

					if (id == 0) {
						break;
					} else if (id < 0) {
						uint16 end = s.readUint16BE();

						for (int j = -id; j < end; j++)
							scriptIDs.push_back(j);
					} else {
						scriptIDs.push_back(id);
					}
				}
			} else if (id < 0) {
				// Simple list of id's
				for (int j = 0; j < -id; j++)
					scriptIDs.push_back(s.readUint16BE());
			} else {
				// Single id
				scriptIDs.push_back(id);
			}

			/* ConditionalScriptList scripts = */ readConditionalScripts(s);
		}
	}
}

Script Database::readScript(Common::SeekableReadStream &s) {
	Script script;

	for (;;) {
		uint16 code = s.readUint16BE();

		if (code == 0)
			break;

		ScriptOpcode opcode;
		opcode.op = code & 0xff;

		for (uint i = 0; i < (code >> 8); i++)
			opcode.args.push_back(s.readSint16BE());

		script.push_back(opcode);
	}

	return script;
}

ConditionalScriptList Database::readConditionalScripts(Common::SeekableReadStream &s) {
	ConditionalScriptList scriptList;

	for (;;) {
		uint16 condition = s.readUint16BE();

		if (!condition)
			break;

		ConditionalScript conditionalScript;
		conditionalScript.condition = condition;
		conditionalScript.script = readScript(s);
		scriptList.push_back(conditionalScript);
	}

	return scriptList;
}

static uint32 getPEFArgument(Common::SeekableReadStream *stream, uint &pos) {
	uint32 r = 0;
	byte numEntries = 0;

	for (;;) {
		numEntries++;

		byte in = stream->readByte();
		pos++;

		if (numEntries == 5) {
			r <<= 4;
		} else {
			r <<= 7;
		}

		r += (in & 0x7f);

		if (!(in & 0x80))
			return r;

		if (numEntries == 5)
			error("bad argument in PEF");
	}
}

// decompressPEFDataSegment is entirely based on https://github.com/fuzzie/unity/blob/master/data.cpp
Common::SeekableReadStream *Database::decompressPEFDataSegment(Common::SeekableReadStream *stream, uint segmentID) const {
	// Read the header
	if (stream->readUint32BE() != MKTAG('J','o','y','!'))
		error("Bad PEF header tag 1");
	if (stream->readUint32BE() != MKTAG('p','e','f','f'))
		error("Bad PEF header tag 2");
	if (stream->readUint32BE() != MKTAG('p','w','p','c'))
		error("PEF header is not PowerPC");
	if (stream->readUint32BE() != 1)
		error("PEF header is not version 1");

	stream->skip(16); // dateTimeStamp, oldDefVersion, oldImpVersion, currentVersion
	uint16 sectionCount = stream->readUint16BE();
	stream->skip(6); // instSectionCount, reservedA

	if (segmentID >= sectionCount)
		error("Not enough segments in PEF");

	stream->skip(28 * segmentID);

	stream->skip(8); // nameOffset, defaultAddress
	uint32 totalSize = stream->readUint32BE();
	uint32 unpackedSize = stream->readUint32BE();
	uint32 packedSize = stream->readUint32BE();
	assert(unpackedSize <= totalSize);
	assert(packedSize <= unpackedSize);
	uint32 containerOffset = stream->readUint32BE();
	byte sectionKind = stream->readByte();

	switch (sectionKind) {
	case 2:
		break; // pattern-initialized data
	default:
		error("Unsupported PEF sectionKind %d", sectionKind);
	}

	debug(1, "Unpacking PEF segment of size %d (total %d, packed %d) at 0x%x", unpackedSize, totalSize, packedSize, containerOffset);

	bool r = stream->seek(containerOffset, SEEK_SET);
	assert(r);

	// note that we don't bother with the zero-initialised section..
	byte *data = (byte *)malloc(unpackedSize);

	// unpack the data
	byte *targ = data;
	unsigned int pos = 0;
	while (pos < packedSize) {
		byte next = stream->readByte();
		byte opcode = next >> 5;
		uint32 count = next & 0x1f;
		pos++;

		if (count == 0)
			count = getPEFArgument(stream, pos);

		switch (opcode) {
		case 0: // Zero
			memset(targ, 0, count);
			targ += count;
			break;

		case 1: // blockCopy
			stream->read(targ, count);
			targ += count;
			pos += count;
			break;

		case 2:	{ // repeatedBlock
				uint32 repeatCount = getPEFArgument(stream, pos);

				byte *src = targ;
				stream->read(src, count);
				targ += count;
				pos += count;

				for (uint i = 0; i < repeatCount; i++) {
					memcpy(targ, src, count);
					targ += count;
				}
			} break;

		case 3: { // interleaveRepeatBlockWithBlockCopy
				uint32 customSize = getPEFArgument(stream, pos);
				uint32 repeatCount = getPEFArgument(stream, pos);

				byte *commonData = targ;
				stream->read(commonData, count);
				targ += count;
				pos += count;

				for (uint i = 0; i < repeatCount; i++) {
					stream->read(targ, customSize);
					targ += customSize;
					pos += customSize;

					memcpy(targ, commonData, count);
					targ += count;
				}
			} break;

		case 4: { // interleaveRepeatBlockWithZero
				uint32 customSize = getPEFArgument(stream, pos);
				uint32 repeatCount = getPEFArgument(stream, pos);

				for (uint i = 0; i < repeatCount; i++) {
					memset(targ, 0, count);
					targ += count;

					stream->read(targ, customSize);
					targ += customSize;
					pos += customSize;
				}
				memset(targ, 0, count);
				targ += count;
			} break;

		default:
			error("Unknown opcode %d in PEF pattern-initialized section", opcode);
		}
	}

	if (pos != packedSize)
		error("Failed to parse PEF pattern-initialized section (parsed %d of %d)", pos, packedSize);
	if (targ != data + unpackedSize)
		error("Failed to unpack PEF pattern-initialized section");

	return new Common::MemoryReadStream(data, unpackedSize, DisposeAfterUse::YES);
}

} // End of namespace Sprint
