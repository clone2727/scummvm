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

#include "common/error.h"
#include "common/events.h"
#include "common/rect.h"

#include "jmp/journey.h"

namespace JMP {

// Buttons	
enum {
	TheDream = 0,
	TheKickoff = 1,
	Expanding = 2,
	TheScript = 3,
	ConceptDesign = 4,
	Modeling = 5,
	AnimTextures = 6,
	Audio = 7,
	Programming = 8,
	PRM = 9,
	TheFuture = 10,
	PlayAll = 11,
	MaxVideoButtons = 12,
		
	NoVideoButtonSelected = -1
};
	
static const Common::Rect videoButtons[] = {
	Common::Rect(28, 58, 283, 96),
	Common::Rect(28, 96, 283, 134),
	Common::Rect(28, 134, 283, 170),
	Common::Rect(28, 170, 283, 206),
	Common::Rect(28, 206, 283, 244),
	Common::Rect(28, 244, 283, 280),
	Common::Rect(28, 280, 283, 318),
	Common::Rect(28, 318, 283, 356),
	Common::Rect(28, 356, 283, 394),
	Common::Rect(28, 394, 283, 430),
	Common::Rect(28, 430, 283, 470),
	Common::Rect(390, 299, 528, 386)
};

Common::Error JMPEngine_Journey::run() {
	init();

	_gfx->drawBitmap(Common::String("rawmenu.bmp"), 0, 0);
	// TODO: Play intro video
	_sound->playSound(Common::String("j2loop.wav"), true);
	Common::Event event;
	int buttonSelected = NoVideoButtonSelected;
	
	while (!shouldQuit()) {
		while (_eventMan->pollEvent(event)) {
			switch (event.type) {
				case Common::EVENT_MOUSEMOVE:
					if (buttonSelected != NoVideoButtonSelected)
						if (!videoButtons[buttonSelected].contains(event.mouse)) {
							_gfx->drawBitmap(Common::String("rawmenu.bmp"), videoButtons[buttonSelected], videoButtons[buttonSelected].left, videoButtons[buttonSelected].top, 0);
							buttonSelected = NoVideoButtonSelected;
						}
					_system->updateScreen();
					break;
				case Common::EVENT_LBUTTONDOWN:
					for (int i = 0; i < MaxVideoButtons; i++)
						if (videoButtons[i].contains(event.mouse))
								buttonSelected = i;
		
					if (buttonSelected != NoVideoButtonSelected) {
						_gfx->drawBitmap(Common::String("highligt.bmp"), videoButtons[buttonSelected], videoButtons[buttonSelected].left, videoButtons[buttonSelected].top);
						// TODO: Play requested video
						// TODO: Some sort of play position bar interactivity too
					}
					break;
				case Common::EVENT_LBUTTONUP:
					if (buttonSelected != NoVideoButtonSelected) {
						_gfx->drawBitmap(Common::String("rawmenu.bmp"), videoButtons[buttonSelected], videoButtons[buttonSelected].left, videoButtons[buttonSelected].top);
						buttonSelected = NoVideoButtonSelected;
					}
					break;
				default:
					break;
			}
		}
	}
	
	return Common::kNoError;
}

} // End of namespace JMP
