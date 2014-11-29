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

#ifndef PRINCE_FLAGS_H
#define PRINCE_FLAGS_H

#include "common/scummsys.h"

namespace Prince {

class Flags {
public:
	static int compareFlagDebug(const void *a, const void *b);
	static const char *getFlagName(uint16 flagId);

	enum Id {
		FLAGA1			=	0x8000,
		FLAGA2			=	0x8002,
		FLAGA3			=	0x8004,
		DESTX			=	0x8006,
		DESTY			=	0x8008,
		DESTD			=	0x800A,
		DwarfDone		=	0x800C,
		GRABARZCOUNTER	=	0x800E,
		KIERUNEK		=	0x8010,
		BACKFLAG1		=	0x8012,
		BACKFLAG2		=	0x8014,
		BACKFLAG3		=	0x8016,
		BACKFLAG4		=	0x8018,
		MACROFLAG1		=	0x801A,
		MACROFLAG2		=	0x801C,
		MACROFLAG3		=	0x801E,
		HEROLDDONE		=	0x8020,
		BRIDGESET		=	0x8022,
		U_BT_1			=	0x8024,
		U_BT_2			=	0x8026,
		U_BT_3			=	0x8028,
		U_BT_4			=	0x802A,
		U_BT_5			=	0x802C,
		U_BT_6			=	0x802E,
		U_BT_7			=	0x8030,
		U_BT_8			=	0x8032,
		U_BT_9			=	0x8034,
		U_BT_COUNTER	=	0x8036,
		ARIVALDALIVE	=	0x8038,
		TALKCHAR1		=	0x803A,
		TalkType1		=	0x803C,
		TALKROUT1		=	0x803E,
		TALKROUT2		=	0x8042,
		TALKROUT3		=	0x8046,
		TALKROUT4		=	0x804A,
		TALKANIM1		=	0x804E,
		TALKANIM2		=	0x8050,
		TALKCOLOR1		=	0x8052,
		TALKCOLOR2		=	0x8054,
		KapciuchTaken	=	0x8056,
		CurrentBeggarA	=	0x8058,
		TempKapc		=	0x805A,
		HomTaken		=	0x805C,
		WizardTalk		=	0x805E,
		SunlordTalk		=	0x8060,
		HermitTalk		=	0x8062,
		RunyMode		=	0x8064,
		FatMerchantTalk	=	0x8066,
		HotDogTalk		=	0x8068,
		ThiefTalk		=	0x806A,
		BeggarTalk		=	0x806C,
		// DwarfTalk		=	0x806E, // Redefinition
		MonkTalk		=	0x8070,
		BardTalk		=	0x8072,
		BarmanTalk		=	0x8074,
		LeftPlayerTalk		=	0x8076,
		OczySowy		=	0x8078,
		CzachySpeed1		=	0x807A,
		CzachySpeed2		=	0x807C,
		CzachySpeed3		=	0x807E,
		CzachySlowDown1		=	0x8080,
		CzachySlowDown2		=	0x8082,
		CzachySlowDown3		=	0x8084,
		FjordDane		=	0x8086,
		GKopany1		=	0x8088,
		GKopany2		=	0x808A,
		GKopany3		=	0x808C,
		GKopany4		=	0x808E,
		KnowGodWord		=	0x8090,
		TALKROUT21		=	0x8092,
		TALKROUT22		=	0x8096,
		TALKROUT23		=	0x809A,
		TALKROUT24		=	0x809E,
		TalkType2		=	0x80A2,
		GrabarzTalk		=	0x80A4,
		LastTalker		=	0x80A6,
		MapaPustelniaEnabled	=	0x80A8,
		MapaTempleEnabled	=	0x80AA,
		MapaFjordEnabled	=	0x80AC,
		MapaSilmanionaEnabled	=	0x80AE,
		MapaKurhanEnabled	=	0x80B0,
		MapaDragonEnabled	=	0x80B2,
		MapaMillEnabled		=	0x80B4,
		DwarfRunning		=	0x80B6,
		DwarfTalk		=	0x80B8,
		CurseLift		=	0x80BA,
		KosciSwapped		=	0x80BC,
		BookStolen		=	0x80BE,
		MapaUsable		=	0x80C0,
		FjordBoss		=	0x80C2,
		FjordHotDog		=	0x80C4,
		FjordLewy		=	0x80C6,
		FjordPrawy		=	0x80C8,
		TalkArivald		=	0x80CA,
		ShootDone		=	0x80CC,
		ShootRunning		=	0x80CE,
		ShootKnow		=	0x80D0,
		MirrorKnow		=	0x80D2,
		Gar1stTime		=	0x80D4,
		KosciTaken		=	0x80D6,
		ArivGotSpell		=	0x80D8,
		BookGiven		=	0x80DA,
		Wywieszka		=	0x80DC,
		TalkSheila		=	0x80DE,
		TalkSheila2		=	0x80E0,
		BackHuman		=	0x80E2,
		SkarbiecOpen		=	0x80E4,
		LustroTaken		=	0x80E6,
		GargoyleHom		=	0x80E8,
		GargoyleBroken		=	0x80EA,
		FjordDzien		=	0x80EC,
		GargoyleHom2		=	0x80EE,
		RunMonstersRunning	=	0x80F0,
		FoundPaperInCoffin	=	0x80F2,
		KnowSunlord		=	0x80F4,
		KnowSunlordTalk		=	0x80F6,
		ArivaldCzyta		=	0x80F8,
		TelepX			=	0x80FA,
		TelepY			=	0x80FC,
		TelepDir		=	0x80FE,
		TelepRoom		=	0x8100,
		ListStolen		=	0x8102,
		WifeInDoor		=	0x8104,
		TalkWifeFlag		=	0x8106,
		LetterGiven		=	0x8108,
		LutniaTaken		=	0x810A,
		BardHomeOpen		=	0x810C,
		FjordNoMonsters		=	0x810E,
		ShandriaWallTalking	=	0x8110,
		ShandriaWallCounter	=	0x8112,
		ShandriaWallDone	=	0x8114,
		FutureDone		=	0x8116,
		TalkButch		=	0x8118,
		GotSzalik		=	0x811A,
		GotCzosnek		=	0x811C,
		BearDone		=	0x811E,
		NekrVisited		=	0x8120,
		SunRiddle		=	0x8122,
		PtaszekAway		=	0x8124,
		KotGadanie		=	0x8126,
		SzlafmycaTaken		=	0x8128,
		BabkaTalk		=	0x812A,
		SellerTalk		=	0x812C,
		CzosnekDone		=	0x812E,
		PriestCounter		=	0x8130,
		PriestGest1		=	0x8132,
		PriestGest2		=	0x8134,
		PriestGest3		=	0x8136,
		PriestGest4		=	0x8138,
		PriestAnim		=	0x813A,
		HolyWaterTaken		=	0x813C,
		AxeTaken		=	0x813E,
		BadylTaken1		=	0x8140,
		BadylTaken2		=	0x8142,
		BadylSharpened		=	0x8144,
		PorwanieSmoka		=	0x8146,
		ShopReOpen		=	0x8148,
		LuskaShown		=	0x814A,
		CudKnow			=	0x814C,
		VampireDead		=	0x814E,
		MapaVisible1		=	0x8150,
		MapaVisible2		=	0x8152,
		MapaVisible3		=	0x8154,
		MapaVisible4		=	0x8156,
		MapaVisible5		=	0x8158,
		MapaVisible6		=	0x815A,
		MapaVisible7		=	0x815C,
		MapaVisible8		=	0x815E,
		MapaVisible9		=	0x8160,
		MapaX			=	0x8162,
		MapaY			=	0x8164,
		MapaD			=	0x8166,
		OldMapaX		=	0x8168,
		OldMapaY		=	0x816A,
		OldMapaD		=	0x816C,
		MovingBack		=	0x816E,
		MapaCount		=	0x8170,
		Pustelnia1st		=	0x8172,
		CzarnePole1st		=	0x8174,
		TalkArivNum		=	0x8176,
		Pfui			=	0x8178,
		MapaSunlordEnabled	=	0x817A,
		WebDone			=	0x817C,
		DragonDone		=	0x817E,
		KanPlay			=	0x8180,
		OldKanPlay		=	0x8182,
		LapkiWait		=	0x8184,
		WebNoCheck		=	0x8186,
		Perfumeria		=	0x8188,
		SmokNoCheck		=	0x818A,
		IluzjaBroken		=	0x818C,
		IluzjaWorking		=	0x818E,
		IluzjaCounter		=	0x8190,
		KurhanOpen1		=	0x8192,
		KastetTaken		=	0x8194,
		KastetDown		=	0x8196,
		KurhanDone		=	0x8198,
		SkelCounter		=	0x819A,
		SkelDial1		=	0x819C,
		SkelDial2		=	0x819E,
		SkelDial3		=	0x81A0,
		SkelDial4		=	0x81A2,
		SameTalker		=	0x81A4,
		RunMonstersText		=	0x81A6,
		PiwnicaChecked		=	0x81A8,
		DragonTalked		=	0x81AA,
		ToldAboutBook		=	0x81AC,
		SilmanionaDone		=	0x81AE,
		ToldBookCount		=	0x81B0,
		SmrodNoCheck		=	0x81B2,
		RopeTaken		=	0x81B4,
		RopeTime		=	0x81B6,
		LaskaFree		=	0x81B8,
		ShanSmokTalked		=	0x81BA,
		SwordTaken		=	0x81BC,
		Mill1st			=	0x81BE,
		SawRat			=	0x81C0,
		KnowRat			=	0x81C2,
		DziuraTimer		=	0x81C4,
		LaskaInside		=	0x81C6,
		HoleBig			=	0x81C8,
		EnableWiedzmin		=	0x81CA,
		EnableTrucizna		=	0x81CC,
		KnowPoison		=	0x81CE,
		KufelTaken		=	0x81D0,
		BojkaEnabled		=	0x81D2,
		BitwaNot1st		=	0x81D4,
		BojkaTimer		=	0x81D6,
		BojkaGirl		=	0x81D8,
		Look1st			=	0x81DA,
		RatTaken		=	0x81DC,
		LaskaTalkedGr		=	0x81DE,
		RatusGivus		=	0x81E0,
		MamObole		=	0x81E2,
		Speed1st		=	0x81E4,
		SpeedTimer		=	0x81E6,
		ProveIt			=	0x81E8,
		Proven			=	0x81EA,
		ShowWoalka		=	0x81EC,
		PoisonTaken		=	0x81EE,
		HellOpened		=	0x81F0,
		HellNoCheck		=	0x81F2,
		TalAn1			=	0x81F4,
		TalAn2			=	0x81F6,
		TalAn3			=	0x81F8,
		TalkDevilGuard		=	0x81fA,
		Sword1st		=	0x81FC,
		IluzjaNoCheck		=	0x81FE,
		RozdzielniaNumber	=	0x8200,
		JailChecked		=	0x8202,
		JailTalked		=	0x8204,
		TrickFailed		=	0x8206,
		WegielVisible		=	0x8208,
		WegielTimer1		=	0x820A,
		RandomSample		=	0x820C,
		RandomSampleTimer	=	0x820E,
		SampleTimer		=	0x8210,
		ZonaSample		=	0x8212,
		HoleTryAgain		=	0x8214,
		TeleportTimer		=	0x8216,
		RozLezy			=	0x8218,
		UdkoTimer		=	0x821A,
		ZaworZatkany		=	0x821C,
		ZaworOpened		=	0x821E,
		DoorExploded		=	0x8220,
		SkoraTaken		=	0x8222,
		CiezkieByl		=	0x8224,
		MamWegiel		=	0x8226,
		SwiecaAway		=	0x8228,
		ITSAVE			=	0x822A,
		RozpadlSie		=	0x822C,
		WegielFullTimer		=	0x822E,
		WegielDown		=	0x8230,
		WegielDownTimer		=	0x8232,
		PaliSie			=	0x8234,
		DiabGuardTalked		=	0x8236,
		GuardsNoCheck		=	0x8238,
		TalkedPowloka		=	0x823A,
		JailOpen		=	0x823C,
		PrzytulTimer		=	0x823E,
		JailDone		=	0x8240,
		MamMonety		=	0x8242,
		LotTimer		=	0x8244,
		LotObj			=	0x8246,
		PtakTimer		=	0x8248,
		BookTimer		=	0x824A,
		BookGiba		=	0x824C,
		PtakLata		=	0x824E,
		Podej			=	0x8250,
		GotHint			=	0x8252,
		LawaLeci		=	0x8254,
		PowerKlik		=	0x8258,
		LucekBad		=	0x825A,
		LucekBad1st		=	0x825C,
		IntroDial1		=	0x825E,
		IntroDial2		=	0x8260,
		ItsOutro		=	0x8262,
		KamienComment		=	0x8264,
		KamienSkip		=	0x8266,
		TesterFlag		=	0x8268,
		RememberLine		=	0x826A,
		OpisLapek		=	0x826C,
		//OpisKamienia		=	0x826E, // Redefinition
		TalWait			=	0x8270,
		OpisKamienia		=	0x8272,
		JumpBox			=	0x8274,
		JumpBox1		=	0x8276,
		JumpBox2		=	0x8278,
		JumpBox3		=	0x827A,
		SpecPiesek		=	0x827C,
		SpecPiesekCount		=	0x827E,
		SpecPiesekGadanie	=	0x8282,
		ZnikaFlag		=	0x8284,
		ZnikaTimer		=	0x8286,
		SowaTimer		=	0x8288,
		MamrotanieOff		=	0x828A,
		//	System flags controlled by script
		CURRMOB		=	0x8400,
		KOLOR		=	0x8402,
		MBFLAG		=	0x8404,
		MXFLAG		=	0x8406,
		MYFLAG		=	0x8408,
		SCROLLTYPE	=	0x840A,
		SCROLLVALUE	=	0x840C,
		SCROLLVALUE2	=	0x840E,
		TALKEXITCODE	=	0x8410,
		SPECROUTFLAG1	=	0x8412,
		SPECROUTFLAG2	=	0x8414,
		SPECROUTFLAG3	=	0x8416,
		TALKFLAGCODE	=	0x8418,
		CURRROOM	=	0x841A,
		Talker1Init	=	0x841C,
		Talker2Init	=	0x841E,
		RESTOREROOM	=	0x8420,
		INVALLOWED	=	0x8422,
		BOXSEL		=	0x8424,
		CURSEBLINK	=	0x8426,
		EXACTMOVE	=	0x8428,
		MOVEDESTX	=	0x842A,
		MOVEDESTY	=	0x842C,
		NOANTIALIAS	=	0x842E,
		ESCAPED		=	0x8430,
		ALLOW1OPTION	=	0x8432,
		VOICE_H_LINE	=	0x8434,
		VOICE_A_LINE	=	0x8436,
		VOICE_B_LINE	=	0x8438,
		VOICE_C_LINE	=	0x843A,
		NOHEROATALL	=	0x843C,
		MOUSEENABLED	=	0x843E,
		DIALINES	=	0x8440,
		//SELITEM		=	0x8442, // Redefinition
		SHANWALK	=	0x8444,
		SHANDOG		=	0x8446,
		GETACTIONBACK	=	0x8448,
		GETACTIONDATA	=	0x844C,
		GETACTION	=	0x8450,
		HEROFAST	=	0x8452,
		SELITEM		=	0x8454,
		LMOUSE		=	0x8456,
		MINMX		=	0x8458,
		MAXMX		=	0x845A,
		MINMY		=	0x845C,
		MAXMY		=	0x845E,
		TORX1		=	0x8460,
		TORY1		=	0x8462,
		TORX2		=	0x8464,
		TORY2		=	0x8466,
		POWER		=	0x8468,
		POWERENABLED	=	0x846A,
		FLCRESTORE	=	0x846C,
		NOCLSTEXT	=	0x846E,
		ESCAPED2	=	0x8470
	};

	struct FlagDebug {
		Id id;
		char flagName[30];
	};

	static const int kFlagDebugAmount = 368;
	static const FlagDebug _flagNames[kFlagDebugAmount];
};

} // End of namespace Prince

#endif
