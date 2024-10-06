#include "rpg.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <assert.h>

#include "utils.h"
#include "stringReplacement.h"

uint8_t rankedDifficulties[] = { VERY_EASY_TEST, EASY_TEST, NORMAL_TEST, DIFFICULT_TEST, HARD_TEST, VERY_HARD_TEST, NEAR_IMPOSSIBLE_TEST };

int rollBetween( int min, int max )
{
	return min + balancedRandom( max - min + 1 );
}

int simpleRoll( int stat )
{
	return 1 + balancedRandom( stat );
}

void initGame( RPGGame* game )
{
	game->arrStats = NULL;
	game->arrSkills = NULL;
	game->arrGear = NULL;
	game->arrCompanions = NULL;
	game->arrDisciplines = NULL;

	game->enucumberanceStat = -1;
	game->enucumberanceSkill = -1;

	game->companionWithFlag = 0;
}

// stats are the base attributes players have, these act as dice rolls that provide the base of any action
void initStats( RPGGame* game, int numStats )
{
	assert( game );
	game->arrStats = NULL;
	arrsetlen( game->arrStats, numStats );
	for( int i = 0; i < numStats; ++i ) {
		game->arrStats[i].name = "UNSET";
		game->arrStats[i].description = "UNSET";
		game->arrStats[i].abbreviation = "UNSET";
	}
}

void addStat( RPGGame* game, int id, const char* name, const char* description, const char* abbreviation )
{
	assert( game );
	assert( game->arrStats );
	assert( id < arrlen( game->arrStats ) );
	assert( id >= 0 );
	game->arrStats[id].name = name;
	game->arrStats[id].description = description;
	game->arrStats[id].abbreviation = abbreviation;
}

// skills are constant parts of any action test, when testing a skill you roll the stat associated with the skill then add the value for the skill
void initSkills( RPGGame* game, int numSkills )
{
	assert( game );
	game->arrSkills = NULL;
	arrsetlen( game->arrSkills, numSkills );
	for( int i = 0; i < numSkills; ++i ) {
		game->arrSkills[i].name = "UNSET";
		game->arrSkills[i].description = "UNSET";
		game->arrSkills[i].abbreviation = "UNSET";
		game->arrSkills[i].associatedStatID = -1;
	}
}

void addSkill( RPGGame* game, int id, int associatedStatID, const char* name, const char* description, const char* abbreviation )
{
	assert( game );
	assert( game->arrSkills );
	assert( id < arrlen( game->arrSkills ) );
	assert( id >= 0 );
	game->arrSkills[id].associatedStatID = associatedStatID;
	game->arrSkills[id].name = name;
	game->arrSkills[id].description = description;
	game->arrSkills[id].abbreviation = abbreviation;
}

// gear are items that take up space
void initGearTypes( RPGGame* game, int numGear )
{
	assert( game );
	game->arrGear = NULL;
	arrsetlen( game->arrGear, numGear );
	for( int i = 0; i < numGear; ++i ) {
		game->arrGear[i].name = "UNSET";
		game->arrGear[i].description = "UNSET";
		game->arrGear[i].value = 0;
		game->arrGear[i].flags = 0;
		game->arrGear[i].alternateVersionID = -1;
		game->arrGear[i].article = IA_UNCOUNTABLE;
	}
}

void addGearType( RPGGame* game, int id, char* name, const char* description, int8_t value, uint32_t flags, int alternateVersionID, IndefiniteArticle article )
{
	assert( game );
	assert( game->arrGear );
	assert( id < arrlen( game->arrGear ) );
	assert( id >= 0 );
	game->arrGear[id].name = name;
	game->arrGear[id].description = description;
	game->arrGear[id].value = value;
	game->arrGear[id].flags = flags;
	game->arrGear[id].alternateVersionID = alternateVersionID;
	game->arrGear[id].article = article;
}

// mechanically companions act as changes to stats
void initCompanions( RPGGame* game, int numCompanions, uint32_t companionWithFlag )
{
	assert( game );
	assert( game->arrSkills );
	game->arrCompanions = NULL;
	arrsetlen( game->arrCompanions, numCompanions );
	game->companionWithFlag = companionWithFlag;
	for( int i = 0; i < numCompanions; ++i ) {
		game->arrCompanions[i].name = "UNSET";
		game->arrCompanions[i].description = "UNSET";
		game->arrCompanions[i].arrStatChanges;
	}
}

