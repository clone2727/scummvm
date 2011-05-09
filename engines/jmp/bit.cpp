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
 
#include "jmp/bit.h"

#include "common/archive.h"
#include "common/config-manager.h"
#include "common/error.h"
#include "common/events.h"
#include "common/file.h"
#include "common/fs.h"
#include "common/rect.h"
#include "common/str.h"
#include "common/textconsole.h"
#include "graphics/cursorman.h"

namespace JMP {

// GOG Version: PE executable? External DLL still? WTF?!?!?!? SOMEONE TELL ME THE DETAILS!

// Unknown EXE Resources:
//
// Type   | Range
// -------------------------
// 0xd8c  | [0x4011, 0x40a1)
// 0xd94  | [0x4111, 0x4111)
// 0xd9c  | [0x4211, 0x4272]
// 0xda5  | [0x4311, 0x436a]
// 0xdac  | [0x1000, 0x1000)
// 0xdb5  | [0x0382, 0x0387]
// 0xdbe  | [0x0386, 0x0386)
// 0xdc9  | [0x189d, 0x189e]

// Known EXE Resources:
//
// Type   | Range            | Meaning
// -----------------------------------
// 0xdd1  | 0x03b6           | Alert WAVE File

// DLL Resources:
//
// Type   | Range            | Meaning
// -----------------------------------
// Bitmap | [0x3000, 0x3005] | Interface
// Bitmap | 0x3006           | BioChip Examine Screen
// Bitmap | 0x3007           | Interface Text Area
// Bitmap | [0x3008, 0x3017] | Interface Arrows
// Bitmap | 0x301b           | Alert Light
// Bitmap | [0x3020, 0x3025] | Interface Dates
// Bitmap | [0x303c, 0x3043] | Main Menu
// Bitmap | [0x3048, 0x304a] | Change Disc Images
// Bitmap | [0x3070, 0x3076] | Death Screen
// Bitmap | [0x307a, 0x307c] | Interface BioChip
// Bitmap | [0x307d, 0x307f] | Death Screen Buttons *Duplicate*
// Bitmap | [0x30a2, 0x30b9] | BioChip Menu Items
// Bitmap | [0x3200, 0x322d] | Inventory Drawing
// Bitmap | [0x3264, 0x32a2] | Dragging Inventory Drawing

JMPEngine_BIT::JMPEngine_BIT(OSystem *syst, const JMPGameDescription *gamedesc) : JMPEngine(syst, gamedesc) {
	_useHighColor = true;
}
	
Common::Error JMPEngine_BIT::run() {
	init();

	// The EXE files hold static tables, strings, cursors, and the alert sound
	// The DLL files hold bitmaps they decided not to put externally
	if (useHighColor()) {
		loadMandatoryEXE("BIT2416.EXE");
		loadMandatoryEXE("BIT24LIB.DLL");
	} else {
		loadMandatoryEXE("BIT816.EXE");
		loadMandatoryEXE("BIT8LIB.DLL");
	}

#if 0
	// Double Alert!
	_sound->playSound(getEXEResource(0xdd1, 0x3b6));
	waitUntilSoundEnds();
	_system->delayMillis(100);
	_sound->playSound(getEXEResource(0xdd1, 0x3b6));
	waitUntilSoundEnds();
#endif

	const Common::FSNode gameDataDir(ConfMan.get("path"));
	SearchMan.addSubDirectoryMatching(gameDataDir, "bitdata");
	
	// Run the title screens.
	//_gfx->setCursor(kBITMainCursor);
	CursorMan.showMouse(false);
	playVideoCentered("bitdata/title/swlogo.btv");
	playVideoCentered("bitdata/title/presto.btv");
	if (useHighColor())
		_gfx->drawBitmap("bitdata/title/bkg.btb", 0, 0);
	else
		_gfx->drawBitmap("bitdata/title/bkg8.btb", 0, 0);
	playVideo("bitdata/title/title.btv", 60, 138); // Line the video up with the background

	runMainMenu();
		
	return Common::kNoError;
}

void JMPEngine_BIT::runMainMenu() {
	CursorMan.showMouse(true);
	_gfx->drawEXEBitmap(0x303c);
	_system->updateScreen();
	_sound->playSound("bitdata/common/mainmenu.bta", true);

	// 0x303C - Main Menu Background
	// 0x303D - Interface Overview Pressed
	// 0x303E - New Game Pressed
	// 0x303F - Walkthrough Selected
	// 0x3040 - Skip Intro Selected
	// 0x3041 - Restore Game Pressed
	// 0x3042 - Credits Pressed
	// 0x3043 - Quit Pressed

	while (!shouldQuit()) {
		Common::Event event;
		while (_eventMan->pollEvent(event)) {
			switch (event.type) {
			case Common::EVENT_MOUSEMOVE:
				_system->updateScreen();
				break;
			default:
				break;
			}
		}

		_system->delayMillis(10);
	}
}

void JMPEngine_BIT::drawInterface() {
	_gfx->drawEXEBitmap(0x3000, 0, 0);
	_gfx->drawEXEBitmap(0x3001, 0, 128);
	_gfx->drawEXEBitmap(0x3002, 640-144, 128);
	_gfx->drawEXEBitmap(0x3003, 0, 128+189);
	// 0x3004 is just a copy of the inventory section from 0x3003
	// TODO: 0x3005 is the up/examine/down buttons of the inventory section
	// TODO: Draw current inventory item selected [0x3200, 0x322d]
	// TODO: Draw current selected BioChip [0x30a2, 0x30b9]
	// TODO: Draw arrows
	// TODO: Draw date
	_system->updateScreen();
}

void JMPEngine_BIT::waitUntilSoundEnds() {
	Common::Event event;
	while (_sound->isPlaying() && !shouldQuit()) {
		while (_eventMan->pollEvent(event))
			;

		_system->delayMillis(10);
	}
}

void JMPEngine_BIT::checkDisc(int disc) {
	// TODO: More on this
	while (!shouldQuit()) {
		if (Common::File::exists(Common::String::format("BIT_DISC.%d", disc)))
			break;

		_gfx->drawEXEBitmap(0x3047 + disc, 155, 182);

		// TODO: Probably should draw some sort of OK button
		_system->updateScreen();
		error("You need to have disc %d in", disc);
	}
}

} // End of namespace JMP
