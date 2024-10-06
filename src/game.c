#include "game.h"

#include <stdio.h>
#include <assert.h>

#include "stringReplacement.h"
#include "mainScenes.h"
#include "baseScenes.h"

Window* window = NULL;
RPGGame rpgGame;
Character character;
GameCharacterData gameCharacterData;

static bool choiceMade = false; // indicates if a choice has been made and the scene should progress to the new scene
static int choiceTop;
static int standardCurrentChoice;

void setupCompanionData( void )
{
	initCompanions( &rpgGame, NUM_COMPANIONS, CF_THERE );

	addCompanion( &rpgGame, CMP_HOUSE_HOB, "Timthirid", "a tiny, hairy man", NUM_STATS, 0, 0, 0, 0 );
	addCompanion( &rpgGame, CMP_FAIRY, "Meraige", "a tiny, winged woman", NUM_STATS, 0, 0, 0, 0 );
	addCompanion( &rpgGame, CMP_DRYAD, "Fidbaid", "a tiny woman with skin like bark", NUM_STATS, 0, 0, 0, 0 );
	addCompanion( &rpgGame, CMP_BABY_SITTER, "Derbshiur", "a tiny, twisted woman with many arms", NUM_STATS, 0, 0, 0, 0 );
	addCompanion( &rpgGame, CMP_BUILDER, "Foirmeacha", "a tiny man with a tall hat and long beard", NUM_STATS, 0, 0, 0, 0 );
	addCompanion( &rpgGame, CMP_WATER, "Ginach", "a snake with long hair, arms, and legs", NUM_STATS, 0, 0, 0, 0 );
	addCompanion( &rpgGame, CMP_HUNGRY, "Occorach Fer", "a short, emaciated man with yellow eyes", NUM_STATS, 0, 0, 0, 0 );
	//addCompanion( &rpgGame, CMP_CANNIBAL, "Torathar", "a hunched creature with red eyes and long claws", NUM_STATS, 0, 0, 0, 0 );

	rpgGame.companionWithFlag = CF_THERE;
}

void setupGearData( void )
{
}

void setupStatsData( void )
{
	initStats( &rpgGame, NUM_STATS );

	addStat( &rpgGame, STAT_SELF, "You", "[YOUR_NAME]", "YOUR" );
	rpgGame.arrStats[STAT_SELF].subjectPronoun = "you";
	rpgGame.arrStats[STAT_SELF].objectPronoun = "you";
	rpgGame.arrStats[STAT_SELF].possessivePronoun = "your";
	addStat( &rpgGame, STAT_SPOUSE, "[SPOUSE_NAME]", "[SPOUSE_NAME]", "SPOUSE" );
	rpgGame.arrStats[STAT_SPOUSE].subjectPronoun = "[SPOUSE_SUBJECT_PRONOUN]";
	rpgGame.arrStats[STAT_SPOUSE].objectPronoun = "[SPOUSE_OBJECT_PRONOUN]";
	rpgGame.arrStats[STAT_SPOUSE].possessivePronoun = "[SPOUSE_POSSESSIVE_PRONOUN]";
	addStat( &rpgGame, STAT_CHILD, "[CHILD_NAME]", "[CHILD_NAME]", "CHILD" );
	rpgGame.arrStats[STAT_CHILD].subjectPronoun = "[CHILD_SUBJECT_PRONOUN]";
	rpgGame.arrStats[STAT_CHILD].objectPronoun = "[CHILD_OBJECT_PRONOUN]";
	rpgGame.arrStats[STAT_CHILD].possessivePronoun = "[CHILD_POSSESSIVE_PRONOUN]";
}

void setupSkillsData( void )
{
	initSkills( &rpgGame, NUM_SKILLS );

	addSkill( &rpgGame, SKL_CROPS, INVALID_STAT, "Crops", "crops", "" );
	addSkill( &rpgGame, SKL_ANIMALS, INVALID_STAT, "Animals", "animals and barn", "" );
	addSkill( &rpgGame, SKL_FARMHOUSE, INVALID_STAT, "Farmhouse", "farm house", "" );
	addSkill( &rpgGame, SKL_FAMILY, INVALID_STAT, "Family", "", "" );
}

