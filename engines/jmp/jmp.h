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

#ifndef JMP_H
#define JMP_H

#include "common/array.h"
#include "common/scummsys.h"
#include "common/util.h"
#include "common/str-array.h"
#include "common/system.h"
#include "common/rect.h"
#include "common/winexe_ne.h"

#include "engines/engine.h"

#include "jmp/graphics.h"
#include "jmp/sound.h"

namespace Video {
	class VideoDecoder;
}

namespace JMP {

enum JMPGameType {
	GType_JMAN = 1,
	GType_BIT = 2,
	GType_JOURNEY = 3
};

enum JMPGameFeatures {
	GF_DEMO =    (1 << 0),
	GF_TRAILER = (1 << 1)
};

struct JMPGameDescription;
class Sound;
class GraphicsManager;

class JMPEngine : public ::Engine {
	int _gameId;

protected:
	virtual Common::Error run() = 0;
	
	void init();

public:
	JMPEngine(OSystem *syst, const JMPGameDescription *gamedesc);
	virtual ~JMPEngine();

	// Detection related functions
	const JMPGameDescription *_gameDescription;
	uint32 getFeatures() const;
	uint16 getVersion() const;
	Common::Platform getPlatform() const;
	uint8 getGameType();
	Common::Language getLanguage();
	bool hasFeature(EngineFeature f) const;

	// Various pointers to various classes
	GraphicsManager *_gfx;
	Sound *_sound;

	// EXE parsing
	Common::SeekableReadStream *getEXEResource(uint16 type, Common::WinResourceID id);
	Common::Array<Common::NEResources*> _exeFiles;
	Common::StringArray getStringResource(Common::WinResourceID id);

protected:
	void playVideo(Common::String filename, uint16 x = 0, uint16 y = 0);
	void playVideoCentered(Common::String filename);

	void loadMandatoryEXE(Common::String filename);

private:
	void playVideo(Video::VideoDecoder *video, uint16 x, uint16 y);
};

} // End of namespace JMP

#endif
