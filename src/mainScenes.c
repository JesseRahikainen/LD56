
#include "mainScenes.h"

#include <stdio.h>

#include "game.h"
#include "stringReplacement.h"
#include "baseScenes.h"



void titleScene( CLOSURE( void )* closure, bool firstTimeRun )
{
	startDraw( window ); {
		char* blockText =
			" _____ _           ______    _       ______    _ _    \n"
			"|_   _| |          |  ___|  (_)      |  ___|  | | |   \n"
			"  | | | |__   ___  | |_ __ _ _ _ __  | |_ ___ | | | __\n"
			"  | | | '_ \\ / _ \\ |  _/ _` | | '__| |  _/ _ \\| | |/ /\n"
			"  | | | | | |  __/ | || (_| | | |    | || (_) | |   < \n"
			"  \\_/ |_| |_|\\___| \\_| \\__,_|_|_|    \\_| \\___/|_|_|\\_\\\n"
			"                                                      \n"
			"                                                      \n"
			"          __  ______ _     _ _              __        \n"
			"         / _| | ___ (_)   | (_)            / _|       \n"
			"    ___ | |_  | |_/ /_  __| |_  __ _ _ __ | |_ __ _   \n"
			"   / _ \\|  _| |    /| |/ _` | |/ _` | '_ \\|  _/ _` |  \n"
			"  | (_) | |   | |\\ \\| | (_| | | (_| | | | | || (_| |  \n"
			"   \\___/|_|   \\_| \\_|_|\\__,_|_|\\__,_|_| |_|_| \\__,_|  \n";

		centerStringHoriz( window, renderArea, 12, blockText, attr_Clrs( PDC_CYAN, PDC_BLACK ) );

		centerStringHoriz( window, renderArea, 34, "A Game Created For", attr_Clrs( PDC_GREY, PDC_BLACK ) );
		centerStringHoriz( window, renderArea, 35, "Ludum Dare 56", attr_Clrs( PDC_BROWN, PDC_BLACK ) );

		centerStringHoriz( window, safeWriteArea, safeWriteArea.bottom, "Press any key to begin", attr_Clrs( PDC_YELLOW, PDC_BLACK ) );

	} endDraw( window );

	initCharacter( );

	waitForAnyInput( window );

	switchScene( closure_Make( introScene, 0 ) );
}

void introScene( CLOSURE( void )* closure, bool firstTimeRun )
{
	// set up which fae will be on the farm, will choose 4, the cannibal is always there
	Companions allFey[NUM_COMPANIONS];
	for( size_t i = 0; i < ARRAYSIZE( allFey ); ++i ) allFey[i] = i;
	SHUFFLE( allFey, Companions, i );
	for( size_t i = 0; i < 4; ++i ) addFlagToCompanion( &character, allFey[i], CF_THERE );

	// set up the days for the various times things will visit
	gameCharacterData.cannibalKidnapDay = 20;
	gameCharacterData.cannibalDoorDay = 8;
	do {
		gameCharacterData.hungryManDay = 2 + balancedRandom( 16 ); // between day 3 and 18 (2 and 17 if using zero indexed)
	} while( gameCharacterData.hungryManDay == gameCharacterData.cannibalDoorDay );

	gameCharacterData.wellDay = 2 + balancedRandom( 16 ); // between day 3 and 18 (2 and 17 if using zero indexed)
	do {
		gameCharacterData.treeDay = 2 + balancedRandom( 16 ); // between day 3 and 18 (2 and 17 if using zero indexed)
	} while( gameCharacterData.treeDay == gameCharacterData.wellDay );
	do {
		gameCharacterData.prankDay = 2 + balancedRandom( 16 ); // between day 3 and 18 (2 and 17 if using zero indexed)
	} while( ( gameCharacterData.prankDay == gameCharacterData.treeDay ) && ( gameCharacterData.prankDay == gameCharacterData.wellDay ) );

	gameCharacterData.daysWithoutFamilyInteraction = 0;
	gameCharacterData.daysWithoutHouseMilk = 0;

	gameCharacterData.spouseBusyWith = NUM_FARM_STATUS;
	gameCharacterData.childBusyWith = NUM_FARM_STATUS;

	if( isCompanionWithCharacter( &rpgGame, &character, CMP_BUILDER ) ) {
		addReplacementString( "BUILDER_INITIAL_DESCRIPTION", "Every building had been replaced recently except for the barn. It has somehow survived the decades that have passed and is in fairly good condition" );
	} else {
		addReplacementString( "BUILDER_INITIAL_DESCRIPTION", "Every building had been replaced recently and the paint had barely dried by the time all of you had moved in" );
	}

	topTitle = " The Story So Far ";
	startDraw( window ); {
		drawString( window, 2, 2, safeWriteArea,
			"You grew up on a farm deep in the country. Every day was getting up at the crack of dawn, feeding the animals, taking care of the crops, and doing "
			"whatever needed to be done to keep danger away, whether natural or not. Your father wasn't happy here though and moved the family to the "
			"city. He worked at the docks while your mother worked as a maid. They made enough money to get you a decent education.\n\n"
			"You and [SPOUSE_NAME] were married around fifteen years ago. A few years later you had a child who you named [CHILD_NAME]. Working hard as a clerk "
			"you were able to save up some money. You had missed those years on the farm, and after talking it through with [SPOUSE_NAME], the both of you decided it would "
			"be good to get out of the city. Shortly afterwards you found out you were going to have another child. After [BABY_NAME] had been born the family moved. "
			"Spending your savings you were able to buy a plot of land that had a house and barn on it, along with the necessary tools, seeds, and livestock.\n\n"
			"The farm was recently renovated. [BUILDER_INITIAL_DESCRIPTION]. It wasn't much land, and bordered on a wild forest, but you would make this work. It's been "
			"a few weeks and everyone is settled in.",
			attr_Clrs( PDC_GREY, PDC_BLACK ) );

		centerStringHoriz( window, safeWriteArea, safeWriteArea.bottom, "Press any key to start", attr_Clrs( PDC_YELLOW, PDC_BLACK ) );
	} endDraw( window );

	waitForAnyInput( window );

	switchScene( closure_Make( morningScene, 0 ) );
}

