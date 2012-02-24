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

#include "audio/mixer.h" // TODO: Move this to psx_decoder.h...
#include "common/archive.h"
#include "common/array.h"
#include "common/substream.h"
#include "common/tokenizer.h"
#include "video/psx_decoder.h"

#include "made/psx_inf.h"

namespace Made {

struct FileListEntry {
	uint startSector;
	uint frameCount;
	bool highSpeed;
	Common::String fileName;
};

#define RAW_CD_SECTOR_SIZE 2352

PSXStreamINF::PSXStreamINF(const Common::String &baseFileName, bool audio) {
	Common::String infFileName = baseFileName + ".inf";
	_streamFileName = baseFileName + (audio ? ".xai" : ".stl");

	// Read in all lines from the file
	Common::File infFile;
	if (!infFile.open(infFileName))
		error("Could not open '%s'", infFileName.c_str());

	// Now open the stream file
	Common::File file;
	if (!file.open(_streamFileName))
		error("Could not open '%s'", _streamFileName.c_str());

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
		fileList[i].highSpeed = atoi(tokens.nextToken().c_str()) == 1;
		fileList[i].fileName = tokens.nextToken();
	}

	fileStrList.clear();

	// Fill in our map using that data
	for (uint32 i = 0; i < fileList.size(); i++) {
		FileEntry entry;
		entry.offset = fileList[i].startSector * RAW_CD_SECTOR_SIZE;
		entry.size = ((i == fileList.size() - 1) ? file.size() : fileList[i + 1].startSector * RAW_CD_SECTOR_SIZE) - entry.offset;
		entry.highSpeed = fileList[i].highSpeed;
		_map[fileList[i].fileName] = entry;
	}
}

PSXStreamINF::~PSXStreamINF() {
}

bool PSXStreamINF::hasFile(const Common::String &name) const {
	return _map.contains(name);
}

Video::VideoDecoder *PSXStreamINF::createVideoDecoderForMember(const Common::String &name) const {
	if (!_map.contains(name))
		return 0;

	const FileEntry &entry = _map[name];

	Common::SeekableReadStream *stream = SearchMan.createReadStreamForMember(_streamFileName);

	if (!stream)
		return 0;

	Common::SeekableReadStream *subStream = new Common::SeekableSubReadStream(stream, entry.offset, entry.offset + entry.size, DisposeAfterUse::YES);

	Video::VideoDecoder *video = new Video::PSXStreamDecoder(entry.highSpeed ? Video::PSXStreamDecoder::kCD2x : Video::PSXStreamDecoder::kCD1x);

	if (!video->loadStream(subStream)) {
		delete video;
		return 0;
	}

	return video;
}

} // End of namespace Made
