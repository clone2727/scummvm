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

#include "jmp/jman.h"

#include "common/error.h"
#include "common/events.h"
#include "common/rect.h"
#include "common/stream.h"
#include "common/textconsole.h"
#include "graphics/cursorman.h"

namespace JMP {

JMPEngine_JMANTrailer::JMPEngine_JMANTrailer(OSystem *syst, const JMPGameDescription *gamedesc) : JMPEngine(syst, gamedesc) {
}

JMPEngine_JMANTrailer::~JMPEngine_JMANTrailer() {
}

Common::Error JMPEngine_JMANTrailer::run() {
	// Initialization:
	init();

	// Show the video portion
	CursorMan.showMouse(false);
	_gfx->drawBitmap("UI.BMP", 0, 0);
	playVideo("JM1DEMO.AVI", 128, 90);

	// Draw the ending images
	runFinalImage(1, 6000);
	runFinalImage(2, 6000);
	runFinalImage(3, 2000);
	runFinalImage(4, 6000);
	runFinalImage(5, 4000);
	runFinalImage(6, 6000);

	return Common::kNoError;
}

void JMPEngine_JMANTrailer::runFinalImage(int index, uint32 delay) {
	if (shouldQuit())
		return;

	_gfx->drawBitmap(Common::String::format("FINAL%d.BMP", index), 0, 0);

	uint32 startTime = _system->getMillis();

	while (_system->getMillis() < startTime + delay && !shouldQuit()) {
		Common::Event event;
		while (_eventMan->pollEvent(event))
			;

		_system->delayMillis(10);
	}
}
	
} // End of namespace JMP