void helpScene( CLOSURE( void )* closure, bool firstTimeRun )
{
	topTitle = " Help ";
	Coord outPos;

	startDraw( window ); {
		outPos = drawString( window, safeWriteArea.left, safeWriteArea.top, safeWriteArea, "Controls", attr_FGClr( PDC_WHITE ) );
		outPos = drawString( window, safeWriteArea.left + 1, outPos.y + 1, safeWriteArea,
			"When presented with a choice press up and down to select the choice you want. Then press enter to make that choice.\n"
			"Pressing H will bring up this help screen, pressing C will bring up a more detailed character sheet.",
			attr_FGClr( PDC_GREY ) );
		outPos.y += 1;

		outPos = drawString( window, safeWriteArea.left, outPos.y + 1, safeWriteArea, "Choices", attr_FGClr( PDC_WHITE ) );
		outPos = drawString( window, safeWriteArea.left + 1, outPos.y + 1, safeWriteArea,
			"Some choices will have a chance of failure. How well these are performed are determined by rolling the die for the "
			"person that performs them minus the number of Wounds they have. If do well then nothing bad will happen. If they roll too low they will be hurt and will "
			"not finish what was attempted. If they roll in the middle they will have a costly success, they will finish the task "
			"but be hurt while doing so. This may be an actual injury or just them wearing down as they have to perform hard work.\n"
			"If you see an arrow at the bottom or top of the choice section of the screen that means there are more choices in that "
			"direction.",
			attr_FGClr( PDC_GREY ) );
		outPos.y += 1;

		outPos = drawString( window, safeWriteArea.left, outPos.y + 1, safeWriteArea, "Wounds", attr_FGClr( PDC_WHITE ) );
		outPos = drawString( window, safeWriteArea.left + 1, outPos.y + 1, safeWriteArea,
			"Failures can result in someone gaining a Wound. These Wounds are associated with a specific person and will give a penalty "
			"to any rolls they make. If the number of Wounds they have equals their die size, they will die and the game will be over.\n"
			"Each night there's a chance someone will heal a Wound.",
			attr_FGClr( PDC_GREY ) );
		outPos.y += 1;

		outPos = drawString( window, safeWriteArea.left, outPos.y + 1, safeWriteArea, "Farm Status", attr_FGClr( PDC_WHITE ) );
		outPos = drawString( window, safeWriteArea.left + 1, outPos.y + 1, safeWriteArea,
			"This indicates how well your farm is doing. You have four rankings: Crops, Animals, Farm House, and Family. Keeping them "
			"high enough will make keeping other parts of your farm easier.\n"
			"Crops is how well the crops on your farm are doing. Taking care of them includes making sure they have enough water, "
			"keeping pests away, and harvesting once ripe.\n"
			"Animals is how healthy and happy the various chicked, cows, and horses you have on the farm are. Taking care of them "
			"includes feeding, brushing, watering, and exercising them. And also making sure the barn is in good shape.\n"
			"Farm House is how clean and sound your home is. Taking care of this includes cleaning and doing small repairs.\n"
			"Family is how cohesive and happy your family is.",
			attr_FGClr( PDC_GREY ) );
		outPos.y += 1;

		outPos = drawString( window, safeWriteArea.left, outPos.y + 1, safeWriteArea, "Rules", attr_FGClr( PDC_WHITE ) );
		outPos = drawString( window, safeWriteArea.left + 1, outPos.y + 1, safeWriteArea,
			"Each day you'll have two actions you can do. In the morning you can ask your family to help with the various parts of the farm. "
			"They have other things to take care of during the afternoon. When helping there is "
			"a chance they will gain a Wound while doing so. If you or any of your family members have more Wounds then they have die size "
			"they'll die and the game will be over.\n"
			"Your farm requires constant upkeep. Each day there is a chance that the rating of a part of the farm will decay. If any of "
			"the parts of your farm reach of level of 0 the game will be over. You can counteract this by using your, or the families, "
			"actions to perform upkeep on the farm.",
			attr_FGClr( PDC_GREY ) );
		outPos.y += 1;

		centerStringHoriz( window, renderArea, renderArea.bottom, "Press any key to continue", attr_Clrs( PDC_YELLOW, PDC_BLACK ) );
	} endDraw( window );

	waitForAnyInput( window );
	eatAllInputs( window );

	topTitle = NULL;
	popScene( );
}