void addCompanion( RPGGame* game, int id, char* name, char* description, int numStats, ... )
{
	assert( game );
	assert( game->arrCompanions );
	assert( game->arrStats );
	assert( id < arrlen( game->arrCompanions ) );
	assert( id >= 0 );
	assert( numStats == arrlen( game->arrStats ) );
	game->arrCompanions[id].name = name;
	game->arrCompanions[id].description = description;
	game->arrCompanions[id].arrStatChanges = NULL;
	arrsetlen( game->arrCompanions[id].arrStatChanges, numStats );
	va_list va;
	va_start( va, numStats ); {
		for( int i = 0; i < numStats; ++i ) {
			game->arrCompanions[id].arrStatChanges[i] = va_arg( va, int8_t );
		}
	} va_end( va );
}

void initDisciplines( RPGGame* game, int numDisciplines )
{
	assert( game );
	game->arrDisciplines = NULL;
	arrsetlen( game->arrDisciplines, numDisciplines );
	for( int i = 0; i < numDisciplines; ++i ) {
		game->arrDisciplines[i].name = "UNSET";
		game->arrDisciplines[i].description = "UNSET";
		game->arrDisciplines[i].maxLevel = -1;
		game->arrDisciplines[i].arrPassiveChanges = NULL;
		game->arrDisciplines[i].arrAbilityIDs = NULL;
	}
}

void addDiscipline( RPGGame* game, int disciplineID, const char* name, const char* description, int8_t maxLevel, IndefiniteArticle nameArticle )
{
	assert( game );
	assert( game->arrDisciplines );
	assert( disciplineID < arrlen( game->arrDisciplines ) );
	assert( disciplineID >= 0 );
	assert( maxLevel > 0 );
	Discipline* discipline = &( game->arrDisciplines[disciplineID] );
	discipline->name = name;
	discipline->description = description;
	discipline->maxLevel = maxLevel;
	discipline->nameArticle = nameArticle;
	discipline->arrPassiveChanges = NULL;
	discipline->arrAbilityIDs = NULL;
	arrsetlen( discipline->arrAbilityIDs, maxLevel + 1 );
	arrsetlen( discipline->arrPassiveChanges, maxLevel + 1 );
	for( int8_t i = 0; i <= maxLevel; ++i ) {
		discipline->arrAbilityIDs[i] = -1;

		discipline->arrPassiveChanges[i].arrSkillChanges = NULL;
		discipline->arrPassiveChanges[i].arrStatChanges = NULL;

		arrsetlen( discipline->arrPassiveChanges[i].arrSkillChanges, arrlen( game->arrSkills ) );
		memset( discipline->arrPassiveChanges[i].arrSkillChanges, 0, arrlen( discipline->arrPassiveChanges[i].arrSkillChanges ) * sizeof( discipline->arrPassiveChanges[i].arrSkillChanges[0] ) );

		arrsetlen( discipline->arrPassiveChanges[i].arrStatChanges, arrlen( game->arrStats ) );
		memset( discipline->arrPassiveChanges[i].arrStatChanges, 0, arrlen( discipline->arrPassiveChanges[i].arrStatChanges ) * sizeof( discipline->arrPassiveChanges[i].arrStatChanges[0] ) );
	}
}

void addDisciplinePassiveStatChange( RPGGame* game, int disciplineID, int8_t level, int statID, int8_t skillChange )
{
	assert( game );
	assert( game->arrDisciplines );
	assert( disciplineID < arrlen( game->arrDisciplines ) );
	assert( disciplineID >= 0 );
	assert( statID < arrlen( game->arrStats ) );
	assert( statID >= 0 );
	assert( level <= game->arrDisciplines[disciplineID].maxLevel );
	assert( level >= 0 );

	game->arrDisciplines[disciplineID].arrPassiveChanges[level].arrStatChanges[statID] = skillChange;
}

