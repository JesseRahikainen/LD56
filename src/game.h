#ifndef GAME_H
#define GAME_H

#include "window.h"
#include "rpg.h"

typedef enum {
	INVALID_STAT = -1,
	STAT_SELF,
	STAT_SPOUSE,
	STAT_CHILD,
	NUM_STATS
} Stats;

// the skills will be hidden from the player
typedef enum {
	INVALID_SKILL = -1,

	SKL_CROPS,
	SKL_ANIMALS,
	SKL_FARMHOUSE,
	SKL_FAMILY,

	NUM_SKILLS
} Skills;

// we'll use companions as the fae that are on the farm
typedef enum {
	INVALID_COMPANION = -1,

	CMP_HOUSE_HOB,
	CMP_FAIRY,
	CMP_DRYAD,
	CMP_BABY_SITTER,
	CMP_BUILDER,
	CMP_WATER,
	CMP_HUNGRY,
	//CMP_CANNIBAL,

	NUM_COMPANIONS
} Companions;

typedef enum {
	CF_THERE,
	CF_APPEASED,
	CF_ANGERED,
	CF_ALWAYS_ANGERED
} CompanionFlags;

typedef enum {
	FS_CROPS,
	FS_ANIMALS,
	FS_FARMHOUSE,
	FS_FAMILY,
	NUM_FARM_STATUS
} FarmStatus;

typedef enum {
	CF_IS_AFTERNOON,
	CF_DIRTY_WELL,
	CF_READ_BOOK,
	CF_PERFORMED_FAE_ACTION,
	CF_SEEN_BODY_IN_WELL,
	CF_CUT_TREES,
	CF_SEEN_WELL,
} CharacterFlags;

typedef enum {
	LOC_FARMHOUSE,
	LOC_BARN,
	LOC_FIELD,
	LOC_WOODS,
	LOC_WELL,
	LOC_WITH_FAMILY,
	LOC_CLEANING,
	NUM_LOCATIONS
} Locations;

typedef struct {
	int8_t currentDay;

	// days for all the fae related events
	int8_t cannibalDoorDay;
	int8_t cannibalKidnapDay;

	int8_t hungryManDay;
	int8_t wellDay;
	int8_t treeDay;
	int8_t prankDay;

	// the Derbshiur will consider you as being derilict in your duties if you don't have family night at least every other day
	int8_t daysWithoutFamilyInteraction;

	// the Timthirid will consider you being derilict in your duties if you haven't placed out milk at least every three days
	int8_t daysWithoutHouseMilk;

	FarmStatus spouseBusyWith;
	FarmStatus childBusyWith;

	Locations currentLocation;
} GameCharacterData;

extern Window* window;
extern RPGGame rpgGame;
extern Character character;
extern GameCharacterData gameCharacterData;

#define MAX_SPIRIT_BIND_SPOTS 6
#define BASE_SKILL_POINTS 64
#define MIN_SKILL_POINTS 48
#define TOTAL_TIME_ALLOWED 40

void setupCompanionData( void );
void setupGearData( void );
void setupStatsData( void );
void setupSkillsData( void );
void setupDisciplines( void );
void setupGame( );

void initCharacter( );

bool isDead( );
bool isDeadFromStat( Stats stat );
bool isDeadFromFarmStatus( FarmStatus status );
bool isWounded( );
Stats getRandomWound( );

void startPlayDraw( void );
void startScene( );
bool testSharedInput( Input input );
void standardSceneChoice( bool allowDefaults );
void clearSceneDisplayString( );
void appendToSceneDisplayString( const char* str );
void setInitialSceneDescriptionString( const char* str );
char* createCopyOfSceneDisplayString( );
void drawPlayScreen( void );
Coord drawMainScreenStatString( Stats stat, Window* screen, int16_t x, int16_t y, SmallRect border );
Coord drawMainScreenSkillString( Skills skill, Window* screen, int16_t x, int16_t y, SmallRect border );
void defaultSceneLoop( );

void pushSimpleChoice( const char* description, CLOSURE( void )* nextScene );
void pushSkillBasedChoice( const char* description, int skill, int stat, uint8_t difficulty,
	CLOSURE( void )* successScene, CLOSURE( void )* costlySuccessScene, CLOSURE( void )* failureScene );

// scene related functions, does something and appends an appropriate string to the scene display string
void sceneInflictWound( Stats stat, const char* woundDescription );
void sceneHealWound( Stats stat );
void sceneIncreaseDiscipline( FarmStatus status );
void sceneDecreaseDiscipline( FarmStatus status );

#endif // inclusion guard