void setupDisciplines( void )
{
	initDisciplines( &rpgGame, NUM_FARM_STATUS );

	// these will be how well the farm is doing, if any of these reach 0 the game is over, similar to how wounds work
	// they will also deteriorate over time and need constant upkeep

	// each will give bonuses to upkeep other areas of the farm based on their level, will give bonuses at levels 6, 8, and 10

	addDiscipline( &rpgGame, FS_CROPS, "Crops", "How well your crops are doing.", 10, IA_PROPER );
	addDisciplinePassiveSkillChange( &rpgGame, FS_CROPS, 6, SKL_ANIMALS, 1 );
	addDisciplinePassiveSkillChange( &rpgGame, FS_CROPS, 8, SKL_FAMILY, 1 );
	addDisciplinePassiveSkillChange( &rpgGame, FS_CROPS, 10, SKL_FARMHOUSE, 1 );

	addDiscipline( &rpgGame, FS_ANIMALS, "Animals", "How well your animals are doing.", 10, IA_PROPER );
	addDisciplinePassiveSkillChange( &rpgGame, FS_ANIMALS, 6, SKL_CROPS, 1 );
	addDisciplinePassiveSkillChange( &rpgGame, FS_ANIMALS, 8, SKL_FARMHOUSE, 1 );
	addDisciplinePassiveSkillChange( &rpgGame, FS_ANIMALS, 10, SKL_FAMILY, 1 );

	addDiscipline( &rpgGame, FS_FARMHOUSE, "Farm House", "How well your house and other buildings are holding up.", 10, IA_PROPER );
	addDisciplinePassiveSkillChange( &rpgGame, FS_FARMHOUSE, 6, SKL_FAMILY, 1 );
	addDisciplinePassiveSkillChange( &rpgGame, FS_FARMHOUSE, 8, SKL_ANIMALS, 1 );
	addDisciplinePassiveSkillChange( &rpgGame, FS_FARMHOUSE, 10, SKL_CROPS, 1 );

	addDiscipline( &rpgGame, FS_FAMILY, "Family", "How well your family gets along.", 10, IA_PROPER );
	addDisciplinePassiveSkillChange( &rpgGame, FS_FAMILY, 6, SKL_FARMHOUSE, 1 );
	addDisciplinePassiveSkillChange( &rpgGame, FS_FAMILY, 8, SKL_CROPS, 1 );
	addDisciplinePassiveSkillChange( &rpgGame, FS_FAMILY, 10, SKL_ANIMALS, 1 );
}

void setupClassData( void )
{
}

void setupGame( )
{
	initGame( &rpgGame );
	setupGearData( );
	setupStatsData( );
	setupSkillsData( );
	setupDisciplines( );
	setupCompanionData( );
	setupClassData( );

	rpgGame.enucumberanceStat = INVALID_STAT;
	rpgGame.enucumberanceSkill = INVALID_STAT;
}

void initCharacter( )
{
	character = createCharacter( &rpgGame, NULL );

	for( int i = 0; i < NUM_STATS; ++i ) {
		character.arrStats[i] = 8;
	}

	character.arrStats[STAT_SELF] = 8;
	character.arrStats[STAT_SPOUSE] = 8;
	character.arrStats[STAT_CHILD] = 6;

	for( int i = 0; i < NUM_SKILLS; ++i ) {
		character.arrSkills[i] = 4;
	}

	// fae that will start out appeased
	addFlagToCompanion( &character, CMP_HOUSE_HOB, CF_APPEASED );
	addFlagToCompanion( &character, CMP_FAIRY, CF_APPEASED );
	addFlagToCompanion( &character, CMP_BABY_SITTER, CF_APPEASED );
	addFlagToCompanion( &character, CMP_BUILDER, CF_APPEASED );

	// farm will start at leve 4 for everything
	for( int i = 0; i < NUM_FARM_STATUS; ++i ) {
		character.arrDisciplineLevels[i] = 4;
	}
}

bool isDead( )
{
	for( int i = 0; i < NUM_STATS; ++i ) {
		if( character.arrWounds[i] >= character.arrStats[i] ) {
			return true;
		}
	}

	return false;
}

bool isDeadFromStat( Stats stat )
{
	return character.arrWounds[stat] >= character.arrStats[stat];
}

bool isDeadFromFarmStatus( FarmStatus status )
{
	return character.arrDisciplineLevels[status] <= 0;
}

