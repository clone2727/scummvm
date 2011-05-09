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

#include "common/error.h"
#include "common/events.h"
#include "common/rect.h"
#include "common/str.h"
#include "common/textconsole.h"
#include "graphics/cursorman.h"

namespace JMP {

enum {
	kDemoMainMenuQuit = 4,
	kDemoMaxMainMenuButtons = 5,
	
	kDemoNoButtonSelected = -1
};

Common::Error JMPEngine_BITDemo::run() {
	init();

	if (useHighColor())
		loadMandatoryEXE("BIT2416.EXE");
	else
		loadMandatoryEXE("BIT816.EXE");

	static const Common::Rect demoMainMenuButtons[] = {
		Common::Rect( 28, 155, 168, 326),
		Common::Rect(177, 155, 315, 326),
		Common::Rect(324, 155, 462, 326),
		Common::Rect(471, 155, 609, 326),
		Common::Rect(551, 439, 640, 480)
	};
	
	// Hide the cursor for the opening.
	CursorMan.showMouse(false);
	
	// Unlike the full game, the Sanctuary Woods logo is divided into an image and a sound.
	drawMiscBitmap("swlogo.bmp", 195, 115);
	_sound->playSound("misc/swsting.wav");
	waitUntilSoundEnds();
	
	if (shouldQuit())
		return Common::kNoError;
	
	// Next, play the Presto Studios logo.
	_system->fillScreen(_system->getScreenFormat().RGBToColor(0, 0, 0));
	playVideoCentered("misc/presto.avi");
	
	if (shouldQuit())
		return Common::kNoError;
	
	// Now draw the splash screen and then start the music and wait 5 seconds before showing the menu.
	drawMiscBitmap("splash.bmp", 0, 0);
	_sound->playSound("misc/menuloop.wav", true);
		
	uint32 curMillis = _system->getMillis();
	Common::Event event;
	while (_system->getMillis() < curMillis + 5000 && !shouldQuit()) {
		while (_eventMan->pollEvent(event)) {
			// Do nothing :P
		}
	}
	
	if (shouldQuit())
		return Common::kNoError;
		
	// Show the menu.
	drawMiscBitmap("mainmenu.bmp", 0, 0);
	CursorMan.showMouse(true);
	int buttonSelected = kDemoNoButtonSelected;
		
	while (!shouldQuit()) {
		while (_eventMan->pollEvent(event)) {
			switch (event.type) {
				case Common::EVENT_MOUSEMOVE:
					if (buttonSelected != kDemoNoButtonSelected && !demoMainMenuButtons[buttonSelected].contains(event.mouse))
						buttonSelected = kDemoNoButtonSelected;								
					_system->updateScreen();
					break;
				case Common::EVENT_LBUTTONDOWN:
					for (int i = 0; i < kDemoMaxMainMenuButtons; i++)
						if (demoMainMenuButtons[i].contains(event.mouse))
							buttonSelected = i;
					break;
				case Common::EVENT_LBUTTONUP:
					if (buttonSelected == kDemoNoButtonSelected)
						break;
					switch (buttonSelected) {
						case kDemoMainMenuQuit:
							quitFromMainMenu();
							return Common::kNoError;
						default:
							warning("Unhandled BIT Demo main menu button");
					}
					break;
				default:
					break;
			}
		}
	}
		
	return Common::kNoError;
}
	
void JMPEngine_BITDemo::quitFromMainMenu() {
	drawMiscBitmap("feature1.bmp", 0, 0);
	waitUntilMouseClick();
	if (shouldQuit())
		return;
		
	drawMiscBitmap("feature2.bmp", 0, 0);
	waitUntilMouseClick();
	if (shouldQuit())
		return;
		
	drawMiscBitmap("feature3.bmp", 0, 0);
	waitUntilMouseClick();
	if (shouldQuit())
		return;
		
	drawMiscBitmap("closing.bmp", 0, 0);
	waitUntilMouseClick();
	if (shouldQuit())
		return;
}
	
void JMPEngine_BITDemo::drawMiscBitmap(Common::String filename, uint16 x, uint16 y) {
	// Create filename
	Common::String newFilename = "misc/";
	// Check if we're going to use 8bpp/24bpp
	newFilename += (useHighColor() ? "24bpp" : "8bpp");
	// Add the slash
	newFilename += "/";
	// Add the actual filename
	newFilename += filename;
	// Draw the image
	_gfx->drawBitmap(newFilename, x, y);
}
	
void JMPEngine_BITDemo::waitUntilMouseClick() {
	Common::Event event;
	while (!shouldQuit()) {
		while (_eventMan->pollEvent(event)) {
			switch (event.type) {
				case Common::EVENT_MOUSEMOVE:
					_system->updateScreen();
					break;
				case Common::EVENT_LBUTTONUP:
					return;
				default:
					break;
			}
		}

		_system->delayMillis(10);
	}
}

} // End of namespace JMP