void addDisciplinePassiveSkillChange( RPGGame* game, int disciplineID, int8_t level, int skillID, int8_t skillChange )
{
	assert( game );
	assert( game->arrDisciplines );
	assert( disciplineID < arrlen( game->arrDisciplines ) );
	assert( disciplineID >= 0 );
	assert( skillID < arrlen( game->arrSkills ) );
	assert( skillID >= 0 );
	assert( level <= game->arrDisciplines[disciplineID].maxLevel );
	assert( level >= 0 );

	game->arrDisciplines[disciplineID].arrPassiveChanges[level].arrSkillChanges[skillID] = skillChange;
}

void addDisciplineAbility( RPGGame* game, int disciplineID, int8_t level, int abilityID )
{
	assert( game );
	assert( game->arrDisciplines );
	assert( disciplineID < arrlen( game->arrDisciplines ) );
	assert( disciplineID >= 0 );
	assert( level <= game->arrDisciplines[disciplineID].maxLevel );
	assert( level >= 0 );

	game->arrDisciplines[disciplineID].arrAbilityIDs[level] = abilityID;
}

Character createCharacter( RPGGame* game, void* extraData )
{
	Character c;

	c.arrStats = NULL;
	arrsetlen( c.arrStats, arrlen( game->arrStats ) );
	memset( c.arrStats, 0, sizeof( c.arrStats[0] ) * arrlenu( c.arrStats ) );

	c.arrWounds = NULL;
	arrsetlen( c.arrWounds, arrlen( game->arrStats ) );
	memset( c.arrWounds, 0, sizeof( c.arrWounds[0] ) * arrlenu( c.arrWounds ) );

	c.arrSkills = NULL;
	arrsetlen( c.arrSkills, arrlen( game->arrSkills ) );
	memset( c.arrSkills, 0, sizeof( c.arrSkills[0] ) * arrlenu( c.arrSkills ) );

	c.arrGearCount = NULL;
	arrsetlen( c.arrGearCount, arrlen( game->arrGear ) );
	memset( c.arrGearCount, 0, sizeof( c.arrGearCount[0] ) * arrlenu( c.arrGearCount ) );

	c.arrCompanionFlags = NULL;
	arrsetlen( c.arrCompanionFlags, arrlen( game->arrCompanions ) );
	memset( c.arrCompanionFlags, 0, sizeof( c.arrCompanionFlags[0] ) * arrlenu( c.arrCompanionFlags ) );

	c.arrDisciplineLevels = NULL;
	arrsetlen( c.arrDisciplineLevels, arrlen( game->arrDisciplines ) );
	memset( c.arrDisciplineLevels, 0, sizeof( c.arrDisciplineLevels[0] ) * arrlenu( c.arrDisciplineLevels ) );

	c.flags = 0;
	c.extraData = extraData;

	return c;
}

int carryWeight( RPGGame* game, Character* character )
{
	assert( game->enucumberanceStat != -1 );
	assert( game->enucumberanceSkill != -1 );
	return ( character->arrStats[game->enucumberanceStat] + character->arrSkills[game->enucumberanceSkill] );
}

int totalGearCount( Character* character )
{
	int total = 0;
	for( int i = 0; i < arrlen( character->arrGearCount ); ++i ) {
		total += character->arrGearCount[i];
	}
	return total;
}

int gearSpaceLeft( RPGGame* game, Character* character )
{
	return ( carryWeight( game, character ) - totalGearCount( character ) );
}

void addGear( Character* character, int id )
{
	character->arrGearCount[id] += 1;
}

void removeGear( Character* character, int id )
{
	if( character->arrGearCount[id] > 0 ) {
		character->arrGearCount[id] -= 1;
	}
}

void addFlagToCompanion( Character* character, int companionID, uint32_t flag )
{
	character->arrCompanionFlags[companionID] = turnOnFlag( character->arrCompanionFlags[companionID], flag );
}

void removeFlagFromCompanion( Character* character, int companionID, uint32_t flag )
{
	character->arrCompanionFlags[companionID] = turnOffFlag( character->arrCompanionFlags[companionID], flag );
}

bool isFlagSetOnCompanion( Character* character, int companionID, uint32_t flag )
{
	return isFlagOn( character->arrCompanionFlags[companionID], flag );
}

bool areFlagsSetOnCompanion( Character* character, int companionID, uint32_t flags )
{
	return ( character->arrCompanionFlags[companionID] & flags ) == flags;
}

bool isCompanionWithCharacter( RPGGame* game, Character* character, int companionID )
{
	return isFlagOn( character->arrCompanionFlags[companionID], game->companionWithFlag );
}

