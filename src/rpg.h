/*
* General things:
* Difficulty test - you have a die going from a d4 to a d12, with d8 being the median, skills will range from 0 to n, lets assume
*  that on average it will be 1/2 die size, so we only have to worry about balancing around d4+2, d6+3, d8+4, d10+5, and d12+6
*   Very Easy (d4+2) - 8
*   Easy (d6+3) - 10
*   Normal (d8+4) - 12
*   Difficult (d10+5) - 14
*   Hard (d12+6) - 16
*   Very Hard - 20
*   Near Impossible - 24

Test 2	d4	d6	d8	d10	d12			Test 3	d4	d6	d8	d10	d12			Test 4	d4	d6	d8	d10	d12			Test 5	d4	d6	d8	d10	d12			Test 6	d4	d6	d8	d10	d12
costly	100	100	100	100	100			costly	100	100	100	100	100			costly	100	100	100	100	100			costly	100	100	100	100	100			costly	100	100	100	100	100
total	100	100	100	100	100			total	100	100	100	100	100			total	75	100	100	100	100			total	50	83	100	100	100			total	25	66	87	100	100

Test 7	d4	d6	d8	d10	d12			Test 8	d4	d6	d8	d10	d12			Test 9	d4	d6	d8	d10	d12			Test 10	d4	d6	d8	d10	d12			Test 11	d4	d6	d8	d10	d12
costly	100	100	100	100	100			costly	75	100	100	100	100			costly	75	100	100	100	100			costly	50	83	100	100	100			costly	50	83	100	100	100
total	0	50	75	90	100			total	0	33	62	80	91			total	0	16	50	70	83			total	0	0	37	60	75			total	0	0	25	50	66

Test 12	d4	d6	d8	d10	d12			Test 13	d4	d6	d8	d10	d12			Test 14	d4	d6	d8	d10	d12			Test 15	d4	d6	d8	d10	d12			Test 16	d4	d6	d8	d10	d12
costly	25	66	87	100	100			costly	25	66	87	100	100			costly	0	50	75	90	100			costly	0	50	75	90	100			costly	0	33	62	80	91
total	0	0	12	40	58			total	0	0	0	30	50			total	0	0	0	20	41			total	0	0	0	10	33			total	0	0	0	0	25

Test 17	d4	d6	d8	d10	d12			Test 18	d4	d6	d8	d10	d12			Test 19	d4	d6	d8	d10	d12			Test 20	d4	d6	d8	d10	d12			Test 21	d4	d6	d8	d10	d12
costly	0	33	62	80	91			costly	0	16	50	70	83			costly	0	16	50	70	83			costly	0	0	37	60	75			costly	0	0	37	60	75
total	0	0	0	0	16			total	0	0	0	0	8			total	0	0	0	0	0			total	0	0	0	0	0			total	0	0	0	0	0

Test 22	d4	d6	d8	d10	d12			Test 23	d4	d6	d8	d10	d12			Test 24	d4	d6	d8	d10	d12			Test 25	d4	d6	d8	d10	d12			Test 26	d4	d6	d8	d10	d12
costly	0	0	25	50	66			costly	0	0	25	50	66			costly	0	0	12	40	58			costly	0	0	12	40	58			costly	0	0	0	30	50
total	0	0	0	0	0			total	0	0	0	0	0			total	0	0	0	0	0			total	0	0	0	0	0			total	0	0	0	0	0

Test 27	d4	d6	d8	d10	d12			Test 28	d4	d6	d8	d10	d12			Test 29	d4	d6	d8	d10	d12			Test 30	d4	d6	d8	d10	d12
costly	0	0	0	30	50			costly	0	0	0	20	41			costly	0	0	0	20	41			costly	0	0	0	10	33
total	0	0	0	0	0			total	0	0	0	0	0			total	0	0	0	0	0			total	0	0	0	0	0

Generally we should follow a fail-forward style game play. Even if you fail you can still continue onward and just take some losses. There may be some combats where failure
locks you into a loop until you succeed or die (have to make sure death is an option there). Generally failure should make future encounters harder, making it more likely
that you'll die, or have to use up some resource (like spending time to rest when you're on a countdown). But because we're not able to adapt as well as an in person GM would
we have to always give them a way forward, and generally that means you take a hit but you can continue. This is easier in outdoor scenarios as you can just say you find a
way around it eventually (e.g. spending a few days to get to a bridge to cross over the river). This reduces the number of scenes we'll need to write as we don't have to
worry about every single failure scenario having a back up way to get around it, and won't bore the player with "You failed. Try again?".
If we do any sort of sidequests however, those should be able to be failed completely. The fail-forward is only needed for the main quests. Unless we're able to make almost
the entire game sidequests.
*/

