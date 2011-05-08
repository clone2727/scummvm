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
#include "common/textconsole.h"

namespace JMP {

/////////////////////////////////////////////////////
// Construction/Destruction! :P
/////////////////////////////////////////////////////

JMPEngine_JMAN::JMPEngine_JMAN(OSystem *syst, const JMPGameDescription *gamedesc) : JMPEngine(syst, gamedesc) {
}

JMPEngine_JMAN::~JMPEngine_JMAN() {
}


/////////////////////////////////////////////////////
// Miscellaneous Defines
/////////////////////////////////////////////////////

enum {
	kNoButtonSelected = -1
};

/////////////////////////////////////////////////////
// Main Menu Functions
/////////////////////////////////////////////////////

// Main Menu Buttons
enum {
	kOverviewButton = 0,
	kNewGameButton = 1,
	kNewGameSkipBkgButton = 2,
	kNewGameSkipIntroButton = 3,
	kCreditsButton = 4,
	kLoadGameButton = 5,
	kQuitGameButton = 6,
	kMaxMainMenuButtons = 7
};
	
int JMPEngine_JMAN::runMainMenu() {
	static const Common::Rect mainMenuButtons[] = {
		Common::Rect(402, 64,  612, 116),
		Common::Rect(402, 128, 612, 176),
		Common::Rect(402, 182, 504, 230),
		Common::Rect(510, 182, 612, 230),
		Common::Rect(402, 245, 612, 293),
		Common::Rect(402, 299, 504, 347),
		Common::Rect(510, 299, 612, 347)
	};
	
	static const Common::Rect mainMenuTextBox = Common::Rect(407, 386, 608, 415);
	
	static const char *jmanEnglishMenuDescriptions[] = {
		"Show me how to use the Journeyman's interface.",
		"Start the game from the very beginning.",
		"Start the game at the introductory animation.",
		"Start from after the background and introductory animations.",
		"Run the credits.",
		"Restart a previously saved game.",
		"Quit, so I can optimize my system for best performance."
	};

	// This is all very hacky
	// The music sounds nice, of course

	_gfx->drawBitmap("support/mainmnu1.bmp", 0, 0);
	_sound->playSound("support/shared/caltheme.wav");
	Common::Event event;
	int buttonSelected = kNoButtonSelected;
	int textSelected = kNoButtonSelected;
	
	while (!shouldQuit()) {
		while (_eventMan->pollEvent(event)) {
			switch (event.type) {
				case Common::EVENT_MOUSEMOVE:
					if (textSelected == kNoButtonSelected) {
						for (int i = 0; i < kMaxMainMenuButtons; i++)
							if (mainMenuButtons[i].contains(event.mouse))
								textSelected = i;
						if (textSelected != kNoButtonSelected)
							_gfx->drawString(jmanEnglishMenuDescriptions[textSelected], mainMenuTextBox, _system->getScreenFormat().RGBToColor(0, 0, 0));
					} else {
						if (!mainMenuButtons[textSelected].contains(event.mouse)) {
							_gfx->drawBitmap("support/mainmnu1.bmp", mainMenuTextBox, mainMenuTextBox.left, mainMenuTextBox.top, false);
							textSelected = kNoButtonSelected;
						}
					}
					if (buttonSelected != kNoButtonSelected)
						if (!mainMenuButtons[buttonSelected].contains(event.mouse)) {
							_gfx->drawBitmap(Common::String("support/mainmnu1.bmp"), mainMenuButtons[buttonSelected], mainMenuButtons[buttonSelected].left, mainMenuButtons[buttonSelected].top, false);
							buttonSelected = kNoButtonSelected;
						}
					_system->updateScreen();
					break;
				case Common::EVENT_LBUTTONDOWN:
					for (int i = 0; i < kMaxMainMenuButtons; i++)
						if (mainMenuButtons[i].contains(event.mouse))
								buttonSelected = i;
					if (buttonSelected != kNoButtonSelected)
						_gfx->drawBitmap(Common::String("support/mainmnu2.bmp"), mainMenuButtons[buttonSelected], mainMenuButtons[buttonSelected].left, mainMenuButtons[buttonSelected].top);
					break;
				case Common::EVENT_LBUTTONUP:
					if (buttonSelected != kNoButtonSelected) {
						_gfx->drawBitmap(Common::String("support/mainmnu1.bmp"), mainMenuButtons[buttonSelected], mainMenuButtons[buttonSelected].left, mainMenuButtons[buttonSelected].top);
						switch (buttonSelected) {
							//case kNewGameButton:
							//case kNewGameSkipBkgButton:
							//case kNewGameSkipIntroButton:
							case kQuitGameButton:
								return buttonSelected;
							//case kOverviewButton:
							//	runTutorial();
							//	break;
							case kCreditsButton:
								_sound->stopSound();
								runCredits();
								if (!shouldQuit()) {
									_system->fillScreen(0);
									_gfx->drawBitmap(Common::String("support/mainmnu1.bmp"), 0, 0);
								}
								break;
							default:
								warning ("No function here yet");
								break;
						}
						buttonSelected = kNoButtonSelected;
					}
					break;
				default:
					break;
			}
		}
	}
		
	return kQuitGameButton;
}
	
/////////////////////////////////////////////////////
// Interface Functions
/////////////////////////////////////////////////////
	
void JMPEngine_JMAN::redrawScreen() {
	_gfx->drawBitmap("support/fullscrn.bmp", 0, 0);
	// drawCompass();
	drawDate();
	// drawInventory();
}

void JMPEngine_JMAN::drawDate() {
#if 0
	static const Common::Rect dateRects[] = {
		Common::Rect(0, 1 , 65, 11),
		Common::Rect(0, 13, 65, 23),
		Common::Rect(0, 25, 65, 35),
		Common::Rect(0, 37, 65, 47),
		Common::Rect(0, 49, 65, 59)
	};

	static const Common::Point dateLocation = Common::Point(473, 64);
#endif

	// TODO
}

/////////////////////////////////////////////////////
// Intro Functions
/////////////////////////////////////////////////////

void JMPEngine_JMAN::runDreamSequence() {
	_sound->playSound("support/dream/intro.wav");
	// TODO: Play frames from dream/dream.avi and dream/wake.avi at the correct timing.
}

void JMPEngine_JMAN::runSepiaTone() {
	_sound->playSound("support/sepiaton/sepiaton.wav");
	// TODO: Play frames from sepiaton/sepiaton.avi at the correct timing.
}

void JMPEngine_JMAN::runTutorial() {
	// TODO: play frames from tutorial/tutor.avi at the correct timing with the sounds in that folder.
}

/////////////////////////////////////////////////////
// Credits Functions
/////////////////////////////////////////////////////

enum {
	kCreditsReturnButton = 9,
	kCreditsText = 10,
	kCreditsButtonCount = 11
};

void JMPEngine_JMAN::runCredits() {
	static const Common::Rect creditsButtons[] = {
		Common::Rect(290, 9, 401, 163),
		Common::Rect(402, 9, 513, 163),
		Common::Rect(514, 9, 625, 163),
		Common::Rect(290, 164, 401, 316),
		Common::Rect(402, 164, 513, 316),
		Common::Rect(514, 164, 625, 316),
		Common::Rect(290, 317, 401, 471),
		Common::Rect(402, 317, 513, 471),
		Common::Rect(514, 317, 625, 471),
		Common::Rect(189, 433, 262, 461),
		Common::Rect(27, 55, 262, 399)
	};

	// This also is very hacky
	// And the swipes don't happen correctly
	// etc, etc, etc

	int buttonSelected = kNoButtonSelected;
	byte textState = 0;
	Common::Event event;
	
	bool facesClicked[9];
	for (byte i = 0; i < 9; i++)
		facesClicked[i] = false;
	
	// Get the main background up
	_gfx->drawBitmap("support/credits/crdscrn1.bmp", 0, 0);
	
	while (!shouldQuit()) {
		while (_eventMan->pollEvent(event)) {
			switch (event.type) {
				case Common::EVENT_MOUSEMOVE:
					if (buttonSelected == kCreditsReturnButton && !creditsButtons[buttonSelected].contains(event.mouse)) {
						_gfx->drawBitmap("support/credits/crdscrn1.bmp", creditsButtons[buttonSelected], creditsButtons[buttonSelected].left, creditsButtons[buttonSelected].top, 0);
						buttonSelected = kNoButtonSelected;
					}
					_system->updateScreen();
					break;
				case Common::EVENT_LBUTTONDOWN:
					for (int i = 0; i < kCreditsButtonCount; i++)
						if (creditsButtons[i].contains(event.mouse))
								buttonSelected = i;
					if (buttonSelected == kCreditsReturnButton) {
						_gfx->drawBitmap("support/credits/crdscrn2.bmp", creditsButtons[buttonSelected], creditsButtons[buttonSelected].left, creditsButtons[buttonSelected].top, 0);
						_system->updateScreen();
					}
					break;
				case Common::EVENT_LBUTTONUP:
					if (buttonSelected != kNoButtonSelected) {
						if (buttonSelected == kCreditsText) {
							textState++;
							if (textState == 3) textState = 0;
							switch (textState) {
								case 0:
									_gfx->drawBitmap("support/credits/crdscrn1.bmp", creditsButtons[buttonSelected], creditsButtons[buttonSelected].left, creditsButtons[buttonSelected].top, 0);
									break;
								case 1:
									_gfx->drawBitmap("support/credits/crdscrn2.bmp", creditsButtons[buttonSelected], creditsButtons[buttonSelected].left, creditsButtons[buttonSelected].top, 0);
									break;
								case 2:
									_gfx->drawBitmap("support/credits/crdscrn3.bmp", creditsButtons[buttonSelected], creditsButtons[buttonSelected].left, creditsButtons[buttonSelected].top, 0);
									break;
							}
						} else if (buttonSelected == kCreditsReturnButton) {
							return;
						} else {
							facesClicked[buttonSelected] = !facesClicked[buttonSelected];
							if (facesClicked[buttonSelected]) {
								_gfx->drawBitmap("support/credits/crdscrn2.bmp", creditsButtons[buttonSelected], creditsButtons[buttonSelected].left, creditsButtons[buttonSelected].top, 0);
								_sound->playSound("support/credits/crd_open.wav");
							} else {
								_gfx->drawBitmap("support/credits/crdscrn1.bmp", creditsButtons[buttonSelected], creditsButtons[buttonSelected].left, creditsButtons[buttonSelected].top, 0);
								_sound->playSound("support/credits/crd_clos.wav");
							}
						}
					}
					buttonSelected = kNoButtonSelected;
					break;
				default:
					break;
			}
		}
	}
}

/////////////////////////////////////////////////////
// Engine Functions
/////////////////////////////////////////////////////

void JMPEngine_JMAN::initVars() {
	// TODO
}

Common::Error JMPEngine_JMAN::run() {
	// Initialization:
	init();
	//initVars();

	// Load our EXE
	loadMandatoryEXE("JMAN.EXE");
	
	// Opening Videos:
	playVideoCentered("support/slogo.avi");
	playVideoCentered("support/presto.avi");
	_system->fillScreen(_system->getScreenFormat().RGBToColor(0, 0, 0));
	_system->updateScreen();
	// show present.bmp
	playVideoCentered("support/jmanspin.avi");
	
	// Main Menu/Introduction:
	int menuResult = runMainMenu();
	if (menuResult == kQuitGameButton)
		return Common::kNoError;
		
	if (menuResult == kNewGameButton)
		runSepiaTone();
		
	if (shouldQuit())
		return Common::kNoError;
		
	if (menuResult == kNewGameButton || menuResult == kNewGameSkipBkgButton)
		runDreamSequence();
		
	// Game Main Loop:
	// TODO: Actually do something
	return Common::kNoError;
}
	
} // End of namespace JMP