int8_t statBoostFromAllCompanions( RPGGame* game, Character* character, int statID )
{
	int8_t boost = 0;
	for( int i = 0; i < arrlen( game->arrCompanions ); ++i ) {
		if( isFlagOn( character->arrCompanionFlags[i], game->companionWithFlag ) ) {
			boost += game->arrCompanions[i].arrStatChanges[statID];
		}
	}
	return boost;
}

int numCompanions( Character* character, uint32_t flag )
{
	int count = 0;
	for( int i = 0; i < arrlen( character->arrCompanionFlags ); ++i ) {
		if( isFlagOn( character->arrCompanionFlags[i], flag ) ) ++count;
	}
	return count;
}

int getFirstCompanion( Character* character, uint32_t flag )
{
	for( int i = 0; i < arrlen( character->arrCompanionFlags ); ++i ) {
		if( isFlagOn( character->arrCompanionFlags[i], flag ) ) return i;
	}
	return -1;
}

int getNextCompanionAfter( Character* character, int companion, uint32_t flag )
{
	for( int i = companion + 1; i < arrlen( character->arrCompanionFlags ); ++i ) {
		if( isFlagOn( character->arrCompanionFlags[i], flag ) ) return i;
	}
	return -1;
}

int getRandomCompanion( Character* character, uint32_t flags )
{
	int c = -1;
	int num = 0;

	for( int i = 0; i < arrlen( character->arrCompanionFlags ); ++i ) {
		if( ( character->arrCompanionFlags[i] & flags ) == flags ) {
			++num;
			if( ( balancedRandom( num ) % num ) == 0 ) {
				c = i;
			}
		}
	}

	return c;
}

int numFlaggedGear( RPGGame* game, Character* character, uint32_t flag )
{
	int found = 0;
	for( int i = 0; i < arrlen( game->arrGear ); ++i ) {
		if( isFlagOn( game->arrGear[i].flags, flag ) && ( character->arrGearCount[i] > 0 ) ) {
			++found;
		}
	}
	return found;
}

int bestFlaggedGear( RPGGame* game, Character* character, uint32_t flag )
{
	int foundGear = -1;
	int8_t foundBonus = INT8_MIN;
	for( int i = 0; i < arrlen( game->arrGear ); ++i ) {
		if( isFlagOn( game->arrGear[i].flags, flag ) && ( character->arrGearCount[i] > 0 ) ) {
			if( ( foundGear == -1 ) || ( game->arrGear[i].value > foundBonus ) ) {
				foundGear = i;
				foundBonus = game->arrGear[i].value;
			}
		}
	}
	return foundGear;
}

int randomOwnedGear( Character* character )
{
	int total = 0;
	for( int i = 0; i < arrlen( character->arrGearCount ); ++i ) {
		total += character->arrGearCount[i];
	}

	int choice = balancedRandom( total );

	int idx = 0;
	while( character->arrGearCount[idx] < choice ) {
		choice -= character->arrGearCount[idx];
		++idx;
	}

	return idx;
}

void takeAndGiveAlternateGear( RPGGame* game, Character* character, int gearID )
{
	if( character->arrGearCount[gearID] <= 0 ) return;

	int alternate = game->arrGear[gearID].alternateVersionID;
	assert( alternate >= 0 && alternate < arrlen( game->arrGear ) );

	character->arrGearCount[gearID] -= 1;
	character->arrGearCount[alternate] += 1;
}

const char* getIndefiniteArticle( IndefiniteArticle ia )
{
	switch( ia ) {
	case IA_CONSONANT:
		return "a ";
	case IA_VOWEL:
		return "an ";
	case IA_UNCOUNTABLE:
		return "some ";
	case IA_PROPER:
		// don't use any article
		return "";
	}
	return "UNKNOWN INDEFINITE ARTICLE";
}

// cost to raise skill by one point
int8_t getSkillCost( RPGGame* game, Character* character, int skillID )
{
	int8_t skill = character->arrSkills[skillID];
	int8_t stat = character->arrStats[game->arrSkills[skillID].associatedStatID];
	return ( skill % stat ) + 1;
}