#ifndef RPG_H
#define RPG_H

#include <stdbool.h>
#include <stdint.h>

#include "utils.h"

#define VERY_EASY_TEST 8
#define EASY_TEST 10
#define NORMAL_TEST 12
#define DIFFICULT_TEST 14
#define HARD_TEST 16
#define VERY_HARD_TEST 20
#define NEAR_IMPOSSIBLE_TEST 24

extern uint8_t rankedDifficulties[];// = { VERY_EASY_TEST, EASY_TEST, NORMAL_TEST, DIFFICULT_TEST, HARD_TEST, VERY_HARD_TEST, NEAR_IMPOSSIBLE_TEST };

#define VERY_EASY_RANK 0
#define EASY_RANK 1
#define NORMAL_RANK 2
#define DIFFICULT_RANK 3
#define HARD_RANK 4
#define VERY_HARD_RANK 5
#define NEAR_IMPOSSIBLE_RANK 6

int increaseRank( int rank );
int decreaseRank( int rank );
const char* difficultyDescription( uint8_t difficulty );

typedef enum {
	R_FAILURE,
	R_COSTLY_SUCCESS,
	R_SUCCESS
} Result;

typedef enum {
	IA_CONSONANT,
	IA_VOWEL,
	IA_UNCOUNTABLE,
	IA_PROPER, // don't use an article
} IndefiniteArticle;

typedef struct {
	const char* name;
	const char* description;
	const char* abbreviation;
	const char* subjectPronoun;
	const char* objectPronoun;
	const char* possessivePronoun;
} StatData;

typedef struct {
	int associatedStatID;
	const char* name;
	const char* description;
	const char* abbreviation;
} SkillData;

typedef struct {
	const char* name;
	const char* description;
	int8_t value; // value used for various things based on the type of gear
	uint32_t flags;
	int alternateVersionID;
	IndefiniteArticle article;
} GearData;

typedef struct {
	const char* name;
	const char* description;
	int8_t* arrStatChanges;
} CompanionData;

// disciplines have to be initialized after skills and stats
typedef struct {
	int8_t* arrStatChanges;
	int8_t* arrSkillChanges;
} DisciplinePassiveChange;

typedef struct {
	const char* name;
	const char* description;
	int8_t maxLevel;
	DisciplinePassiveChange* arrPassiveChanges;
	int* arrAbilityIDs;
	IndefiniteArticle nameArticle;
} Discipline;

typedef struct {
	int8_t* arrStats;
	int8_t* arrWounds;
	int8_t* arrSkills;
	uint8_t* arrGearCount;
	uint32_t* arrCompanionFlags;
	int8_t* arrDisciplineLevels;
	uint32_t flags;
	void* extraData;
} Character;

typedef struct {
	StatData* arrStats;
	SkillData* arrSkills;
	GearData* arrGear;
	CompanionData* arrCompanions;
	Discipline* arrDisciplines;

	int enucumberanceStat; // id of stat associated with how much Gear a character can carry
	int enucumberanceSkill; // id of skill associated with how much Gear a character can carry

	uint32_t companionWithFlag; // if this flag is on for the companion it will count as accompanying the character
} RPGGame;

int rollBetween( int min, int max );
int simpleRoll( int stat );

void initGame( RPGGame* game );

// stats are the base attributes players have, these act as dice rolls that provide the base of any action
void initStats( RPGGame* game, int numStats );
void addStat( RPGGame* game, int id, const char* name, const char* description, const char* abbreviation );

// skills are constant parts of any action test, when testing a skill you roll the stat associated with the skill then add the value for the skill
void initSkills( RPGGame* game, int numSkills );
void addSkill( RPGGame* game, int id, int associatedStatID, const char* name, const char* description, const char* abbreviation );

// gear are items that take up space
void initGearTypes( RPGGame* game, int numGear );
void addGearType( RPGGame* game, int id, char* name, const char* description, int8_t value, uint32_t flags, int alternateVersionID, IndefiniteArticle article );

// mechanically companions act as changes to stats
void initCompanions( RPGGame* game, int numCompanions, uint32_t companionWithFlag );
void addCompanion( RPGGame* game, int id, char* name, char* description, int numStats, ... );