bool isWounded( )
{
	for( Stats stat = 0; stat < NUM_STATS; ++stat ) {
		if( character.arrWounds[stat] > 0 ) return true;
	}
	return false;
}

Stats getRandomWound( )
{
	InfiniteListSelector selector;
	infiniteListSelector_Init( &selector, INVALID_STAT );
	for( Stats stat = 0; stat < NUM_STATS; ++stat ) {
		for( uint8_t i = 0; i < character.arrWounds[stat]; ++i ) {
			infiniteListSelector_Choose( &selector, stat, character.arrWounds[stat] );
		}
	}
	return selector.chosenId;
}

#define DESC_BORDER_BOTTOM 41
#define DESC_BORDER_RIGHT 55
SmallRect descriptionSafeArea = { 2, 2, DESC_BORDER_RIGHT - 2, DESC_BORDER_BOTTOM - 2 };
SmallRect choicesSafeArea = { 3, DESC_BORDER_BOTTOM + 1, DESC_BORDER_RIGHT - 2, SCREEN_HEIGHT - 2 };
SmallRect characterSafeArea = { DESC_BORDER_RIGHT + 2, 2, SCREEN_WIDTH - 3, SCREEN_HEIGHT - 2 };

Coord drawMainScreenStatString( Stats stat, Window* screen, int16_t x, int16_t y, SmallRect border )
{
	int8_t companionStatBonus = statBoostFromAllCompanions( &rpgGame, &character, stat );
	int8_t spiritStatBonus = allDisciplineStatChanges( &rpgGame, &character, stat );
	if( ( companionStatBonus > 0 ) && ( spiritStatBonus > 0 ) ) {
		return drawString( window, x, y, border, "%s: d%i{fg=Cyan}+%i{/fg}{fg=DarkGrey}+%i{/fg}", attr_Clrs( PDC_GREEN, PDC_BLACK ), rpgGame.arrStats[stat].name, character.arrStats[stat], companionStatBonus, spiritStatBonus );
	} else if( companionStatBonus > 0 ) {
		return drawString( window, x, y, border, "%s: d%i{fg=Cyan}+%i{/fg}", attr_Clrs( PDC_GREEN, PDC_BLACK ), rpgGame.arrStats[stat].name, character.arrStats[stat], companionStatBonus );
	} else if( spiritStatBonus > 0 ) {
		return drawString( window, x, y, border, "%s: d%i{fg=DarkGrey}+%i{/fg}", attr_Clrs( PDC_GREEN, PDC_BLACK ), rpgGame.arrStats[stat].name, character.arrStats[stat], spiritStatBonus );
	} else {
		return drawString( window, x, y, border, "%s: d%i", attr_Clrs( PDC_GREEN, PDC_BLACK ), rpgGame.arrStats[stat].name, character.arrStats[stat] );
	}
}

Coord drawMainScreenSkillString( Skills skill, Window* screen, int16_t x, int16_t y, SmallRect border )
{
	int8_t spiritSkillBonus = allDisciplineSkillChanges( &rpgGame, &character, skill );
	if( spiritSkillBonus > 0 ) {
		return drawString( window, x, y, characterSafeArea, " %s: %i{fg=DarkGrey}+%i{/fg}", attr_FGClr( PDC_DARK_CYAN ), rpgGame.arrSkills[skill].name, character.arrSkills[skill], spiritSkillBonus );
	} else {
		return drawString( window, x, y, characterSafeArea, " %s: %i", attr_FGClr( PDC_DARK_CYAN ), rpgGame.arrSkills[skill].name, character.arrSkills[skill] );
	}
}

void startPlayDraw( void )
{
	startDraw( window );
	drawPlayScreen( );
}

void startScene( )
{
	standardCurrentChoice = 0;
	choiceTop = 0;
	topTitle = NULL;
}

bool testSharedInput( Input input )
{
	bool handled = false;
	switch( input ) {
	/*case IN_C:
		handled = true;
		pushScene( closure_Make( characterDetailsScene, 0 ) );
		choiceMade = true;
		break;*/
	case IN_H:
		handled = true;
		pushScene( closure_Make( helpScene, 0 ) );
		choiceMade = true;
		break;
	}

	return handled;
}

