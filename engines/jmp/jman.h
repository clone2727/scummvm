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

#ifndef JMP_JMAN_H
#define JMP_JMAN_H

#include "jmp/jmp.h"

#include "common/str.h"

namespace JMP {

struct JMAN_Variables {		
	Common::String gComingFrom;
	bool gCalledFlFourElev;
	int gEnergyLossRate;
	bool gCommandCenterBlink;
	bool gInitialRip;
	bool gReadyRoomBlink;
	bool gTSAElevatorBlink;
	bool gMoleculeSynthesized;
	bool gMoleculeAnalyzed;
	bool gNoradLaunchDeactivated;
	bool gUseRetinalChipNow;
	bool gHistoricalLogInCD;
	uint gVolume;
	uint gTransScore;
	uint gBkgndMonScore;
	uint gTheoryMonScore;
	uint gProcMonScore;
	uint gBodySuitScore;
	uint gPegChipScore;
	uint gMapChipScore;
	uint gTraceChipScore;
	uint gOptChipScore;
	uint gRetChipScore;
	uint gShieldChipScore;
	uint gPrehistoricScore;
	uint gMarsAttempts;
	int gMarsTimeLeft;
	bool gMarsPeaceBonus;
	uint gMarsScore;
	bool gMarsComplete;
	uint gWSCAttempts;
	int gWSCTimeLeft;
	bool gWSCPeaceBonus;
	uint gWSCScore;
	bool gWSCComplete;
	uint gNoradAttempts;
	int gNoradTimeLeft;
	bool gNoradPeaceBonus;
	uint gNoradScore;
	bool gNoradComplete;
	bool gAresMemoryChip;
	bool gMercuryMemoryChip;
	bool gPoseidonMemoryChip;
	bool gStartTimeInMaze;
	int gOxygenStatus;
	bool gUsingOxygenMask;
	Common::String gMarsRobotLoc;
	uint gRobotDownTime;
	bool gPodAtUpperHall;
	Common::String gAlertLevel;
	Common::String gObjectToUse;
	Common::String gObjectPos;
	Common::String gCompassSpin;
	bool gInspecting;
	bool gMessageUp;
	bool gChipsOpen;
	Common::String gDoorCloseSound;
	Common::String gCurrentBioChip;
	Common::String gDoorOpenN;
	Common::String gDoorOpenE;
	Common::String gDoorOpenS;
	Common::String gDoorOpenW;
	byte gHowManyChips;
	uint gscrolloflist;

	uint32 gEnergy;
	uint32 gLocale;
	uint32 gDirection;
};

struct JMPGameDescription;

class JMPEngine_JMAN : public JMPEngine {
public:
	JMPEngine_JMAN(OSystem *syst, const JMPGameDescription *gamedesc);
	~JMPEngine_JMAN();

	Common::Error run();

private:
	JMAN_Variables _vars;

	//AVIPlayer *_animVideo;
	//AVIPlayer *_navVideo;
		
	// Main Menu/Intro
	int runMainMenu();
	void runSepiaTone();
	void runDreamSequence();
	void runTutorial();
	void runCredits();

	// Initialization/Main Screen
	void initVars();
	void redrawScreen();
	void drawDate();
};

} // End of namespace JMP

#endif