void initDisciplines( RPGGame* game, int numDisciplines );
void addDiscipline( RPGGame* game, int disciplineID, const char* name, const char* description, int8_t maxLevel, IndefiniteArticle nameArticle );
void addDisciplinePassiveStatChange( RPGGame* game, int disciplineID, int8_t level, int statID, int8_t skillChange );
void addDisciplinePassiveSkillChange( RPGGame* game, int disciplineID, int8_t level, int skillID, int8_t skillChange );
void addDisciplineAbility( RPGGame* game, int disciplineID, int8_t level, int abilityID );

// character
Character createCharacter( RPGGame* game, void* extraData );

// gear
int carryWeight( RPGGame* game, Character* character );
int totalGearCount( Character* character );
int gearSpaceLeft( RPGGame* game, Character* character );
void addGear( Character* character, int id );
void removeGear( Character* character, int id );
int numFlaggedGear( RPGGame* game, Character* character, uint32_t flag );
int bestFlaggedGear( RPGGame* game, Character* character, uint32_t flag );
int randomOwnedGear( Character* character );
void takeAndGiveAlternateGear( RPGGame* game, Character* character, int gearID );

// companions
void addFlagToCompanion( Character* character, int companionID, uint32_t flag );
void removeFlagFromCompanion( Character* character, int companionID, uint32_t flag );
bool isFlagSetOnCompanion( Character* character, int companionID, uint32_t flag );
bool areFlagsSetOnCompanion( Character* character, int companionID, uint32_t flags );
bool isCompanionWithCharacter( RPGGame* game, Character* character, int companionID );
int8_t statBoostFromAllCompanions( RPGGame* game, Character* character, int statID );
int numCompanions( Character* character, uint32_t flag );
int getFirstCompanion( Character* character, uint32_t flag );
int getNextCompanionAfter( Character* character, int companion, uint32_t flag );
int getRandomCompanion( Character* character, uint32_t flag );

const char* getIndefiniteArticle( IndefiniteArticle ia );

// skills
int8_t getSkillCost( RPGGame* game, Character* character, int skillID ); // cost to raise skill by one point
bool canBuyAnySkills( RPGGame* game, Character* character, uint8_t skillPoints );
uint8_t randomlyDistributeSkills( RPGGame* game, Character* character, uint8_t skillPoints );

// disciplines
bool disciplineHasAnyBonusForLevel( RPGGame* game, int id, int8_t level );
int8_t disciplineStatChange( RPGGame* game, Character* character, int disciplineID, int statID );
int8_t allDisciplineStatChanges( RPGGame* game, Character* character, int statID );
int8_t disciplineSkillChange( RPGGame* game, Character* character, int disciplineID, int skillID );
int8_t allDisciplineSkillChanges( RPGGame* game, Character* character, int skillID );
bool canUseAbility( RPGGame* game, Character* character, int abilityID );

Result skillCheck( RPGGame* game, Character* character, int skillIdx, uint8_t modifiers, uint8_t difficulty );
Result complexSkillCheck( RPGGame* game, Character* character, int statIdx, int skillIdx, int8_t modifiers, uint8_t difficulty );

#pragma region Scene Management
void pushScene( CLOSURE( void )* newScene );
void popScene( void );
void switchScene( CLOSURE( void )* newScene );
void runCurrentScene( void );
#pragma endregion

#pragma region Choices
typedef enum {
	CT_SKILL_BASED,
	CT_SIMPLE,
	NUM_CHOICE_TYPES
} ChoiceType;

typedef struct {
	ChoiceType type;
	char* sbDescription;

	CLOSURE( void )* failureSceneNew;
	CLOSURE( void )* successSceneNew;
	CLOSURE( void )* costlySuccessSceneNew;

	int skill;
	int stat;
	uint8_t difficulty;
} SkillBasedChoice;

typedef struct {
	ChoiceType type;
	char* sbDescription;

	CLOSURE( void )* nextSceneNew;
} SimpleChoice;

typedef struct {
	ChoiceType type;
	char* description;
} BaseChoice;

typedef union {
	ChoiceType type;

	BaseChoice base;
	SkillBasedChoice skillBased;
	SimpleChoice simple;
} Choice;

void handleChoice( RPGGame* game, Character* character, Choice* choice );
void pushSimpleChoiceFull( const char* description, CLOSURE( void )* nextScene );
void pushSkillBasedChoiceFull( RPGGame* game, Character* character, const char* description, int skill, int stat, uint8_t difficulty,
	CLOSURE( void )* successScene, CLOSURE( void )* costlySuccessScene, CLOSURE( void )* failureScene );
void clearSceneChoices( );
int getNumSceneChoices( );
Choice* getSceneChoice( int idx );
#pragma endregion


#endif // inclusion guard