void fitCurrentSelection( int numChoices )
{
	// TODO: Get this working with multi-line options

	// see if the current choice is visible
	int32_t range = choicesSafeArea.bottom - choicesSafeArea.top + 1;
	while( ( standardCurrentChoice - choiceTop ) >= ( range - 1 ) ) {
		++choiceTop;
	}

	while( ( standardCurrentChoice - choiceTop ) <= 0 ) {
		--choiceTop;
	}

	// if we're showing above the top option then clamp it to the top
	if( choiceTop < 0 ) choiceTop = 0;
	// if we're showing below the bottom option then clamp it to the bottom
	if( ( ( numChoices - range ) > 0 ) && ( choiceTop > ( numChoices - range ) ) ) choiceTop = numChoices - range;
}

void standardSceneChoice( bool allowDefaults )
{
	Input input;
	bool handled = false;
	int numChoices = getNumSceneChoices( );

	do {
		input = getNextInput( window );
		if( allowDefaults ) {
			handled = testSharedInput( input );
		}
		if( !handled ) {
			switch( input ) {
			case IN_UP:
				if( standardCurrentChoice > 0 ) {
					--standardCurrentChoice;
				}
				fitCurrentSelection( numChoices );
				handled = true;
				break;
			case IN_DOWN:
				if( standardCurrentChoice < ( numChoices - 1 ) ) {
					++standardCurrentChoice;
				}
				fitCurrentSelection( numChoices );
				handled = true;
				break;
			case IN_ENTER:
				handleChoice( &rpgGame, &character, getSceneChoice( standardCurrentChoice ) );
				choiceMade = true;
				handled = true;
				break;
			}
		}
	} while( !handled );

	// eat the rest of the inputs
	eatAllInputs( window );
}

void standardSceneChoiceDraw( )
{
	PredefinedColors selectedFG = PDC_CYAN;
	PredefinedColors selectedBG = PDC_DARK_GREY;

	PredefinedColors normalFG = PDC_DARK_CYAN;
	PredefinedColors normalBG = PDC_BLACK;

	int numChoices = getNumSceneChoices( );

	Coord outPos;
	outPos.y = choicesSafeArea.top - 1;
	for( int i = choiceTop; i < numChoices; ++i ) {
		PredefinedColors fg = normalFG;
		PredefinedColors bg = normalBG;

		if( standardCurrentChoice == i ) {
			fg = selectedFG;
			bg = selectedBG;
		}

		outPos = drawString( window, choicesSafeArea.left, outPos.y + 1, choicesSafeArea, getSceneChoice( i )->base.description, attr_Clrs( fg, bg ) );
	}

	// if there are more below then show down arrow
	if( ( choiceTop + ( choicesSafeArea.bottom - choicesSafeArea.top ) ) < (int32_t)( numChoices - 1 ) ) {
		drawString( window, choicesSafeArea.left - 1, choicesSafeArea.bottom, renderArea, "v", attr_FGClr( PDC_YELLOW ) );
	}
	// if there are more above then show up arrow
	if( choiceTop > 0 ) {
		drawString( window, choicesSafeArea.left - 1, choicesSafeArea.top, renderArea, "^", attr_FGClr( PDC_YELLOW ) );
	}
}

static char* arrSceneDisplayString = NULL;
void clearSceneDisplayString( )
{
	int len = arrlen( arrSceneDisplayString );
	if( len > 0 ) arrdeln( arrSceneDisplayString, 0, len - 1 );
}

void appendToSceneDisplayString( const char* str )
{
	// remove ending 0
	if( arrlen( arrSceneDisplayString ) > 0 ) arrpop( arrSceneDisplayString );

	char* result = runReplacements( str, CAPTYPE_IGNORE, NULL );

	char* start = arraddnptr( arrSceneDisplayString, strlen( result ) + 1 );
	memcpy( start, result, strlen( result ) + 1 );

	arrfree( result );
}

void setInitialSceneDescriptionString( const char* str )
{
	clearSceneDisplayString( );
	char* finalStr = runReplacements( str, CAPTYPE_STANDARD, NULL );
	appendToSceneDisplayString( finalStr );
	arrfree( finalStr );
}

char* createCopyOfSceneDisplayString( )
{
	char* copy = NULL;
	size_t dispLen = arrlen( arrSceneDisplayString );
	arrsetlen( copy, dispLen );
	memcpy( copy, arrSceneDisplayString, dispLen );
	return copy;
}