bool canBuyAnySkills( RPGGame* game, Character* character, uint8_t skillPoints )
{
	for( int i = 0; i < arrlen( character->arrSkills ); ++i ) {
		if( getSkillCost( game, character, i ) <= skillPoints ) return true;
	}
	return false;
}

uint8_t randomlyDistributeSkills( RPGGame* game, Character* character, uint8_t skillPoints )
{
	// randomly weight based on the size of the attribute and how many points have already
	//  been spent, don't go over the size of the die
	while( canBuyAnySkills( game, character, skillPoints ) ) {
		InfiniteListSelector selector;
		infiniteListSelector_Init( &selector, -1 );
		for( int i = 0; i < arrlen( character->arrSkills ); ++i ) {
			if( getSkillCost( game, character, i ) <= skillPoints ) {
				uint32_t diff = ( character->arrStats[game->arrSkills[i].associatedStatID] - character->arrSkills[i] );
				infiniteListSelector_Choose( &selector, i, diff );
			}
		}

		skillPoints -= getSkillCost( game, character, selector.chosenId );
		character->arrSkills[selector.chosenId] += 1;
	}
	return skillPoints;
}

#pragma region Scene Management
typedef struct {
	CLOSURE( void )* closure;
	bool firstTimeRun;
} SceneInstance;
static SceneInstance* arrSceneStack = NULL;

void pushScene( CLOSURE( void )* newScene )
{
	SceneInstance instance = { newScene, true };
	arrpush( arrSceneStack, instance );
	SceneInstance* test = &( arrlast( arrSceneStack ) );
	int x = 0;
}

void popScene( void )
{
	if( arrlenu( arrSceneStack ) > 0 ) {
		SceneInstance popped = arrpop( arrSceneStack );
		mem_Release( popped.closure );
	}
}

void switchScene( CLOSURE( void )* newScene )
{
	popScene( );
	pushScene( newScene );
}

void runCurrentScene( void )
{
	size_t size = arrlenu( arrSceneStack );
	if( size > 0 ) {
		size_t idx = size - 1;
		if( arrSceneStack[idx].closure != NULL ) {
			// func() can cause the scene stack to change, so we'll not want to touch anything on it after we've called it
			bool firstTimeRun = arrSceneStack[idx].firstTimeRun;
			arrSceneStack[idx].firstTimeRun = false;
			arrSceneStack[idx].closure->func( arrSceneStack[idx].closure, firstTimeRun );
		}
	}
}
#pragma endregion

bool disciplineHasAnyBonusForLevel( RPGGame* game, int id, int8_t level )
{
	if( level < 0 ) return false;
	if( ( id < 0 ) || ( id > arrlen( game->arrDisciplines ) ) ) return false;

	Discipline* d = &( game->arrDisciplines[id] );

	if( level >= d->maxLevel ) return false;

	DisciplinePassiveChange* passiveChange = &( d->arrPassiveChanges[level] );
	for( int a = 0; a < arrlen( passiveChange->arrSkillChanges ); ++a ) {
		if( passiveChange->arrSkillChanges[a] != 0 ) return true;
	}
	for( int a = 0; a < arrlen( passiveChange->arrStatChanges ); ++a ) {
		if( passiveChange->arrStatChanges[a] != 0 ) return true;
	}

	if( d->arrAbilityIDs[level] != -1 ) return true;

	return false;
}
int8_t disciplineStatChange( RPGGame* game, Character* character, int disciplineID, int statID )
{
	int8_t boost = 0;
	if( ( disciplineID >= 0 ) && ( disciplineID < arrlen( game->arrDisciplines ) ) ) {
		int8_t level = character->arrDisciplineLevels[disciplineID];
		for( int8_t i = 0; i < level; ++i ) {
			boost += game->arrDisciplines[disciplineID].arrPassiveChanges[level].arrStatChanges[statID];
		}
	}
	return boost;
}

int8_t allDisciplineStatChanges( RPGGame* game, Character* character, int statID )
{
	int8_t boost = 0;
	for( int id = 0; id < arrlen( game->arrDisciplines ); ++id ) {
		boost += disciplineStatChange( game, character, id, statID );
	}
	return boost;
}

