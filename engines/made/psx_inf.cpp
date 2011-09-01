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

#include "common/array.h"
#include "common/tokenizer.h"

#include "made/psx_inf.h"

namespace Made {

struct FileListEntry {
	uint startSector;
	uint frameCount;
	uint length;
	Common::String fileName;
};

#define RAW_CD_SECTOR_SIZE 2352

PSXStreamINF::PSXStreamINF(const Common::String &baseFileName, bool audio) : Common::Archive() {
	Common::String infFileName = baseFileName + ".inf";
	Common::String streamFileName = baseFileName + (audio ? ".xai" : ".stl");

	// Read in all lines from the file
	Common::File infFile;
	if (!infFile.open(infFileName))
		error("Could not open '%s'", infFileName.c_str());

	// Now open the stream file
	if (!_file.open(streamFileName))
		error("Could not open '%s'", streamFileName.c_str());

	Common::Array<Common::String> fileStrList;
	while (infFile.pos() < infFile.size())
		fileStrList.push_back(infFile.readLine());

	infFile.close();

	// Parse the strings into something we can use
	Common::Array<FileListEntry> fileList;
	fileList.resize(fileStrList.size());

	for (uint32 i = 0; i < fileStrList.size(); i++) {
		Common::StringTokenizer tokens(fileStrList[i]);

		fileList[i].startSector = (uint)atoi(tokens.nextToken().c_str());
		fileList[i].frameCount = (uint)atoi(tokens.nextToken().c_str());
		fileList[i].length = (uint)atoi(tokens.nextToken().c_str());
		fileList[i].fileName = tokens.nextToken();
	}

	fileStrList.clear();

	// Fill in our map using that data
	for (uint32 i = 0; i < fileList.size(); i++) {
		FileEntry entry;
		entry.offset = fileList[i].startSector * RAW_CD_SECTOR_SIZE;
		entry.size = ((i == fileList.size() - 1) ? _file.size() : fileList[i + 1].startSector * RAW_CD_SECTOR_SIZE) - entry.offset;
		_map[fileList[i].fileName] = entry;
	}
}

PSXStreamINF::~PSXStreamINF() {
}

bool PSXStreamINF::hasFile(const Common::String &name) {
	return _map.contains(name);
}

int PSXStreamINF::listMembers(Common::ArchiveMemberList &list) {
	for (FileMap::const_iterator it = _map.begin(); it != _map.end(); it++)
		list.push_back(getMember(it->_key));

	return _map.size();
}

Common::ArchiveMemberPtr PSXStreamINF::getMember(const Common::String &name) {
	return Common::ArchiveMemberPtr(new Common::GenericArchiveMember(name, this));
}

Common::SeekableReadStream *PSXStreamINF::createReadStreamForMember(const Common::String &name) const {
	if (!_map.contains(name))
		return 0;

	const FileEntry &entry = _map[name];

	_file.seek(entry.offset);

	// TODO: Maybe think about using a SeekableSubReadStream
	return _file.readStream(entry.size);
}

} // End of namespace Made