void drawPlayScreen( void )
{
	// we already have the main border, so we just need to draw the
	//  border for the other areas (text description, choices, and character)
	simplePutChar( window, 204, attr_Clrs( PDC_GREY, PDC_BLACK ), 0, DESC_BORDER_BOTTOM );
	simplePutChar( window, 202, attr_Clrs( PDC_GREY, PDC_BLACK ), DESC_BORDER_RIGHT, SCREEN_HEIGHT - 1 );

	char c = simpleGetChar( window, DESC_BORDER_RIGHT, 0 );
	if( ( c < 0 ) || ( c > 127 ) ) {
		simplePutChar( window, 203, attr_Clrs( PDC_GREY, PDC_BLACK ), DESC_BORDER_RIGHT, 0 );
	}

	for( int i = 1; i < ( DESC_BORDER_RIGHT ); ++i ) {
		simplePutChar( window, 205, attr_Clrs( PDC_GREY, PDC_BLACK ), i, DESC_BORDER_BOTTOM );
	}

	for( int i = 1; i < ( SCREEN_HEIGHT - 1 ); ++i ) {
		simplePutChar( window, 186, attr_Clrs( PDC_GREY, PDC_BLACK ), DESC_BORDER_RIGHT, i );
	}

	simplePutChar( window, 185, attr_Clrs( PDC_GREY, PDC_BLACK ), DESC_BORDER_RIGHT, DESC_BORDER_BOTTOM );

	// draw the character info
	//  stats and skills
	Coord pos;
	pos.x = characterSafeArea.left + 2;
	pos.y = characterSafeArea.top;
	drawString( window, pos.x - 1, pos.y, characterSafeArea, "Day: %i", attr_FGClr( PDC_DARK_CYAN ), gameCharacterData.currentDay + 1 );
	pos.y += 2;
	drawString( window, pos.x - 1, pos.y, characterSafeArea, "Family Members", attr_FGClr( PDC_BROWN ) );
	pos.y += 1;

	for( Stats stat = 0; stat < NUM_STATS; ++stat ) {
		drawMainScreenStatString( stat, window, pos.x, pos.y, characterSafeArea );
		++pos.y;
		int i = 0;
		for( int i = 0; i < NUM_SKILLS; ++i ) {
			if( rpgGame.arrSkills[i].associatedStatID == stat ) {
				drawMainScreenSkillString( i, window, pos.x, pos.y, characterSafeArea );
				++pos.y;
			}
		}
	}

	// draw cumulative wounds
	++pos.y;
	drawString( window, pos.x - 1, pos.y, characterSafeArea, "Total Wounds", attr_FGClr( PDC_BROWN ) );
	for( Stats stat = 0; stat < NUM_STATS; ++stat ) {
		++pos.y;
		drawString( window, pos.x, pos.y, characterSafeArea, "%s: %i", attr_FGClr( PDC_MAROON ), rpgGame.arrStats[stat].name, character.arrWounds[stat] );
	}
	++pos.y;

	// draw farm status
	++pos.y;
	drawString( window, pos.x - 1, pos.y, characterSafeArea, "Farm", attr_FGClr( PDC_BROWN ) );
	++pos.y;
	for( FarmStatus farm = 0; farm < NUM_FARM_STATUS; ++farm ) {
		drawString( window, pos.x, pos.y, characterSafeArea, "%s: %i", attr_FGClr( PDC_DARK_GREEN ), rpgGame.arrDisciplines[farm].name, character.arrDisciplineLevels[farm] );
		++pos.y;
		for( int8_t level = 0; level <= character.arrDisciplineLevels[farm]; ++level ) {
			for( Skills skill = 0; skill < NUM_SKILLS; ++skill ) {
				int8_t change = rpgGame.arrDisciplines[farm].arrPassiveChanges[level].arrSkillChanges[skill];
				if( change > 0 ) {
					drawString( window, pos.x + 1, pos.y, characterSafeArea, "+%s", attr_FGClr( PDC_DARK_BLUE ), rpgGame.arrSkills[skill].name );
					++pos.y;
				} else if( change < 0 ) {
					drawString( window, pos.x + 1, pos.y, characterSafeArea, "-%s", attr_FGClr( PDC_MAROON ), rpgGame.arrSkills[skill].name );
					++pos.y;
				}
			}
		}
	}

	//drawString( window, characterSafeArea.left, characterSafeArea.bottom - 1, characterSafeArea, "C -> Character Sheet", attr_FGClr( PDC_YELLOW ) );
	drawString( window, characterSafeArea.left, characterSafeArea.bottom, characterSafeArea, "H -> Help", attr_FGClr( PDC_YELLOW ) );
}