int8_t disciplineSkillChange( RPGGame* game, Character* character, int disciplineID, int skillID )
{
	int8_t boost = 0;
	if( ( disciplineID >= 0 ) && ( disciplineID < arrlen( game->arrDisciplines ) ) ) {
		int8_t level = character->arrDisciplineLevels[disciplineID];
		for( int8_t i = 0; i < level; ++i ) {
			boost += game->arrDisciplines[disciplineID].arrPassiveChanges[level].arrSkillChanges[skillID];
		}
	}
	return boost;
}

int8_t allDisciplineSkillChanges( RPGGame* game, Character* character, int skillID )
{
	int8_t boost = 0;
	for( int id = 0; id < arrlen( game->arrDisciplines ); ++id ) {
		boost += disciplineSkillChange( game, character, id, skillID );
	}
	return boost;
}

bool canUseAbility( RPGGame* game, Character* character, int abilityID )
{
	for( int i = 0; i < arrlen( game->arrDisciplines ); ++i ) {
		for( int a = 0; a < character->arrDisciplineLevels[i]; ++a ) {
			if( game->arrDisciplines[i].arrAbilityIDs[a] == abilityID ) {
				return true;
			}
		}
	}
	return false;
}

Result skillCheck( RPGGame* game, Character* character, int skillIdx, uint8_t modifiers, uint8_t difficulty )
{
	uint8_t stat = character->arrStats[game->arrSkills[skillIdx].associatedStatID];

	// gather all the data we need to make the roll
	uint8_t companionBonus = statBoostFromAllCompanions( game, character, stat );
	uint8_t disciplineBonus = allDisciplineStatChanges( game, character, stat ) + allDisciplineSkillChanges( game, character, skillIdx );

	uint8_t skill = character->arrSkills[skillIdx] + companionBonus + disciplineBonus;

	// make the roll
	int roll = simpleRoll( (int)stat );
	int check = ( roll + (int)skill ) + (int)modifiers;

	Result result;
	if( check >= difficulty ) {
		// player gets what they want
		result = R_SUCCESS;
	} else if( check >= ( difficulty / 2 ) ) {
		// player gets what they want but at a cost
		result = R_COSTLY_SUCCESS;
	} else {
		// player doesn't get what they want and pays a cost
		result = R_FAILURE;
	}

	// rolling a 1 reduces the result by one step
	if( ( roll == 1 ) && ( result > R_FAILURE ) ) {
		--result;
	}

	return result;
}

Result complexSkillCheck( RPGGame* game, Character* character, int statIdx, int skillIdx, int8_t modifiers, uint8_t difficulty )
{
	uint8_t stat = character->arrStats[statIdx];
	//uint8_t companionBonus = statBoostFromAllCompanions( game, character, stat );
	int8_t dispStatChanges = allDisciplineStatChanges( game, character, statIdx );
	int8_t dispSkillChanges = allDisciplineSkillChanges( game, character, skillIdx );
	uint8_t disciplineBonus = dispStatChanges + dispSkillChanges;

	uint8_t skill = character->arrSkills[skillIdx] + disciplineBonus;

	// make the roll
	int roll = simpleRoll( (int)stat );
	int check = ( roll + (int)skill ) + (int)modifiers;

	Result result;
	if( check >= difficulty ) {
		// player gets what they want
		result = R_SUCCESS;
	} else if( check >= ( difficulty / 2 ) ) {
		// player gets what they want but at a cost
		result = R_COSTLY_SUCCESS;
	} else {
		// player doesn't get what they want and pays a cost
		result = R_FAILURE;
	}

	// rolling a 1 reduces the result by one step
	if( ( roll == 1 ) && ( result > R_FAILURE ) ) {
		--result;
	}

	return result;
}

#pragma region Choices
static Choice* sbSceneChoices = NULL;

void handleSimpleChoice( SimpleChoice choice )
{
	switchScene( closure_Copy( choice.nextSceneNew ) );
}

void handleSkillBasedChoice( RPGGame* game, Character* character, SkillBasedChoice choice )
{
	Result r = complexSkillCheck( game, character, choice.stat, choice.skill, 0, choice.difficulty );
	switch( r ) {
	case R_SUCCESS:
		switchScene( closure_Copy( choice.successSceneNew ) );
		break;
	case R_COSTLY_SUCCESS:
		switchScene( closure_Copy( choice.costlySuccessSceneNew ) );
		break;
	case R_FAILURE:
		switchScene( closure_Copy( choice.failureSceneNew ) );
		break;
	}
}

