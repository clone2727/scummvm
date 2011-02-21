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

#ifndef JMP_BIT_H
#define JMP_BIT_H

#include "jmp/jmp.h"

namespace JMP {

// Cursor groups:
// 0x64 - Windows cursor (broken)
// 0x65 - interact
// 0x66 - examine
// 0x67 - grab
// 0x68 - grabbing
// 0x69 - back up
// 0x6a - turn page right
// 0x6b - turn page left
// 0x6c - look up on the page
// 0x6d - look down on the page
// 0x6e - searching for evidence
// 0x6f - found evidence
// 0x70 - move object up
// 0x71 - move object left
// 0x72 - move object down
// 0x73 - move object right

enum {
	kBITMainCursor = 100,
	kBITInteractCursor,
	kBITExamineCursor,
	kBITGrabCursor,
	kBITGrabbingCursor,
	kBITBackUpCursor,
	kBITTurnPageRightCursor,
	kBITTurnPageLeftCursor,
	kBITPageUpCursor,
	kBITPageDownCursor,
	kBITEvidenceSearchCursor,
	kBITEvidenceFoundCursor,
	kBITUpCursor,
	kBITLeftCursor,
	kBITDownCursor,
	kBITRightCursor
};

struct BIT_Variables {
	bool gWalkThru;            //Walkthrough or Adventure mode
	int gPosition;                /*Room/Looking position?*/
	bool gTranslate;            //Is the Translate biochip active?
	bool gCloaking;            //Are you cloaked?
	bool gUsingLensFilter;        //Is the lens filter attached?
	int gHints;                //How many hints have you used (Arthur)
	bool gCGMssnRead;        //Have you read the Chateau Galliard description?
	bool gMYMssnRead;        //Have you read the Chichen Itza description?
	bool gDSMssnRead;        //Have you read Da Vinci's Studio description?
	bool gAIMssnRead;            //Have you read Farnstein Lab description?
	int gSuitO2Time;            /*O2 Time*/
	int gSuitO2Warning;        /*O2 Warning*/
	bool gSeenAgt5Doll;        //Have you seen the message for you on the doll?
	//gCGTTVideo1
	//gCGMWVideo1 1
	//gCGMBVideo3 0
	//gCGMBVideo4 0
	//gBSKeyBuildState 54
	//gCGBSVid1 1
	//gCGKCVideo2 1
	bool gChestUnlocked;        //Have you unlocked the chest (from the portrait)?
	//gCGKCClue 1
	//gSeenSRAgt3 1
	bool gMYWGRope;            //Is the rope attached in Chichen Itza?
	//gMYTPCorrectCombo 1
	//gMYWTState 0
	int gSnk1State;            //What is the state of holder 1? (closed, open, closed with skull)
	int gSnk2State;            //What is the state of holder 2? (closed, open, closed with skull)
	int gSnk3State;            //What is the state of holder 3? (closed, open, closed with skull)
	int gSnk4State;            //What is the state of holder 4? (closed, open, closed with skull)
	//gSnk1Timer 0
	//gSnk2Timer 0
	//gSnk3Timer 0
	//gSnk4Timer 0
	//gDoor1Open 1
	//gDoor2Open 1
	//gSp1Going 0
	//gSp2Going 0
	bool gSkullFromSpearTaken;    //Is the Spear Room Skull taken/there?
	bool gSkullFromEntryTaken;    //Is the Entry Way Skull taken/there?
	bool gSkullfromCavernTaken;    //Is the Cavern Skull taken/there?
	//gDSCTDoor 2
	int gSeigeBuildState;        //Build state of the Seige Cycle
	int gMRFreqTried;            //Frequency of the resonance device on the sculpture
	bool gO2Made;			// Have you made the O2 after it was mined?
	bool gO2Mined;            //Have you mined O2?
	//gAmbassArrived 0

	char gTimeZone[8];            //Which Time Zone are you in?
	char gTimeZoneLoc[8];        //Which location in the time zone?
	char gActiveBiochip[8];        //Which biochip is selected?
	char gCanisterState[8];        //Is the canister full or empty?
	char gEnvironContains[8];        //What does the environ system contain?
	//gKUCodeEntered
	int gSmallWheelTime;        //What is the time on the small Mayan wheel?
	int gLargeWheelTime;        //What is the time on the large Mayan wheel?
	//gMYDGOffState 000
	char gSnk1Contains[8];        //What skull does holder 1 contain?
	char gSnk2Contains[8];        //What skull does holder 2 contain?
	char gSnk3Contains[8];        //What skull does holder 3 contain?
	char gSnk4Contains[8];        //What skull does holder 4 contain?
	char gDSPTLeftLever[8];        //Which position of the left lever in Da Vinci's studio?
	char gDSPTRightLever[8];        //Which position of the right lever in Da Vinci's studio?
	char gDSPTElevPos[8];        //Is the elevator up or down in Da Vinci's studio?
	//gDSCYBallState Fired
	char gCRPressure[8];        //Is the "blue" room pressurized?
	char gMRPressure[8];        //Is the sculpture room pressurized?
};
	
struct JMPGameDescription;

class JMPEngine_BIT : public JMPEngine {
public:
	JMPEngine_BIT(OSystem *syst, const JMPGameDescription *gamedesc);
	virtual ~JMPEngine_BIT() {}
	virtual Common::Error run();

protected:
	BIT_Variables _vars;

	bool useHighColor() { return true; }
	void waitUntilSoundEnds();

	void drawInterface();

private:
	bool _useHighColor;

	void runMainMenu();
	void checkDisc(int disc);
};
	
class JMPEngine_BITDemo : public JMPEngine_BIT {
public:
	JMPEngine_BITDemo(OSystem *syst, const JMPGameDescription *gamedesc) : JMPEngine_BIT(syst, gamedesc) {}
	virtual ~JMPEngine_BITDemo() {}
	Common::Error run();

private:
	void waitUntilMouseClick();
	void drawMiscBitmap(Common::String filename, uint16 x, uint16 y);
	void quitFromMainMenu();
};

class JMPEngine_BITTrailer : public JMPEngine_BIT {
public:
	JMPEngine_BITTrailer(OSystem *syst, const JMPGameDescription *gamedesc) : JMPEngine_BIT(syst, gamedesc) {}
	virtual ~JMPEngine_BITTrailer() {}
	Common::Error run();

private:
	const Common::String getEXEFileName() const;
	const Common::String getAVIFileName() const;
};

} //End of namespace JMP

#endif