void defaultSceneLoop( )
{
	// assuming all actions for the scene are carried out before the scene loop is started
	if( isDead( ) ) {
		switchScene( closure_Make( deathScene, 0 ) );
		return;
	}

	choiceMade = false;
	while( !choiceMade ) {
		startPlayDraw( ); {
			drawString( window, descriptionSafeArea.left, descriptionSafeArea.top, descriptionSafeArea, arrSceneDisplayString, attr_FGClr( PDC_GREY ) );
			standardSceneChoiceDraw( );
		} endDraw( window );
		standardSceneChoice( true );
	}

	clearSceneChoices( );
}

void pushSimpleChoice( const char* description, CLOSURE( void )* nextScene )
{
	pushSimpleChoiceFull( description, nextScene );
}

void pushSkillBasedChoice( const char* description, int skill, int stat, uint8_t difficulty,
	CLOSURE( void )* successScene, CLOSURE( void )* costlySuccessScene, CLOSURE( void )* failureScene )
{
	pushSkillBasedChoiceFull( &rpgGame, &character, description, skill, stat, difficulty, successScene, costlySuccessScene, failureScene );

#ifdef _DEBUG
	// add simple choices for testing things
	char buffer[128];
	snprintf( buffer, ARRAYSIZE( buffer ), "(S)%s", description );
	pushSimpleChoiceFull( buffer, closure_Copy( successScene ) );

	snprintf( buffer, ARRAYSIZE( buffer ), "(C)%s", description );
	pushSimpleChoiceFull( buffer, closure_Copy( costlySuccessScene ) );

	snprintf( buffer, ARRAYSIZE( buffer ), "(F)%s", description );
	pushSimpleChoiceFull( buffer, closure_Copy( failureScene ) );
#endif
}

void sceneInflictWound( Stats stat, const char* woundDescription )
{
	char name[32];
	snprintf( name, 32, "%s", rpgGame.arrStats[stat].name );
	name[0] = toupper( name[0] );

	character.arrWounds[stat] += 1;
	appendToSceneDisplayString( "\n\n{fg=Red}" );
	appendToSceneDisplayString( name );
	appendToSceneDisplayString( " " );
	appendToSceneDisplayString( woundDescription );
	appendToSceneDisplayString( " and gained a Wound.{/fg}" );
}

void sceneHealWound( Stats stat )
{
	char name[32];
	snprintf( name, 32, "%s", rpgGame.arrStats[stat].name );
	name[0] = toupper( name[0] );

	if( character.arrWounds[stat] >= 1 ) {
		appendToSceneDisplayString( "\n\n{fg=Green}" );
		appendToSceneDisplayString( name );
		if( stat == STAT_SELF ) {
			appendToSceneDisplayString( " feel better!{/fg}" );
		} else {
			appendToSceneDisplayString( " feels better!{/fg}" );
		}
		character.arrWounds[stat] -= 1;
	}
}

void sceneIncreaseDiscipline( FarmStatus status )
{
	if( character.arrDisciplineLevels[status] < rpgGame.arrDisciplines[status].maxLevel ) {
		appendToSceneDisplayString( "\n\n{fg=Green}Status level of the " );
		appendToSceneDisplayString( rpgGame.arrDisciplines[status].name );
		appendToSceneDisplayString( " has increased!{/fg}" );
		character.arrDisciplineLevels[status] += 1;
	}
}

void sceneDecreaseDiscipline( FarmStatus status )
{
	if( character.arrDisciplineLevels[status] > 0 ) {
		appendToSceneDisplayString( "\n\n{fg=Red}Status level of the " );
		appendToSceneDisplayString( rpgGame.arrDisciplines[status].name );
		appendToSceneDisplayString( " has decreased.{/fg}" );
		character.arrDisciplineLevels[status] -= 1;
	}
}