void handleChoice( RPGGame* game, Character* character, Choice* choice )
{
	switch( choice->type ) {
	case CT_SIMPLE:
		handleSimpleChoice( choice->simple );
		break;
	case CT_SKILL_BASED:
		handleSkillBasedChoice( game, character, choice->skillBased );
		break;
	}
}

void pushSimpleChoiceFull( const char* description, CLOSURE( void )* nextScene )
{
	Choice c;
	c.type = CT_SIMPLE;
	c.simple.sbDescription = runReplacements( description, CAPTYPE_STANDARD, NULL );
	c.simple.nextSceneNew = nextScene;

	arrput( sbSceneChoices, c );
}

void pushSkillBasedChoiceFull( RPGGame* game, Character* character, const char* description, int skill, int stat, uint8_t difficulty,
	CLOSURE( void )* successScene, CLOSURE( void )* costlySuccessScene, CLOSURE( void )* failureScene )
{
	// filter skill choices by the amount of wounds they have to that statistic
	if( character->arrWounds[stat] >= character->arrStats[stat] ) {
		return;
	}

	char* baseString = runReplacements( description, CAPTYPE_STANDARD, NULL );
	char buffer[64];
	snprintf( buffer, sizeof( buffer ) / sizeof( buffer[0] ), " (%s)", difficultyDescription( difficulty ) );

	Choice c;
	c.type = CT_SKILL_BASED;
	c.skillBased.sbDescription = appendToSBString( baseString, buffer );
	c.skillBased.skill = skill;
	c.skillBased.stat = stat;
	c.skillBased.difficulty = difficulty;
	c.skillBased.successSceneNew = successScene;
	c.skillBased.costlySuccessSceneNew = costlySuccessScene;
	c.skillBased.failureSceneNew = failureScene;

	arrput( sbSceneChoices, c );
}

void clearSceneChoices( )
{
	for( size_t i = 0; i < arrlenu( sbSceneChoices ); ++i ) {
		switch( sbSceneChoices[i].type ) {
		case CT_SIMPLE:
			arrfree( sbSceneChoices[i].simple.sbDescription );
			mem_Release( sbSceneChoices[i].simple.nextSceneNew );
			break;
		case CT_SKILL_BASED:
			arrfree( sbSceneChoices[i].skillBased.sbDescription );
			mem_Release( sbSceneChoices[i].skillBased.successSceneNew );
			mem_Release( sbSceneChoices[i].skillBased.failureSceneNew );
			mem_Release( sbSceneChoices[i].skillBased.costlySuccessSceneNew );
			break;
		}
	}
	arrsetlen( sbSceneChoices, 0 );
}

int getNumSceneChoices( )
{
	return arrlen( sbSceneChoices );
}

Choice* getSceneChoice( int idx )
{
	if( ( idx < 0 ) || ( idx >= getNumSceneChoices( ) ) ) return NULL;
	return &( sbSceneChoices[idx] );
}

#pragma endregion

int increaseRank( int rank )
{
	++rank;
	rank = max( min( rank, NEAR_IMPOSSIBLE_RANK ), VERY_EASY_RANK );
	return rank;
}

int decreaseRank( int rank )
{
	--rank;
	rank = max( min( rank, NEAR_IMPOSSIBLE_RANK ), VERY_EASY_RANK );
	return rank;
}

const char* difficultyDescription( uint8_t difficulty )
{
	const char* strings[] = { "Very Easy", "Easy", "Normal", "Difficult", "Hard", "Very Hard", "Near Impossible" };
	const uint8_t difficultyLevels[] = { VERY_EASY_TEST, EASY_TEST, NORMAL_TEST, DIFFICULT_TEST, HARD_TEST, VERY_HARD_TEST, NEAR_IMPOSSIBLE_TEST };

	// find which ever is closest
	int closest = 0;
	int closestDist = INT_MAX;
	for( int i = 0; i < sizeof( difficultyLevels) / sizeof( difficultyLevels[0] ); ++i ) {
		int diff = abs( (int)difficulty - (int)difficultyLevels[i] );

		if( diff < closestDist ) {
			closestDist = diff;
			closest = i;
		}
	}

	return strings[closest];
}