#include "baseScenes.h"

#include <stdio.h>
#include <assert.h>

#include "stringReplacement.h"
#include "game.h"
#include "mainScenes.h"

static char* arrReportString = NULL;

void youAnimalsScene( CLOSURE( void )* closure, bool firstTimeRun );
void barnScene( CLOSURE( void )* closure, bool firstTimeRun );
void switchToAfternoon( CLOSURE( void )* closure, bool firstTimeRun );
void switchToNextPartOfDay( CLOSURE( void )* closure, bool firstTimeRun );
void cropsScene( CLOSURE( void )* closure, bool firstTimeRun );
void familyScene( CLOSURE( void )* closure, bool firstTimeRun );
void farmHouseScene( CLOSURE( void )* closure, bool firstTimeRun );
void wellIntro( CLOSURE( void )* closure, bool firstTimeRun );
void treeIntroScene( CLOSURE( void )* closure, bool firstTimeRun );

int countAppeasedFae( )
{
	int count = 0;
	for( int i = 0; i < NUM_COMPANIONS; ++i ) {
		if( isCompanionWithCharacter( &rpgGame, &character, i ) && isFlagSetOnCompanion( &character, i, CF_APPEASED ) ) {
			++count;
		}
	}

	return count;
}

Companions getRandomAppeasedFae( )
{
	return getRandomCompanion( &character, createFlagSet( 2, CF_THERE, CF_APPEASED ) );
}

static void postFaeActionScene( CLOSURE( void )* closure, bool firstTimeRun )
{
	// return to the scene that happens before the action took place
	switch( gameCharacterData.currentLocation ) {
	case LOC_FARMHOUSE:
		if( isFlagOn( character.flags, CF_IS_AFTERNOON ) ) {
			switchScene( closure_Make( afternoonScene, 0 ) );
		} else {
			switchScene( closure_Make( morningScene, 0 ) );
		}
		break;
	case LOC_BARN:
		switchScene( closure_Make( barnScene, 0 ) );
		break;
	case LOC_FIELD:
		switchScene( closure_Make( cropsScene, 0 ) );
		break;
	case LOC_WOODS:
		switchScene( closure_Make( treeIntroScene, 0 ) );
		break;
	case LOC_WELL:
		switchScene( closure_Make( wellIntro, 0 ) );
		break;
	case LOC_WITH_FAMILY:
		switchScene( closure_Make( familyScene, 0 ) );
		break;
	case LOC_CLEANING:
		switchScene( closure_Make( farmHouseScene, 0 ) );
		break;
	default:
		assert( false );
		break;
	}
}

static void knockOnWoodScene( CLOSURE( void )* closure, bool firstTimeRun )
{
	if( firstTimeRun ) {
		character.flags = turnOnFlag( character.flags, CF_PERFORMED_FAE_ACTION );

		switch( gameCharacterData.currentLocation ) {
		case LOC_FARMHOUSE:
		case LOC_WITH_FAMILY:
		case LOC_CLEANING:
			setInitialSceneDescriptionString( "You knock on the table three times." );
			break;
		case LOC_BARN:
			setInitialSceneDescriptionString( "You knock on the frame of the door three times." );
			break;
		case LOC_FIELD:
			setInitialSceneDescriptionString( "You knock on the handle of your hoe three times." );
			break;
		case LOC_WOODS:
			setInitialSceneDescriptionString( "You go to each tree and knock on it three times." );
			if( isFlagSetOnCompanion( &character, CMP_DRYAD, CF_THERE ) ) {
				appendToSceneDisplayString( " One of them knocks back." );
				addFlagToCompanion( &character, CMP_DRYAD, CF_APPEASED );
			}
			break;
		case LOC_WELL:
			setInitialSceneDescriptionString( "You knock on the hood of the well three times." );
			break;
		default:
			setInitialSceneDescriptionString( "You find a nearby piece of wood and knock on it three times." );
			break;
		}
	}

	startScene( );

	pushSimpleChoice( "Continue with what you were doing before.", closure_Make( postFaeActionScene, 0 ) );

	defaultSceneLoop( );
}

static void leaveMilkScene( CLOSURE( void )* closure, bool firstTimeRun )
{
	if( firstTimeRun ) {
		character.flags = turnOnFlag( character.flags, CF_PERFORMED_FAE_ACTION );

		// the hob likes this
		if( gameCharacterData.currentLocation == LOC_FARMHOUSE ) {
			gameCharacterData.daysWithoutHouseMilk = 0;
		}

		setInitialSceneDescriptionString( "You prepare a small bowl of milk and leave it on the ground." );

	}

	startScene( );

	pushSimpleChoice( "Continue with what you were doing before.", closure_Make( postFaeActionScene, 0 ) );

	defaultSceneLoop( );
}

static void leaveSaltScene( CLOSURE( void )* closure, bool firstTimeRun )
{
	if( firstTimeRun ) {
		character.flags = turnOnFlag( character.flags, CF_PERFORMED_FAE_ACTION );

		// the hob hates this and leaves
		if( gameCharacterData.currentLocation == LOC_FARMHOUSE ) {
			removeFlagFromCompanion( &character, CMP_HOUSE_HOB, CF_THERE );
		}

		setInitialSceneDescriptionString( "You prepare a small bowl of milk and leave it on the ground." );

	}

	startScene( );

	pushSimpleChoice( "Continue with what you were doing before.", closure_Make( postFaeActionScene, 0 ) );

	defaultSceneLoop( );
}

static void useIronNailsScene( CLOSURE( void )* closure, bool firstTimeRun )
{
	if( firstTimeRun ) {
		character.flags = turnOnFlag( character.flags, CF_PERFORMED_FAE_ACTION );

		switch( gameCharacterData.currentLocation ) {
		case LOC_FARMHOUSE:
		case LOC_WITH_FAMILY:
		case LOC_CLEANING:
			// the fairy hates this and leaves
			removeFlagFromCompanion( &character, CMP_FAIRY, CF_THERE );
			setInitialSceneDescriptionString( "You hang some iron nails from the ceiling." );
			break;
		case LOC_BARN:
			setInitialSceneDescriptionString( "You pound three nails into the frame of the farm door." );
			break;
		case LOC_FIELD:
			setInitialSceneDescriptionString( "You put a nail tip first into the ground and stomp down on it." );
			break;
		case LOC_WOODS:
			setInitialSceneDescriptionString( "You pound a nail into all the trees." );
			break;
		case LOC_WELL:
			setInitialSceneDescriptionString( "You toss a handful of nails into the well." );
			break;
		default:
			setInitialSceneDescriptionString( "You find a place to hide an iron nail." );
			break;
		}
	}

	startScene( );

	pushSimpleChoice( "Continue with what you were doing before.", closure_Make( postFaeActionScene, 0 ) );

	defaultSceneLoop( );
}

static void addressRespectfullyScene( CLOSURE( void )* closure, bool firstTimeRun )
{
	if( firstTimeRun ) {
		character.flags = turnOnFlag( character.flags, CF_PERFORMED_FAE_ACTION );

		switch( gameCharacterData.currentLocation ) {
		case LOC_FARMHOUSE:
		case LOC_WITH_FAMILY:
		case LOC_CLEANING:
			// the hob hates this and will start ignoring the milk
			removeFlagFromCompanion( &character, CMP_HOUSE_HOB, CF_APPEASED );
			addFlagToCompanion( &character, CMP_HOUSE_HOB, CF_ANGERED );
			addFlagToCompanion( &character, CMP_HOUSE_HOB, CF_ALWAYS_ANGERED );
			setInitialSceneDescriptionString( "You take your hat off and thank whatever is listening for all the help given to you." );
			break;
		case LOC_BARN:
			// the builder likes this
			addFlagToCompanion( &character, CMP_BUILDER, CF_APPEASED );
			setInitialSceneDescriptionString( "You take off your hat and ask whatever is listening for permission to enter the barn." );
			break;
		case LOC_FIELD:
			setInitialSceneDescriptionString( "You take your hat off and thank whatever is listening for your good crops." );
			break;
		case LOC_WOODS:
			setInitialSceneDescriptionString( "You take off your hat and warn whatever that is listening that you're going to chop down these trees." );
			break;
		default:
			setInitialSceneDescriptionString( "You take your hat off and thank whatever is listening for all the help given to you." );
			break;
		}
		appendToSceneDisplayString( " You then put your hat back on." );
	}

	startScene( );

	pushSimpleChoice( "Continue with what you were doing before.", closure_Make( postFaeActionScene, 0 ) );

	defaultSceneLoop( );
}

static void sayPrayerScene( CLOSURE( void )* closure, bool firstTimeRun )
{
	if( firstTimeRun ) {
		character.flags = turnOnFlag( character.flags, CF_PERFORMED_FAE_ACTION );

		if( gameCharacterData.currentLocation == LOC_WELL ) {
			// the well sprite hates this and will leave
			removeFlagFromCompanion( &character, CMP_WATER, CF_THERE );
		}
		setInitialSceneDescriptionString( "You take your hat off and repeat a short prayer." );
		appendToSceneDisplayString( " You then put your hat back on." );
	}

	startScene( );

	pushSimpleChoice( "Continue with what you were doing before.", closure_Make( postFaeActionScene, 0 ) );

	defaultSceneLoop( );
}

static void tossCoinScene( CLOSURE( void )* closure, bool firstTimeRun )
{
	if( firstTimeRun ) {
		character.flags = turnOnFlag( character.flags, CF_PERFORMED_FAE_ACTION );

		switch( gameCharacterData.currentLocation ) {
		case LOC_FARMHOUSE:
		case LOC_WITH_FAMILY:
		case LOC_CLEANING:
			setInitialSceneDescriptionString( "You take a coin and toss it behind your cupboard." );
			break;
		case LOC_BARN:
			setInitialSceneDescriptionString( "You take a coin and toss it into the muck." );
			break;
		case LOC_FIELD:
			setInitialSceneDescriptionString( "You take a coin and toss it into the field." );
			break;
		case LOC_WOODS:
			setInitialSceneDescriptionString( "You take a coin and throw it as deep into the woods as you can." );
			break;
		case LOC_WELL:
			setInitialSceneDescriptionString( "You take a coin and toss it into the well." );
			// well sprite loves this
			if( isCompanionWithCharacter( &rpgGame, &character, CMP_WATER ) && !isFlagSetOnCompanion( &character, CMP_WATER, CF_APPEASED ) ) {
				appendToSceneDisplayString( " The water starts smelling better." );
				addFlagToCompanion( &character, CMP_WATER, CF_APPEASED );
				character.flags = turnOffFlag( character.flags, CF_DIRTY_WELL );
			}
			break;
		default:
			setInitialSceneDescriptionString( "You take a coin and throw it as far as you can." );
			break;
		}
	}

	startScene( );

	pushSimpleChoice( "Continue with what you were doing before.", closure_Make( postFaeActionScene, 0 ) );

	defaultSceneLoop( );
}

static void addFaeOption( const char* description, void (*func)( ) )
{
	CLOSURE( void )* closure = closure_Make( func, 0 );
	pushSimpleChoice( description, closure );
}

static void addFaeActionOptions( Locations where )
{
	gameCharacterData.currentLocation = where;
	if( isFlagOn( character.flags, CF_READ_BOOK ) && !isFlagOn( character.flags, CF_PERFORMED_FAE_ACTION ) ) {
	//if( !isFlagOn( character.flags, CF_PERFORMED_FAE_ACTION ) ) {
		addFaeOption( "Knock on wood.", knockOnWoodScene );
		addFaeOption( "Leave some milk.", leaveMilkScene );
		addFaeOption( "Leave some salt.", leaveSaltScene );
		addFaeOption( "Use iron nails.", useIronNailsScene );
		addFaeOption( "Address whatever is there respectfully.", addressRespectfullyScene );
		addFaeOption( "Say a prayer.", sayPrayerScene );
		addFaeOption( "Toss a coin.", tossCoinScene );
	}
}

static void checkForFarmStatusDecay( FarmStatus status, Companions associatedFae )
{
	const int BASE_CHANCE = 80;
	const int APPEASED_CHANCE = 70;

	bool appeased = areFlagsSetOnCompanion( &character, associatedFae, createFlagSet( 2, CF_THERE, CF_APPEASED ) );
	appeased = appeased || areFlagsSetOnCompanion( &character, CMP_HUNGRY, createFlagSet( 2, CF_THERE, CF_APPEASED ) ); // blesses your farm if you help

	int chance = appeased ? APPEASED_CHANCE : BASE_CHANCE;
	if( ( balancedRandom( 100 ) + 1 ) <= chance ) {
		character.arrDisciplineLevels[status] -= 1;
	}
}

uint8_t getAnimalsDifficulty( )
{
	// if the builder is angry any attempt to work with the animals is more difficult, otherwise it becomes easier, the fairy can also cause troubles out here
	bool builderTroubles = isFlagSetOnCompanion( &character, CMP_BUILDER, CF_THERE ) && !isFlagSetOnCompanion( &character, CMP_BUILDER, CF_APPEASED );
	bool builderHelp = isFlagSetOnCompanion( &character, CMP_BUILDER, CF_THERE ) && isFlagSetOnCompanion( &character, CMP_BUILDER, CF_APPEASED );
	bool fairyTroubles = isFlagSetOnCompanion( &character, CMP_FAIRY, CF_THERE ) && isFlagSetOnCompanion( &character, CMP_FAIRY, CF_ANGERED );

	int baseRank = NORMAL_RANK;
	if( builderTroubles ) baseRank = increaseRank( baseRank );
	if( builderHelp ) baseRank = decreaseRank( baseRank );
	if( fairyTroubles ) baseRank = increaseRank( baseRank );

	return rankedDifficulties[baseRank];
}

uint8_t getFarmHouseDifficulty( )
{
	// two can effect this, the hob and the fairy, the hob can make this easier or harder, the fairy can only make it harder
	bool hobTroubles = isFlagSetOnCompanion( &character, CMP_HOUSE_HOB, CF_THERE ) && isFlagSetOnCompanion( &character, CMP_HOUSE_HOB, CF_ANGERED );
	bool hobHelp = isFlagSetOnCompanion( &character, CMP_HOUSE_HOB, CF_THERE ) && isFlagSetOnCompanion( &character, CMP_HOUSE_HOB, CF_APPEASED );
	bool fairyTroubles = isFlagSetOnCompanion( &character, CMP_FAIRY, CF_THERE ) && isFlagSetOnCompanion( &character, CMP_FAIRY, CF_ANGERED );

	int baseRank = NORMAL_RANK;
	if( hobTroubles ) baseRank = increaseRank( baseRank );
	if( fairyTroubles ) baseRank = increaseRank( baseRank );
	if( hobHelp ) baseRank = decreaseRank( baseRank );

	return rankedDifficulties[baseRank];
}

uint8_t getCropsDifficulty( )
{
	// the dryad can make this easier if they're appeased
	bool dryadHelp = isFlagSetOnCompanion( &character, CMP_DRYAD, CF_THERE ) && isFlagSetOnCompanion( &character, CMP_DRYAD, CF_APPEASED );

	int baseRank = NORMAL_RANK;
	if( dryadHelp ) baseRank = decreaseRank( baseRank );

	return rankedDifficulties[baseRank];
}

uint8_t getFamilyDifficulty( int baseRank )
{
	// the baby sitter can help here, the fairy can hinder if angered
	bool sitterHelp = isFlagSetOnCompanion( &character, CMP_BABY_SITTER, CF_THERE ) && isFlagSetOnCompanion( &character, CMP_BABY_SITTER, CF_APPEASED );
	bool fairyTroubles = isFlagSetOnCompanion( &character, CMP_FAIRY, CF_THERE ) && isFlagSetOnCompanion( &character, CMP_FAIRY, CF_ANGERED );

	if( fairyTroubles ) baseRank = increaseRank( baseRank );
	if( sitterHelp && ( baseRank > 0 ) ) baseRank = decreaseRank( baseRank );

	return rankedDifficulties[baseRank];
}

void animalCareSuccess( CLOSURE( void )* closure, bool firstTimeRun )
{
	if( firstTimeRun ) {
		bool builderTroubles = isFlagSetOnCompanion( &character, CMP_BUILDER, CF_THERE ) && !isFlagSetOnCompanion( &character, CMP_BUILDER, CF_APPEASED );
		bool builderHelp = isFlagSetOnCompanion( &character, CMP_BUILDER, CF_THERE ) && isFlagSetOnCompanion( &character, CMP_BUILDER, CF_APPEASED );
		bool fairyTroubles = isFlagSetOnCompanion( &character, CMP_FAIRY, CF_THERE ) && isFlagSetOnCompanion( &character, CMP_FAIRY, CF_ANGERED );

		addReplacementString( "HAIR_EXTRAS", "" );
		addReplacementString( "BUILDER_EXTRAS", "" );

		if( fairyTroubles ) {
			// they'll knot hairs
			if( balancedRandom( 3 ) == 0 ) {
				addReplacementString( "HAIR_EXTRAS", "it takes a bit longer as their hair seems to have been tied into knots, " );
			}
		}

		if( builderTroubles ) {
			if( balancedRandom( 3 ) == 0 ) {
				addReplacementString( "BUILDER_EXTRAS", "You nearly step on a plank of wood with rusty nails sticking up from it. " );
			}
		} else if( builderHelp ) {
			if( balancedRandom( 3 ) == 0 ) {
				addReplacementString( "BUILDER_EXTRAS", "You go to repair one of the windows you thought had been about to fall off, but it's in good condition. " );
			}
		}

		setInitialSceneDescriptionString( "You get in the barn and greet the animals. Quickly you brush the horses and cows, [HAIR_EXTRAS]then let them out to graze. "
			"[BUILDER_EXTRAS]Then you turn your attention to the chickens. You gather their eggs and spread some feed for them. They cluck contentedly." );

		sceneIncreaseDiscipline( FS_ANIMALS );
	}

	startScene( );

	pushSimpleChoice( "Return back to the farm house.", closure_Make( switchToNextPartOfDay, 0 ) );

	defaultSceneLoop( );
}

void animalCareCostlySuccess( CLOSURE( void )* closure, bool firstTimeRun )
{
	if( firstTimeRun ) {
		const char* injuryText = "UNSET";
		int type = balancedRandom( 3 );
		switch( type ) {
		case 0:
			// chickens got loose
			injuryText = "are exhausted yourself chasing chickens";

			setInitialSceneDescriptionString( "You get in the barn and greet the animals. Quickly you brush the horses and cows, then let them out to graze. "
				"Then you turn your attention to the chickens. You gather their eggs and spread some feed for them, but when you turn your back they escape from their "
				"enclosure. You spend a few hours chasing after them." );
			break;
		case 1:
			// nearly got kicked by the horse
			injuryText = "landed on your shoulder";

			setInitialSceneDescriptionString( "You get in the barn and greet the animals. Quickly you start brushing the horses and cows. As you're brushing one of the horses he "
				"begins to kick and buck. You quickly climb out of the pen, but in your haste fall and land on your shoulder. You let the other horses and cows out to graze. You're "
				"still in pain after you're done taking care of the chickens." );
			break;
		case 2:
			// fell while storing something
			injuryText = "fell and bent your ankle the wrong way";

			setInitialSceneDescriptionString( "You get in the barn and greet the animals. Quickly you brush the horses and cows, then let them out to graze. "
				"Then you turn your attention to the chickens. You gather their eggs and spread some feed for them. They cluck contentedly. Finally you go to get some more feed "
				"from the upper floors. While bringing it down you slip and fall, hurting yourself.");
			break;
		}

		bool builderTroubles = isFlagSetOnCompanion( &character, CMP_BUILDER, CF_THERE ) && !isFlagSetOnCompanion( &character, CMP_BUILDER, CF_APPEASED );
		if( builderTroubles && balancedRandom( 3 ) == 0 ) {
			setInitialSceneDescriptionString( "You get in the barn and greet the animals. Quickly you brush the horses and cows, then let them out to graze. "
				"Then you turn your attention to the chickens. You gather their eggs and spread some feed for them. They cluck contentedly. Finally you go to get some more feed "
				"from the upper floors. While bringing it down one of the rungs you step on breaks and you fall, hurting yourself. It looks like the rung had been partially sawed through." );

			injuryText = "fell and bent your ankle the wrong way";
		}

		sceneIncreaseDiscipline( FS_ANIMALS );
		sceneInflictWound( STAT_SELF, injuryText );
	}

	startScene( );

	pushSimpleChoice( "Return back to the farm house.", closure_Make( switchToNextPartOfDay, 0 ) );

	defaultSceneLoop( );
}

void animalCareFailure( CLOSURE( void )* closure, bool firstTimeRun )
{
	if( firstTimeRun ) {
		// you'll get wounded before you can help the animals
		bool builderTroubles = isFlagSetOnCompanion( &character, CMP_BUILDER, CF_THERE ) && !isFlagSetOnCompanion( &character, CMP_BUILDER, CF_APPEASED );
		if( builderTroubles && balancedRandom( 3 ) == 0 ) {
			setInitialSceneDescriptionString( "Stepping into barn you immediately see a bucket swinging down from above. You dodge to the side and it hits you in "
				"the shoulder. Reeling from the pain, you look up to see it tied to a rope going high into the rafters. Shaking this off you go to enter the barn "
				"to almost step on a board laid with rusty nails pointing straight up. While trying not to impale your foot you end up falling backwards into a "
				"pile of cow dung. You spend the rest of the time cleaning yourself off." );
			sceneInflictWound( STAT_SELF, "were hit with a bucket full of wheat" );
		} else {
			setInitialSceneDescriptionString( "Nothing is where it's supposed to be. You spend most of your time running around trying to find whatever tools "
				"you need. The pens are all either rusted shut, or won't stay shut. You feed most of the animals, but between chasing the ones that escape and "
				"trying to let out the ones that are trapped you barely have any energy as it becomes time to head back to the house.");
			sceneInflictWound( STAT_SELF, "were exhausted from everything going wrong at once" );
		}
	}

	startScene( );

	pushSimpleChoice( "Return back to the farm house.", closure_Make( switchToNextPartOfDay, 0 ) );

	defaultSceneLoop( );
}

void barnNap( CLOSURE( void )* closure, bool firstTimeRun )
{
	if( firstTimeRun ) {
		// you heal a wound from this
		setInitialSceneDescriptionString( "You find a nice shady part inside the barn not in view of the rest of the farm. Ignoring "
			"the sounds of the animals you put your hat over your face and take a nice long nap." );

		// there's a chance of seeing the builder
		if( isCompanionWithCharacter( &rpgGame, &character, CMP_BUILDER ) ) {
			if( balancedRandom( 3 ) == 0 ) {
				addReplacementString( "DESCRIPTION", rpgGame.arrCompanions[CMP_BUILDER].description );
				appendToSceneDisplayString( " When waking up you thought you saw [DESCRIPTION] duck into the barn. A quick search doesn't show anything there." );
			}
		}
		sceneHealWound( STAT_SELF );
	}

	startScene( );

	pushSimpleChoice( "Head back to the house.", closure_Make( switchToNextPartOfDay, 0 ) );

	defaultSceneLoop( );
}

void barnScene( CLOSURE( void )* closure, bool firstTimeRun )
{
	if( firstTimeRun ) {
		// choose a description based on the status level of the barn
		if( character.arrDisciplineLevels[FS_ANIMALS] <= 2 ) {
			addReplacementString( "BARN_DESCRIPTION", "seen better days but is still holding up," );
			addReplacementString( "ANIMAL_DESCRIPTION", "are hungry and agitated" );
		} else if( character.arrDisciplineLevels[FS_ANIMALS] <= 8 ) {
			addReplacementString( "BARN_DESCRIPTION", "seen better days but is still holding up," );
			addReplacementString( "ANIMAL_DESCRIPTION", "are waiting patiently for you" );
		} else {
			addReplacementString( "BARN_DESCRIPTION", "standing strong," );
			addReplacementString( "ANIMAL_DESCRIPTION", "are waiting patiently for you" );
		}

		if( isCompanionWithCharacter( &rpgGame, &character, CMP_BUILDER ) ) {
			addReplacementString( "BARN_DESCRIPTION", "as ancient and sturdy as ever," );
		}

		setInitialSceneDescriptionString( "You grab your gear and head out to the barn. It's [BARN_DESCRIPTION] and the animals [ANIMAL_DESCRIPTION]." );
	}

	startScene( );
	
	pushSkillBasedChoice( "Go take care of the animals.", SKL_ANIMALS, STAT_SELF, getAnimalsDifficulty( ),
		closure_Make( animalCareSuccess, 0 ), closure_Make( animalCareCostlySuccess, 0 ), closure_Make( animalCareFailure, 0 ) );
	pushSimpleChoice( "Find a place to nap.", closure_Make( barnNap, 0 ) );

	addFaeActionOptions( LOC_BARN );

	defaultSceneLoop( );
}

void youAnimalsScene( CLOSURE( void )* closure, bool firstTimeRun )
{
	// need to turn off a flag and then continue to the barn
	character.flags = turnOffFlag( character.flags, CF_PERFORMED_FAE_ACTION );

	// the builder will be pissed unless you address it before entering the barn
	removeFlagFromCompanion( &character, CMP_BUILDER, CF_APPEASED );

	switchScene( closure_Make( barnScene, 0 ) );
}

void cropsSuccess( CLOSURE( void )* closure, bool firstTimeRun )
{
	if( firstTimeRun ) {
		setInitialSceneDescriptionString( "You open up the hatch to your irrigation system and watch for a bit as the water fills the troughs between your rows of plants. "
			"While this is happening you go through the fields clearing out weeds and spraying some insecticide where you see insect eggs. After a while you close the irrigation "
			"hatch and head home." );
		sceneIncreaseDiscipline( FS_CROPS );
	}

	startScene( );

	pushSimpleChoice( "Head back to the house.", closure_Make( switchToNextPartOfDay, 0 ) );

	defaultSceneLoop( );
}

void cropsCostlySuccess( CLOSURE( void )* closure, bool firstTimeRun )
{
	if( firstTimeRun ) {
		setInitialSceneDescriptionString( "You open up the hatch to your irrigation system and watch for a bit as the water fills the troughs between your rows of plants. "
			"While this is happening you go through the fields clearing out weeds and spraying some insecticide where you see insect eggs. Closing the irrigation system doesn't "
			"go as smoothly. The hatch gets stuck and you try to use your foot to push it down. While doing so you slip and hit your head against a rock. The irrigation is stopped "
			"but you may have a concussion." );
		sceneIncreaseDiscipline( FS_CROPS );
		sceneInflictWound( STAT_SELF, "hit your head" );
	}

	startScene( );

	pushSimpleChoice( "Head back to the house.", closure_Make( switchToNextPartOfDay, 0 ) );

	defaultSceneLoop( );
}

void cropsFailure( CLOSURE( void )* closure, bool firstTimeRun )
{
	if( firstTimeRun ) {
		setInitialSceneDescriptionString( "You set out and try to open the irrigation system but it doesn't seem to be working at all. It seems there's a leak somewhere farther "
			"up that's keeping water from coming through. Following it eventually you find a tree log has fallen onto the pipes breaking some of them. While trying to move "
			"the log you feel a muscle in your back snap. You work through the pain and after getting some spare pipe you're able to repair everything. When you finish it's "
			"time to head back for lunch." );

		sceneInflictWound( STAT_SELF, "strained your back" );
	}

	startScene( );

	pushSimpleChoice( "Head back to the house.", closure_Make( switchToNextPartOfDay, 0 ) );

	defaultSceneLoop( );
}

void cropsNap( CLOSURE( void )* closure, bool firstTimeRun )
{
	if( firstTimeRun ) {

		addReplacementString( "SIGHTING", "" );
		if( isCompanionWithCharacter( &rpgGame, &character, CMP_DRYAD ) && ( balancedRandom( 3 ) == 0 ) ) {
			addReplacementString( "DESCRIPTION", rpgGame.arrCompanions[CMP_DRYAD].description );
			addReplacementString( "SIGHTING", " and you see what looks like [DESCRIPTION] watching you from the tree foliage. She is gone when you go for a closer look" );
		}

		setInitialSceneDescriptionString( "You look around and find a nice spot on the far edge of the fields where the trees will provide plenty of shade. You lie against one of "
			"the trees and close your eyes. After a few hours you wake up[SIGHTING]." );
		sceneHealWound( STAT_SELF );
	}

	startScene( );

	pushSimpleChoice( "Head back to the house.", closure_Make( switchToNextPartOfDay, 0 ) );

	defaultSceneLoop( );
}

void cropsScene( CLOSURE( void )* closure, bool firstTimeRun )
{
	if( firstTimeRun ) {
		// choose a description based on the status level of the barn
		if( character.arrDisciplineLevels[FS_ANIMALS] <= 2 ) {
			addReplacementString( "DESCRIPTION", "sparse and wilted" );
		} else if( character.arrDisciplineLevels[FS_ANIMALS] <= 8 ) {
			addReplacementString( "DESCRIPTION", "healthy" );
		} else {
			addReplacementString( "DESCRIPTION", "full and healthy" );
		}

		setInitialSceneDescriptionString( "You grab your gear and head out to the farm land. The crops are [DESCRIPTION]. You set to work." );
	}

	startScene( );

	pushSkillBasedChoice( "Go take care of the crops.", SKL_CROPS, STAT_SELF, getCropsDifficulty( ),
		closure_Make( cropsSuccess, 0 ), closure_Make( cropsCostlySuccess, 0 ), closure_Make( cropsFailure, 0 ) );
	pushSimpleChoice( "Find a place to nap.", closure_Make( cropsNap, 0 ) );

	addFaeActionOptions( LOC_FIELD );

	defaultSceneLoop( );
}

void youCropsScene( CLOSURE( void )* closure, bool firstTimeRun )
{
	// need to turn off a flag and then continue to the crops
	character.flags = turnOffFlag( character.flags, CF_PERFORMED_FAE_ACTION );
	switchScene( closure_Make( cropsScene, 0 ) );
}

void farmHouseSuccess( CLOSURE( void )* closure, bool firstTimeRun )
{
	if( firstTimeRun ) {
		bool hobHelp = isFlagSetOnCompanion( &character, CMP_HOUSE_HOB, CF_THERE ) && isFlagSetOnCompanion( &character, CMP_HOUSE_HOB, CF_APPEASED );

		if( hobHelp && ( balancedRandom( 3 ) == 0 ) ) {
			setInitialSceneDescriptionString( "You go to start cleaning only to find most everything had already been cleaned. You do some light sweeping, but "
				"nothing else needs to be done for that. You spend the rest of your time making small repairs." );
		} else {
			setInitialSceneDescriptionString( "You start by cleaning everything you can. Dusting, scrubbing, sweeping, and mopping where needed. This takes up "
				"most of your time, but you do get around to making a few repairs that are needed." );
		}

		sceneIncreaseDiscipline( FS_FARMHOUSE );
	}

	startScene( );

	if( isFlagOn( character.flags, CF_IS_AFTERNOON ) ) {
		pushSimpleChoice( "Get dinner ready.", closure_Make( switchToNextPartOfDay, 0 ) );
	} else {
		pushSimpleChoice( "Get lunch ready.", closure_Make( switchToNextPartOfDay, 0 ) );
	}

	defaultSceneLoop( );
}

void farmHouseCostlySuccess( CLOSURE( void )* closure, bool firstTimeRun )
{
	if( firstTimeRun ) {
		bool hobTroubles = isFlagSetOnCompanion( &character, CMP_HOUSE_HOB, CF_THERE ) && isFlagSetOnCompanion( &character, CMP_HOUSE_HOB, CF_ANGERED );

		if( hobTroubles && ( balancedRandom( 3 ) == 0 ) ) {
			setInitialSceneDescriptionString( "You start by cleaning everything you can. Dusting, scrubbing, sweeping, and mopping where needed. This takes up "
				"most of your time. While trying to fix one of the ceiling fixtures you reach down for the hammer you knew was there and it was gone. While groping "
				"around you lose your balance and fall." );
		} else {
			setInitialSceneDescriptionString( "You start by cleaning everything you can. Scrubbing, sweeping, and mopping where needed. You figure you'll finish up with "
				"some dusting. While cleaning off the top of the book shelves the step stool slips out from under you sending you face first to the ground." );
		}

		sceneIncreaseDiscipline( FS_FARMHOUSE );
		sceneInflictWound( STAT_SELF, "hit your head" );
	}

	startScene( );

	if( isFlagOn( character.flags, CF_IS_AFTERNOON ) ) {
		pushSimpleChoice( "Get dinner ready.", closure_Make( switchToNextPartOfDay, 0 ) );
	} else {
		pushSimpleChoice( "Get lunch ready.", closure_Make( switchToNextPartOfDay, 0 ) );
	}

	defaultSceneLoop( );
}

void farmHouseFailure( CLOSURE( void )* closure, bool firstTimeRun )
{
	if( firstTimeRun ) {
		bool hobTroubles = isFlagSetOnCompanion( &character, CMP_HOUSE_HOB, CF_THERE ) && isFlagSetOnCompanion( &character, CMP_HOUSE_HOB, CF_ANGERED );
		bool fairyTroubles = isFlagSetOnCompanion( &character, CMP_FAIRY, CF_THERE ) && isFlagSetOnCompanion( &character, CMP_FAIRY, CF_ANGERED );

		if( fairyTroubles && ( balancedRandom( 3 ) == 0 ) ) {
			setInitialSceneDescriptionString( "One of the windows has been giving you problems so you start with that. You lock the window so it won't open "
				"while you're working on it. Getting on the step stool you put your weight on the window frame and it swings open sending you flying to the "
				"ground outside." );
			sceneInflictWound( STAT_SELF, "landed on your shoulder" );
		} else if( hobTroubles && ( balancedRandom( 3 ) == 0 ) ) {
			setInitialSceneDescriptionString( "All your tools have gone missing. You spend the entire time searching for them but can't find any hint of them." );
			sceneInflictWound( STAT_SELF, "became extremely frustrated" );
		} else {
			setInitialSceneDescriptionString( "While cleaning the house you try to move a heavy cupboard so you can clean behind it. While trying to do so you feel "
				"someething tear in your left arm. You quickly lower the cupboard back down and get something to put on your arm." );
			sceneInflictWound( STAT_SELF, "strained your arm muscles" );
		}
	}

	startScene( );

	if( isFlagOn( character.flags, CF_IS_AFTERNOON ) ) {
		pushSimpleChoice( "Get dinner ready.", closure_Make( switchToNextPartOfDay, 0 ) );
	} else {
		pushSimpleChoice( "Get lunch ready.", closure_Make( switchToNextPartOfDay, 0 ) );
	}

	defaultSceneLoop( );
}

void farmHouseNap( CLOSURE( void )* closure, bool firstTimeRun )
{
	if( firstTimeRun ) {
		addReplacementString( "SIGHTING", "" );
		if( isCompanionWithCharacter( &rpgGame, &character, CMP_HOUSE_HOB ) && balancedRandom( 3 ) == 0 ) {
			addReplacementString( "SIGHTING", " You awake with a start, and see [DESCRIPTION] that vanishes once you clear your eyes." );
			addReplacementString( "DESCRIPTION", rpgGame.arrCompanions[CMP_HOUSE_HOB].description );
		} else if( isCompanionWithCharacter( &rpgGame, &character, CMP_FAIRY ) && balancedRandom( 3 ) == 0 ) {
			addReplacementString( "SIGHTING", " You awake with a start, and see [DESCRIPTION] that vanishes once you clear your eyes." );
			addReplacementString( "DESCRIPTION", rpgGame.arrCompanions[CMP_FAIRY].description );
		}
		setInitialSceneDescriptionString( "You look around and find a nice spot in the cellar. It's cool and people are unlikely to come down here. You close your eyes and drift off.[SIGHTING]" );
		sceneHealWound( STAT_SELF );
	}

	startScene( );

	pushSimpleChoice( "Get up before someone finds you sleeping.", closure_Make( switchToNextPartOfDay, 0 ) );

	defaultSceneLoop( );
}

void farmHouseScene( CLOSURE( void )* closure, bool firstTimeRun )
{
	if( firstTimeRun ) {
		// choose a description based on the status level of the house
		if( character.arrDisciplineLevels[FS_ANIMALS] <= 2 ) {
			addReplacementString( "DESCRIPTION", "dirty" );
			addReplacementString( "TOOLS", "some cleaning supplies and a toolbox" );
		} else if( character.arrDisciplineLevels[FS_ANIMALS] <= 8 ) {
			addReplacementString( "DESCRIPTION", "clean" );
			addReplacementString( "TOOLS", "some cleaning supplies and a toolbox" );
		} else {
			addReplacementString( "DESCRIPTION", "sparkling clean" );
			addReplacementString( "TOOLS", "a toolbox" );
		}

		setInitialSceneDescriptionString( "You look around and see that everything is [DESCRIPTION]. Grabbing [TOOLS] you get to work." );
	}

	startScene( );

	pushSkillBasedChoice( "Take care of the house.", SKL_CROPS, STAT_SELF, getFarmHouseDifficulty( ),
		closure_Make( farmHouseSuccess, 0 ), closure_Make( farmHouseCostlySuccess, 0 ), closure_Make( farmHouseFailure, 0 ) );
	pushSimpleChoice( "Find a place to nap.", closure_Make( farmHouseNap, 0 ) );

	addFaeActionOptions( LOC_CLEANING );

	defaultSceneLoop( );
}

void youFarmHouseScene( CLOSURE( void )* closure, bool firstTimeRun )
{
	// need to turn off a flag and then continue to the farm house
	character.flags = turnOffFlag( character.flags, CF_PERFORMED_FAE_ACTION );
	switchScene( closure_Make( farmHouseScene, 0 ) );
}




void radioSuccess( CLOSURE( void )* closure, bool firstTimeRun )
{
	if( firstTimeRun ) {
		// a radio show is playing
		setInitialSceneDescriptionString( "You turn on the radio. You catch the start of the family's favorite radio "
			"drama. All of you spend the next couple hours enraptured, following the exploits of Callo as he takes out "
			"rival mob bosses, evades the police, and breaks a few hearts. You turn off the radio as the news starts." );

		sceneIncreaseDiscipline( FS_FAMILY );

		// also heal a random wound
		Stats woundedStat = getRandomWound( );
		if( woundedStat != INVALID_STAT ) {
			sceneHealWound( woundedStat );
		}
	}

	startScene( );

	pushSimpleChoice( "Eat supper and go to bed.", closure_Make( switchToNextPartOfDay, 0 ) );

	defaultSceneLoop( );
}

void radioCostlySuccess( CLOSURE( void )* closure, bool firstTimeRun )
{
	if( firstTimeRun ) {

		// normal news, nothing too exciting
		setInitialSceneDescriptionString( "You turn on the radio. After a few songs have played a news show starts. "
			"It's nothing too exciting. Some reports from the front but no details. Advertisements for tooth replacement "
			"and eyeglasses. This inspires [SPOUSE_NAME] to clack [SPOUSE_POSSESSIVE_PRONOUN] steel teeth together. After "
			"listening to the local weather report you turn off the radio.");

		sceneIncreaseDiscipline( FS_FAMILY );

		// also heal a random wound
		Stats woundedStat = getRandomWound( );
		if( woundedStat != INVALID_STAT ) {
			sceneHealWound( woundedStat );
		}
	}

	startScene( );

	pushSimpleChoice( "Eat supper and go to bed.", closure_Make( switchToNextPartOfDay, 0 ) );

	defaultSceneLoop( );
}

void radioFailure( CLOSURE( void )* closure, bool firstTimeRun )
{
	if( firstTimeRun ) {
		// disturbing news about people nearby disappearing
		setInitialSceneDescriptionString( "You turn on the radio. After a few songs have played a news show starts. "
			"It seems that some of your neighbors have disappeared from their homes, leaving their farms completely "
			"empty. Occasionally there are traces of blood and signs of a struggle, but most people seem to just "
			"vanish into thin air.\n"
			"You turn off the radio and everybody tries to put the report out of their mind.");
	}

	startScene( );

	pushSimpleChoice( "Eat supper and go to bed.", closure_Make( switchToNextPartOfDay, 0 ) );

	defaultSceneLoop( );
}

void gameSuccess( CLOSURE( void )* closure, bool firstTimeRun )
{
	if( firstTimeRun ) {
		
		// game is close and exciting, everyone has fun
		Stats winner = balancedRandom( NUM_STATS );
		Stats other;
		do {
			other = balancedRandom( NUM_STATS );
		} while( winner == other );
		addReplacementString( "WINNER_NAME", rpgGame.arrStats[winner].name );
		addReplacementString( "OTHER_NAME", rpgGame.arrStats[other].name );
		addReplacementString( "WS", winner == STAT_SELF ? "" : "S" );
		addReplacementString( "OS", other == STAT_SELF ? "" : "S" );
		addReplacementString( "WINNER_PRONOUN", rpgGame.arrStats[winner].subjectPronoun );

		setInitialSceneDescriptionString( "You pull out one of the board games on the shelf.  After setting up you all begin playing.\n"
			"The game is a constant back and forth. After an hour [WINNER_NAME] gain[WS] the lead. [OTHER_NAME] attempt[OS] a sneaky tactic, "
			"but isn't able to get the win.\n"
			"Everybody packs up the game and it's put back on the shelf." );
		
		sceneIncreaseDiscipline( FS_FAMILY );

		// also heal up to two random wounds
		Stats woundedStat = getRandomWound( );
		if( woundedStat != INVALID_STAT ) {
			sceneHealWound( woundedStat );
		}
		woundedStat = getRandomWound( );
		if( woundedStat != INVALID_STAT ) {
			sceneHealWound( woundedStat );
		}
	}

	startScene( );

	pushSimpleChoice( "Eat supper and go to bed.", closure_Make( switchToNextPartOfDay, 0 ) );

	defaultSceneLoop( );
}

void gameCostlySuccess( CLOSURE( void )* closure, bool firstTimeRun )
{
	if( firstTimeRun ) {

		// one person slowly pulls head, everyone has fun
		Stats winner = balancedRandom( NUM_STATS );
		Stats other;
		do {
			other = balancedRandom( NUM_STATS );
		} while( winner == other );
		addReplacementString( "WINNER_NAME", rpgGame.arrStats[winner].name );
		addReplacementString( "OTHER_NAME", rpgGame.arrStats[other].name );
		addReplacementString( "WS", winner == STAT_SELF ? "" : "S" );
		addReplacementString( "WINNER_PRONOUN", rpgGame.arrStats[winner].subjectPronoun );

		setInitialSceneDescriptionString( "You pull out one of the board games on the shelf. After setting up you all begin playing.\n"
			"It takes a while but eventually [OTHER_NAME] is eliminated from the game and "
			"[WINNER_NAME] slowly take[WS] the lead. There appear to be sometimes where [WINNER_PRONOUN]'ll slip up but [WINNER_PRONOUN] always recovers.\n"
			"Everybody packs up the game and it's put back on the shelf." );

		sceneIncreaseDiscipline( FS_FAMILY );

		// also heal a wound
		Stats woundedStat = getRandomWound( );
		if( woundedStat != INVALID_STAT ) {
			sceneHealWound( woundedStat );
		}
	}

	startScene( );

	pushSimpleChoice( "Eat supper and go to bed.", closure_Make( switchToNextPartOfDay, 0 ) );

	defaultSceneLoop( );
}

void gameFailure( CLOSURE( void )* closure, bool firstTimeRun )
{
	if( firstTimeRun ) {
		// someone beats everyone else soundly, nobody really has fun
		Stats winner = balancedRandom( NUM_STATS );
		Stats other;
		do {
			other = balancedRandom( NUM_STATS );
		} while( winner == other );
		addReplacementString( "WINNER_NAME", rpgGame.arrStats[winner].name );
		addReplacementString( "OTHER_NAME", rpgGame.arrStats[other].name );
		addReplacementString( "WS", winner == STAT_SELF ? "" : "S" );

		setInitialSceneDescriptionString( "You pull out one of the board games on the shelf.  After setting up you all begin playing.\n"
			"Immediately [WINNER_NAME] pull[WS] ahead. They quickly trounce "
			"[OTHER_NAME] who has to sit out while watching [WINNER_NAME] slowly finish the game.\n"
			"Nobody has much fun and you decided to never play this game again, throwing it in the trash." );
	}

	startScene( );

	pushSimpleChoice( "Eat supper and go to bed.", closure_Make( switchToNextPartOfDay, 0 ) );

	defaultSceneLoop( );
}

Result storedResult;

void handleReadResult( CLOSURE( void )* closure, bool firstTimeRun )
{
	if( firstTimeRun ) {

		if( storedResult = R_SUCCESS ) {
			// spend time making up stories
			setInitialSceneDescriptionString( "After closing the book you look up at yoru family and ask them what they think. [SPOUSE_NAME] "
				"says it's a nice set of stories, but that's all they are. You ask then why not make up our own stories about them. [CHILD_NAME] "
				"immediately starts coming up with a scenario about a Derbshiur and Foirmeacha protecting a child that was lost in the woods. You add a "
				"Fidbaid that guides them through the forest when they get lost. They then find a Occorach Fer but mistake it for a Torathar, but "
				"realize their mistake before it's too late and have some tea with him.\n"
				"For the next few hours you spend time expanding on the story, adding new characters and twists, but it starts to get late.");

			sceneIncreaseDiscipline( FS_FAMILY );

			// also heal a wound
			Stats woundedStat = getRandomWound( );
			if( woundedStat != INVALID_STAT ) {
				sceneHealWound( woundedStat );
			}
		} else if( storedResult == R_COSTLY_SUCCESS ) {
			// ask opinions about
			setInitialSceneDescriptionString( "After closing the book you look up at your family and ask them which one was their favorite. [SPOUSE_NAME] "
				"says [SPOUSE_SUBJECT_PRONOUN]'d like a Timthirid to help around the house, but that a Derbshiur would also be nice. [CHILD_NAME] says [CHILD_SUBJECT_PRONOUN] "
				"likes the Meraige, and wonders how the fair folk drive away a Torathar. You shrug and say you don't know, but if one every comes you'll be sure to project "
				"both [CHILD_NAME] and [BABY_NAME]. [CHILD_NAME] grimaces at this and says [CHILD_SUBJECT_PRONOUN] is old enough to not need protection.\n"
				"The family continues this way for a while." );

			sceneIncreaseDiscipline( FS_FAMILY );
		} else {
			// failure, just close book
			setInitialSceneDescriptionString( "You close the book and look up at your family. [CHILD_NAME] is busy playing with [BABY_NAME] and seems to "
				"paid no attention. [SPOUSE_NAME] is smiling at you politely. You sigh and put the book away.");
		}
	}

	startScene( );

	pushSimpleChoice( "Eat supper and go to bed.", closure_Make( switchToNextPartOfDay, 0 ) );

	defaultSceneLoop( );
}

void readSceneOne( CLOSURE( void )* closure, bool firstTimeRun )
{
	character.flags = turnOnFlag( character.flags, CF_READ_BOOK );
	topTitle = " On The Legends Of The Fair Folk In The Lower Ridianfain Region ";
	startDraw( window ); {
		drawString( window, 2, 2, safeWriteArea,
			"  The book is about a mythical species of creatures called the fair folk that were said to live in the region your farm "
			"is now in. They are said to cohabitate with people and come in a wide "
			"variety of types. Most of the text is a dry academic anthropological work. But you find the section describing "
			"the various fair folk and read out the entries.\n"
			"*  The most helpful of them is the Timthirid. A tiny creature that lives in houses, fixing and cleaning things, as well "
			"as leaving offerings of acorns. They will stay helpful as long as they are regularly left a bowl of milk, otherwise "
			"they will start stealing things around the home. A bowl of salt will drive them off. Never attempt to thank them.\n"
			"*  One of the most common types reported were the Meraige. Tiny winged women who loved to play pranks. You must always "
			"laugh at these pranks otherwise they will try to make their pranks funnier, which invariably makes them more dangerous. "
			"They detest iron and can be driven off by hanging small pieces of iron from the ceiling, which is where they prefer to hide.\n"
			"*  The Fidbaid are fair folk said to live in trees. They cultivate the land around the home tree, making these trees desirable. "
			"You can tell if one inhabits a tree if you knock on it and the Fidbaid knocks back from the inside.\n"
			"*  Another helpful fair folk is the Derbshiur. They look like small twisted, women and are quite adept climbers. They love children "
			"and will help dutiful parents. Parents who live with a Derbshiur and ignore their children have said that their homes have been "
			"infested by hordes of spiders.\n"
			"*  One type which is said to be either a fair folk or a spirit of the dead is the Foirmeacha. Short bearded men who will lay claim to "
			"a building and protect it, keeping it safe and in repair with a skill that puts most engineers to shame. You must always ask "
			"them before entering the building they've claimed though as they view as theirs and will protect it with violence if necessary.\n"
			"*  The gaunt and hungry looking Occorach Fer is a lesson in not judging a book by it's cover. They are quite helpful if fed, and if told "
			"to leave will do so while causing no harm.\n"
			"*  The Ginach is a small snake with human arms and legs. They love any sort of water and will lay claim to rivers, ponds and wells. "
			"If they think someone is stealing the water they'll spoil it, making any who drink it sick. They can be bribed with shiny objects.\n"
			"*  One of the most evil of them is the Torathar. Squat humanoid creatures with red eyes and long claws. They love the taste "
			"of flesh. If invited into a home they will tear apart their hosts, devouring them. When becoming hungry enough they have been known "
			"to break into homes where they will start by taking the smallest first. Only other fair folk know how to drive these away.",
			attr_Clrs( PDC_GREY, PDC_BLACK ) );

		centerStringHoriz( window, safeWriteArea, safeWriteArea.bottom, "Press any key to finish", attr_Clrs( PDC_YELLOW, PDC_BLACK ) );
	} endDraw( window );

	waitForAnyInput( window );

	switchScene( closure_Make( handleReadResult, 0 ) );
}

void readSuccess( CLOSURE( void )* closure, bool firstTimeRun )
{
	storedResult = R_SUCCESS;
	switchScene( closure_Make( readSceneOne, 0 ) );
}

void readCostlySuccess( CLOSURE( void )* closure, bool firstTimeRun )
{
	storedResult = R_COSTLY_SUCCESS;
	switchScene( closure_Make( readSceneOne, 0 ) );
}

void readFailure( CLOSURE( void )* closure, bool firstTimeRun )
{
	storedResult = R_FAILURE;
	switchScene( closure_Make( readSceneOne, 0 ) );
}

void familyScene( CLOSURE( void )* closure, bool firstTimeRun )
{
	if( firstTimeRun ) {
		setInitialSceneDescriptionString( "You gather the family around and prepare to spend some quality together." );
	}

	startScene( );

	// these should generally be positive, event costly success shouldn't hurt, failure still does though
	//  reading will be the difficult one because we have a lot of information we want to provide with that one, so it will have to be multiple scenes
	//  and we don't have scrolling text, just scrolling options
	pushSkillBasedChoice( "Read from 'The Book Of Ridianfain Fair Folk'.", SKL_FAMILY, STAT_SELF, getFamilyDifficulty( EASY_RANK ),
		closure_Make( readSuccess, 0 ), closure_Make( readCostlySuccess, 0 ), closure_Make( readFailure, 0 ) );
	pushSkillBasedChoice( "Listen to some radio.", SKL_FAMILY, STAT_SELF, getFamilyDifficulty( NORMAL_RANK ),
		closure_Make( radioSuccess, 0 ), closure_Make( radioCostlySuccess, 0 ), closure_Make( radioFailure, 0 ) );
	pushSkillBasedChoice( "Play a board game.", SKL_FAMILY, STAT_SELF, getFamilyDifficulty( DIFFICULT_RANK ),
		closure_Make( gameSuccess, 0 ), closure_Make( gameCostlySuccess, 0 ), closure_Make( gameFailure, 0 ) );

	addFaeActionOptions( LOC_WITH_FAMILY );

	defaultSceneLoop( );
}

void youFamilyScene( CLOSURE( void )* closure, bool firstTimeRun )
{
	// baby sitter likes this
	gameCharacterData.daysWithoutFamilyInteraction = 0;
	character.flags = turnOffFlag( character.flags, CF_PERFORMED_FAE_ACTION );
	switchScene( closure_Make( familyScene, 0 ) );
}


void treeCutMostScene( CLOSURE( void )* closure, bool firstTimeRun )
{
	if( firstTimeRun ) {
		character.flags = turnOnFlag( character.flags, CF_CUT_TREES );
		setInitialSceneDescriptionString( "You spend hours chopping and sawing, leaving the tree that knocked back standing. Pulling up the stumps and clearing out the underbrush "
			"can wait until the harvest is finished." );
		sceneIncreaseDiscipline( FS_CROPS );
	}

	startScene( );

	pushSimpleChoice( "Head home.", closure_Make( switchToNextPartOfDay, 0 ) );

	defaultSceneLoop( );
}

void treeCutAllScene( CLOSURE( void )* closure, bool firstTimeRun )
{
	if( firstTimeRun ) {
		character.flags = turnOnFlag( character.flags, CF_CUT_TREES );
		removeFlagFromCompanion( &character, CMP_DRYAD, CF_THERE );
		setInitialSceneDescriptionString( "You spend hours chopping and sawing. Eventually the trees are all down. Pulling up the stumps and clearing out the underbrush "
			"can wait until the harvest is finished.");
		sceneIncreaseDiscipline( FS_CROPS );
	}

	startScene( );

	pushSimpleChoice( "Head home.", closure_Make( switchToNextPartOfDay, 0 ) );

	defaultSceneLoop( );
}

void treeIntroScene( CLOSURE( void )* closure, bool firstTimeRun )
{
	if( firstTimeRun ) {
		setInitialSceneDescriptionString( "There's a small copse of young trees you want to clear out to make room for more farmland in the future. Today seems to be a good day "
			"to do it. This species grows fast so you want to do it as soon as possible. It's dense but the trees are all young, only slightly taller than you are. Cutting them shouldn't take too long."
			"\n\nWhat do you do?" );
	}

	startScene( );

	pushSimpleChoice( "Cut down all the trees.", closure_Make( treeCutAllScene, 0 ) );
	if( isCompanionWithCharacter( &rpgGame, &character, CMP_DRYAD ) && isFlagSetOnCompanion( &character, CMP_DRYAD, CF_APPEASED ) ) {
		pushSimpleChoice( "Cut down all the trees except for that strange one.", closure_Make( treeCutMostScene, 0 ) );
	}
	addFaeActionOptions( LOC_WOODS );

	defaultSceneLoop( );
}

void youTreeScene( CLOSURE( void )* closure, bool firstTimeRun )
{
	// need to turn off a flag and then continue to the trees
	character.flags = turnOffFlag( character.flags, CF_PERFORMED_FAE_ACTION );
	switchScene( closure_Make( treeIntroScene, 0 ) );
}

void wellLookFae( CLOSURE( void )* closure, bool firstTimeRun )
{
	if( firstTimeRun ) {
		setInitialSceneDescriptionString( "You pull out your electric lantern and shine it down into the well. It looks clear, although the smell is still there."
			"\n\nWhat do you do?" );
	}

	startScene( );

	pushSimpleChoice( "Leave.", closure_Make( switchToNextPartOfDay, 0 ) );
	addFaeActionOptions( LOC_WELL );

	defaultSceneLoop( );
}

void wellGetBodySuccess( CLOSURE( void )* closure, bool firstTimeRun )
{
	if( firstTimeRun ) {
		character.flags = turnOffFlag( character.flags, CF_DIRTY_WELL );
		setInitialSceneDescriptionString( "Carefully you use the bucket, your electric lantern, and a long pole to retrieve the body. Luckily you're able to "
			"get it mostly intact. Bringing the body to the barn for disposal you grab some chlorine. Heading back to the well you dump the bottle down there "
			"and then run the pump for a few hours. You'll probably want to boil the water for a while but there should be no more problems." );
	}

	startScene( );

	pushSimpleChoice( "Leave.", closure_Make( switchToNextPartOfDay, 0 ) );

	defaultSceneLoop( );
}

void wellGetBodyCostlySuccess( CLOSURE( void )* closure, bool firstTimeRun )
{
	if( firstTimeRun ) {
		character.flags = turnOffFlag( character.flags, CF_DIRTY_WELL );
		setInitialSceneDescriptionString( "Carefully you use the bucket, your electric lantern, and a long pole to retrieve the body. It takes longer than "
			"expected as the body is very fragile. While bringing the body to the bard for disposal you trip. The body lands on the ground, and you land in it. "
			"After spending a few minutes vomiting. You get the barn and grab some bleach. You strip and douse your clothes in it. Then you grab some soap and "
			"using water from one of the troughs clean yourself as well as you can. Finally heading back to the well you dump the rest of the bleach down there "
			"and then run the pump for a few hours. You'll probably want to boil the water for a while but there should be no more problems." );
		sceneInflictWound( STAT_SELF, "fell in something disgusting" );
	}

	startScene( );

	pushSimpleChoice( "Leave.", closure_Make( switchToNextPartOfDay, 0 ) );

	defaultSceneLoop( );
}

void wellGetBodyFailure( CLOSURE( void )* closure, bool firstTimeRun )
{
	if( firstTimeRun ) {
		setInitialSceneDescriptionString( "You spend hours looking around for something to help you. No tools you find can help, and you don't want to risk breaking "
			"the body apart. Finally you decided to try and use just the bucket to get it out. All you end up doing is splashing yourself with corpse water. You'll have "
			"to try again later." );
		sceneInflictWound( STAT_SELF, "got soaked in corpse water" );
	}

	startScene( );

	pushSimpleChoice( "Leave.", closure_Make( switchToNextPartOfDay, 0 ) );

	defaultSceneLoop( );
}

void wellLookBody( CLOSURE( void )* closure, bool firstTimeRun )
{
	if( firstTimeRun ) {
		character.flags = turnOnFlag( character.flags, CF_SEEN_BODY_IN_WELL );
		setInitialSceneDescriptionString( "You pull out your electric lantern and shine it down into the well. The source of the smell is immediately evident. Something "
			"had gotten into the well and died. You're going to have to fish out the corpse and treat the water."
			"\n\nWhat do you do?" );
	}

	startScene( );

	pushSkillBasedChoice( "Try to get the dead animal out of the well.", SKL_FARMHOUSE, STAT_SELF, HARD_TEST,
		closure_Make( wellGetBodySuccess, 0 ), closure_Make( wellGetBodyCostlySuccess, 0 ), closure_Make( wellGetBodyFailure, 0 ) );
	addFaeActionOptions( LOC_WELL );
	pushSimpleChoice( "Leave.", closure_Make( switchToNextPartOfDay, 0 ) );

	defaultSceneLoop( );
}

void wellIntro( CLOSURE( void )* closure, bool firstTimeRun )
{
	if( firstTimeRun ) {

		if( isFlagOn( character.flags, CF_SEEN_WELL ) ) {
			addReplacementString( "INTRO", "You're standing next to the well." );
		} else {
			character.flags = turnOnFlag( character.flags, CF_SEEN_WELL );
			addReplacementString( "INTRO", "You make your way to the well. It's next to the house." );
		}

		addReplacementString( "TASK", "Getting close you can smell something is off." );
		if( !isFlagOn( character.flags, CF_DIRTY_WELL ) ) {
			addReplacementString( "TASK", "The water is clean." );
		}
		if( isFlagOn( character.flags, CF_SEEN_BODY_IN_WELL ) ) {
			addReplacementString( "TASK", "The dead animal is still in the well. You'll have to get it out." );
		}

		setInitialSceneDescriptionString( "[INTRO] The top is open and you can use a bucket to get the water "
			"but there's also a new pump that pulls from it too. [TASK]"
			"\n\nWhat do you do?" );
	}

	startScene( );

	if( isFlagOn( character.flags, CF_SEEN_BODY_IN_WELL ) && isFlagOn( character.flags, CF_DIRTY_WELL ) ) {
		pushSkillBasedChoice( "Try to get the dead animal out of the well.", SKL_FARMHOUSE, STAT_SELF, HARD_TEST,
			closure_Make( wellGetBodySuccess, 0 ), closure_Make( wellGetBodyCostlySuccess, 0 ), closure_Make( wellGetBodyFailure, 0 ) );
	} else {
		if( isFlagOn( character.flags, CF_DIRTY_WELL ) ) {
			if( isCompanionWithCharacter( &rpgGame, &character, CMP_WATER ) ) {
				pushSimpleChoice( "Look down the well.", closure_Make( wellLookFae, 0 ) );
			} else {
				pushSimpleChoice( "Look down the well.", closure_Make( wellLookBody, 0 ) );
			}
		}
	}
	addFaeActionOptions( LOC_WELL );
	pushSimpleChoice( "Leave.", closure_Make( switchToNextPartOfDay, 0 ) );

	defaultSceneLoop( );
}

void youWellScene( CLOSURE( void )* closure, bool firstTimeRun )
{
	// need to turn off a flag and then continue to the well
	character.flags = turnOffFlag( character.flags, CF_PERFORMED_FAE_ACTION );
	switchScene( closure_Make( wellIntro, 0 ) );
}

void applyOtherAnimalsReport( Stats other )
{
	// if the builder isn't appeased he'll try and hurt the person, reducing their chance of success and changing what they'll report back
	bool builderTroubles = isFlagSetOnCompanion( &character, CMP_BUILDER, CF_THERE ) && !isFlagSetOnCompanion( &character, CMP_BUILDER, CF_APPEASED );

	uint8_t difficulty = getAnimalsDifficulty( );

	Result result = complexSkillCheck( &rpgGame, &character, other, SKL_ANIMALS, 0, difficulty );

	addReplacementString( "NAME", rpgGame.arrStats[other].name );

	switch( result ) {
	case R_SUCCESS:
		appendToSceneDisplayString( "[NAME] cleaned, fed, and tended all the animals. Some problems with the barn were also fixed." );
		sceneIncreaseDiscipline( FS_ANIMALS );
		break;
	case R_COSTLY_SUCCESS:
		if( builderTroubles && ( balancedRandom( 3 ) == 0 ) ) {
			appendToSceneDisplayString( "[NAME] collapses on the couch. While setting up all the pens opened up at once. They were nearly trampled but was able to get everyone back in." );
			sceneIncreaseDiscipline( FS_ANIMALS );
			sceneInflictWound( other, "was nearly trampled" );
		} else {
			appendToSceneDisplayString( "[NAME] collapses on the couch. Some of the chickens escaped after the chores were done and it took a few hours of chasing to get them back." );
			sceneIncreaseDiscipline( FS_ANIMALS );
			sceneInflictWound( other, "is exhausted from chasing chickens" );
		}
		break;
	case R_FAILURE:
		if( builderTroubles && ( balancedRandom( 3 ) == 0 ) ) {
			appendToSceneDisplayString( "[NAME] limps back in and goes to was a nasty looking cut. One of the saws stored in the barn fell while they were working." );
			sceneInflictWound( other, "was cut by a saw" );
		} else {
			appendToSceneDisplayString( "[NAME] collapses on the couch. Some of the chickens escaped after the chores were done and it took a few hours of chasing to get them back." );
			sceneInflictWound( other, "is exhausted from chasing chickens" );
		}
		break;
	}
}

void applyOtherCropsReport( Stats other )
{
	uint8_t difficulty = getCropsDifficulty( );

	Result result = complexSkillCheck( &rpgGame, &character, other, SKL_ANIMALS, 0, difficulty );

	addReplacementString( "NAME", rpgGame.arrStats[other].name );

	switch( result ) {
	case R_SUCCESS:
		appendToSceneDisplayString( "[NAME] spent their time taking care of the crops, making sure they had enough water and pulling some weeds." );
		sceneIncreaseDiscipline( FS_CROPS );
		break;
	case R_COSTLY_SUCCESS:
		appendToSceneDisplayString( "[NAME] spent some their time taking care of the crops, but had noticed some deer trying to eat them and spent most of the time scaring them off." );
		sceneIncreaseDiscipline( FS_CROPS );
		sceneInflictWound( other, "is exhausted from chasing deer" );
		break;
	case R_FAILURE:
		appendToSceneDisplayString( "[NAME] tried to take care of the plants but after hearing some howling from the nearby woods was too scared to approach." );
		sceneInflictWound( other, "was frightened by howling" );
		break;
	}
}

void applyOtherFarmHouseReport( Stats other )
{
	// if the hob is happy he'll help, and things will be cleaner than usual
	// if the hob is angry he'll hinder, random things will go missing
	bool hobTroubles = isFlagSetOnCompanion( &character, CMP_HOUSE_HOB, CF_THERE ) && isFlagSetOnCompanion( &character, CMP_HOUSE_HOB, CF_ANGERED );
	bool hobHelp = isFlagSetOnCompanion( &character, CMP_HOUSE_HOB, CF_THERE ) && isFlagSetOnCompanion( &character, CMP_HOUSE_HOB, CF_APPEASED );

	uint8_t difficulty = getFarmHouseDifficulty( );

	Result result = complexSkillCheck( &rpgGame, &character, other, SKL_ANIMALS, 0, difficulty );

	addReplacementString( "NAME", rpgGame.arrStats[other].name );
	addReplacementString( "SUBJECT_PRONOUN", rpgGame.arrStats[other].subjectPronoun );
	addReplacementString( "OBJECT_PRONOUN", rpgGame.arrStats[other].objectPronoun );
	addReplacementString( "POSSESSIVE_PRONOUN", rpgGame.arrStats[other].possessivePronoun );

	switch( result ) {
	case R_SUCCESS:
		if( hobHelp && ( balancedRandom( 3 ) == 0 ) ) {
			appendToSceneDisplayString( "[NAME] brings you lunch as you enter the house. The house was almost completely clean already, and that creaky floor board was easy to fix." );
		} else {
			appendToSceneDisplayString( "[NAME] is just finishing up cleaning and has something delicious cooking." );
		}
		sceneIncreaseDiscipline( FS_FARMHOUSE );
		break;
	case R_COSTLY_SUCCESS:
		if( hobTroubles && ( balancedRandom( 3 ) == 0 ) ) {
			appendToSceneDisplayString( "[NAME] cleaned the house but found that knife they were going to use for cooking had gone missing. They found a duller one to use but [SUBJECT_PRONOUN] ended up cutting [OBJECT_PRONOUN] hand." );
			sceneIncreaseDiscipline( FS_FARMHOUSE );
			sceneInflictWound( other, "got a nasty cut" );
		} else {
			appendToSceneDisplayString( "[NAME] cleaned up the house, but while trying to fix the attic window slipped and twisted their ankle." );
			sceneIncreaseDiscipline( FS_FARMHOUSE );
			sceneInflictWound( other, "hurt [POSSESSIVE_PRONOUN] ankle" );
		}
		break;
	case R_FAILURE:
		if( hobTroubles && ( balancedRandom( 3 ) == 0 ) ) {
			appendToSceneDisplayString( "[NAME] spent most of the morning struggling to find any of the tools then needed to fix the issue with the stove. Then when starting out found that it had been started." );
			sceneInflictWound( other, "was burnt" );
		} else {
			appendToSceneDisplayString( "[NAME] spent the morning trying to get a stubborn stain out of the carpet." );
			sceneInflictWound( other, "is frustrated" );
		}
		break;
	}
}

void applyOtherRestReport( Stats other )
{
	// they just relax and heal a wound

	addReplacementString( "NAME", rpgGame.arrStats[other].name );
	addReplacementString( "SUBJECT_PRONOUN", rpgGame.arrStats[other].subjectPronoun );
	addReplacementString( "OBJECT_PRONOUN", rpgGame.arrStats[other].objectPronoun );
	addReplacementString( "POSSESSIVE_PRONOUN", rpgGame.arrStats[other].possessivePronoun );

	int type = balancedRandom( 3 );
	switch( type ) {
	case 0:
		// the baby sitter likes this
		gameCharacterData.daysWithoutFamilyInteraction = 0;
		appendToSceneDisplayString( "[NAME] spent the morning playing with [BABY_NAME]." );
		break;
	case 1:
		appendToSceneDisplayString( "[NAME] spent the morning reading [POSSESSIVE_PRONOUN] favorite book." );
		break;
	case 2:
		appendToSceneDisplayString( "[NAME] spent the morning trying to play the guitar. From what you heard they still haven't succeeded." );
		break;
	}
	sceneHealWound( other );
}

void applyReport( Stats other )
{
	FarmStatus status;
	if( other == STAT_SPOUSE ) {
		status = gameCharacterData.spouseBusyWith;
	} else if( other == STAT_CHILD ) {
		status = gameCharacterData.childBusyWith;
	} else {
		return;
	}

	appendToSceneDisplayString( "\n\n" );
	switch( status ) {
	case FS_ANIMALS:
		applyOtherAnimalsReport( other );
		break;
	case FS_CROPS:
		applyOtherCropsReport( other );
		break;
	case FS_FARMHOUSE:
		applyOtherFarmHouseReport( other );
		break;
	default:
		applyOtherRestReport( other );
		break;
	}
}

void sendOtherScene( CLOSURE( void )* closure, bool firstTimeRun )
{
	if( firstTimeRun ) {
		Stats who;
		FarmStatus where;

		va_list va;
		va_start( va, closure->args ); {
			who = va_arg( va, Stats );
			where = va_arg( va, FarmStatus );
		} va_end( va );


		// just some text saying you sent them then marking them for resolution during the afternoon
		addReplacementString( "NAME", rpgGame.arrStats[who].name );
		addReplacementString( "SUBJECT_PRONOUN", rpgGame.arrStats[who].subjectPronoun );
		addReplacementString( "PLACE", rpgGame.arrSkills[where].description );
		if( where == FS_FARMHOUSE ) {
			setInitialSceneDescriptionString( "You ask [NAME] to go take care of the [PLACE]. [SUBJECT_PRONOUN] agrees and starts looking around for things to do." );
		} else {
			setInitialSceneDescriptionString( "You ask [NAME] to go take care of the [PLACE]. [SUBJECT_PRONOUN] agrees and heads out after getting ready." );
		}

		if( who == STAT_SPOUSE ) {
			gameCharacterData.spouseBusyWith = where;
		} else if( who == STAT_CHILD ) {
			gameCharacterData.childBusyWith = where;
		}
	}

	startScene( );

	pushSimpleChoice( "Say thanks and continue planning.", closure_Make( morningScene, 0 ) );

	defaultSceneLoop( );
}

void hungryManDriveOff( CLOSURE( void )* closure, bool firstTimeRun )
{
	if( firstTimeRun ) {
		// the fairy thinks this is hilarious
		removeFlagFromCompanion( &character, CMP_FAIRY, CF_ANGERED );
		addFlagToCompanion( &character, CMP_FAIRY, CF_APPEASED );

		setInitialSceneDescriptionString( "You grab the hidden fire poker, raising it above your head yelling at the small man to leave before you trash him. "
			"The man yelps and you see stain forming on the front of his trousers and he stumbles backwards. Quickly regaining his feet he starts screaming "
			"for help and scarmbling off into the dark.\n\n"
			"You shut the door and go back to your dinner." );

		if( isCompanionWithCharacter( &rpgGame, &character, CMP_FAIRY ) ) {
			appendToSceneDisplayString( "\n\nYou hear some high pitched laughter from somewhere." );
		}
	}

	startScene( );

	pushSimpleChoice( "Go to bed.", closure_Make( startDayScene, 0 ) );

	defaultSceneLoop( );
}

void hungryManLeave( CLOSURE( void )* closure, bool firstTimeRun )
{
	if( firstTimeRun ) {
		setInitialSceneDescriptionString( "You tell him firmly that you won't give him any food and that he should leave now. He looks at the ground and wishes "
			"you a good evening before disappearing into the night.\n\n"
			"You shut the door and go back to your dinner." );
	}

	startScene( );

	pushSimpleChoice( "Go to bed.", closure_Make( startDayScene, 0 ) );

	defaultSceneLoop( );
}

void hungryManGiveFood( CLOSURE( void )* closure, bool firstTimeRun )
{
	if( firstTimeRun ) {
		addFlagToCompanion( &character, CMP_HUNGRY, CF_APPEASED );
		setInitialSceneDescriptionString( "You call for [SPOUSE_NAME] to grab an extra loaf of bread you have. Handing it to the small man his eyes go wide "
			"and he thanks you profusely. Bowing and saying all sorts of strange blessings. He starts backing off, still bowing until he's a few yards away from "
			"your door and swallows the entire loaf in one go. He then walks off into the night.\n\n"
			"You shut the door and go back to your dinner." );
	}

	startScene( );

	pushSimpleChoice( "Go to bed.", closure_Make( startDayScene, 0 ) );

	defaultSceneLoop( );
}

void hungryManBeg( CLOSURE( void )* closure, bool firstTimeRun )
{
	if( firstTimeRun ) {
		setInitialSceneDescriptionString( "The man looks at you with begging eyes. Though he seems to be paying more attention to the piece of bread in your hand "
			"and asks if not a meal then a handout of a loaf, or even half a loaf of bread. That even that meager amount would help him until he can find another "
			"kind soul. His yellow eyes are focused on the bread as he wipes drool from his mouth.\n\n"
			"What do you do?" );
	}

	startScene( );

	pushSimpleChoice( "Give him some bread.", closure_Make( hungryManGiveFood, 0 ) );
	pushSimpleChoice( "Grab your fire poker and threaten him.", closure_Make( hungryManDriveOff, 0 ) );
	pushSimpleChoice( "Tell him to leave.", closure_Make( hungryManLeave, 0 ) );

	defaultSceneLoop( );
}

void hungryManLetIn( CLOSURE( void )* closure, bool firstTimeRun )
{
	if( firstTimeRun ) {
		addFlagToCompanion( &character, CMP_HUNGRY, CF_APPEASED );
		setInitialSceneDescriptionString( "You let him in, hiding the fire poker and make a spot for him at the table. He is polite, making small talk while "
			"constantly eying the food.\n\n"
			"After you finish the evening prayers and start eating he begins devouring the food, eating enough for someone four "
			"time his size. All of you watch in wonder, disguist, or something between as he eats. Eventually he slows down and continues speaking with all of you.\n\n"
			"He is pleasent company, sharing local gossip, but nothing too scandulous while the [CHILD_NAME] is awake. Once the moon is high in the sky he excuses "
			"himself and thanks you for the meal. He bows before walking off into the night." );
	}

	startScene( );

	pushSimpleChoice( "Go to bed.", closure_Make( startDayScene, 0 ) );

	defaultSceneLoop( );
}

void hungryManIntro( CLOSURE( void )* closure, bool firstTimeRun )
{
	if( firstTimeRun ) {
		setInitialSceneDescriptionString( "Shortly after sitting down for dinner you hear a knock on your door. A piece of bread still in hand you grab the fire poker and set it next to the door before "
			"opening it slightly. Outside you see a short man,\n\n"
			"He's wearing raggedy clothes and you can see his bones moving underneath his skin. His hair is messy and almost hides vivid "
			"yellow eyes. Drool is dripping down his mouth as he seems to breathing in the smells from the house.\n\n"
			"You clear your throat and ask him what he wants. This knocks him out "
			"of his trance. He bows his head and politely asks if he might be let in to share your meal, as he is famished and does not remember the last time he ate.\n\n"
			"What do you do?" );
	}

	startScene( );

	pushSimpleChoice( "Tell him that you won't let him in.", closure_Make( hungryManBeg, 0 ) );
	pushSimpleChoice( "Grab your fire poker and threaten him.", closure_Make( hungryManDriveOff, 0 ) );
	pushSimpleChoice( "Let him in.", closure_Make( hungryManLetIn, 0 ) );

	defaultSceneLoop( );
}

void cannibalLetIn( CLOSURE( void )* closure, bool firstTimeRun )
{
	// oh no, you're dead
	topTitle = " The Feast of the Torathar ";
	startDraw( window ); {
		drawString( window, 2, 2, safeWriteArea,
			"You open the door, hiding the fire poker behind you as the short man smiles and enters your home.\n\n"
			"As he walks past you a sudden warmth starts spreading through your stomach. Someone screams as you look down and see a red stain spreading "
			"on your clothes. Some pink thing exits from the new hole in your body. As you numbly try to put your intestines back in "
			"you glance to the side. [SPOUSE_NAME] is kneeling down, but [SPOUSE_OBJECT_PRONOUN] head lies on the floor nearby as the man has clamped his "
			"mouth around the neck stump, drinking deeply.\n\n"
			"The last thing you experience is hearing something laughing as a child is screaming.\n\n\n"
			"The End",
			attr_Clrs( PDC_GREY, PDC_BLACK ) );

		centerStringHoriz( window, safeWriteArea, safeWriteArea.bottom, "Press any key to return to the title screen", attr_Clrs( PDC_YELLOW, PDC_BLACK ) );
	} endDraw( window );

	waitForAnyInput( window );

	switchScene( closure_Make( titleScene, 0 ) );
}

void cannibalDriveOff( CLOSURE( void )* closure, bool firstTimeRun )
{
	if( firstTimeRun ) {
		setInitialSceneDescriptionString( "You scowl at the main and grab the hidden fire poker. Thrusting it at him and telling him to get the hell off your property. "
			"He jumps back from the poker and glowers at you. It isn't long before he backs off, makes a few rude gestures, and walks off into the night." );
	}

	startScene( );

	pushSimpleChoice( "Lock everything tight and go to bed.", closure_Make( startDayScene, 0 ) );

	defaultSceneLoop( );
}

void cannibalDeny( CLOSURE( void )* closure, bool firstTimeRun )
{
	if( firstTimeRun ) {
		setInitialSceneDescriptionString( "His smile lessens a bit, and he asks if this is how you treat all strangers. His hand slowly creeps around the door frame and "
			"you can see his long, rust stained nails. He doesn't seem to want to take no as an answer, talking of the dangers nights in the wilderness can hold. He gives "
			"you a piteous look and asks if you'd want his blood on your hands. His eyes flit to the dinner table occasionally.\n\n"
			"What do you do?" );
	}

	startScene( );

	pushSimpleChoice( "Grab your fire poker and threaten him.", closure_Make( cannibalDriveOff, 0 ) );
	pushSimpleChoice( "Let him in.", closure_Make( cannibalLetIn, 0 ) );

	defaultSceneLoop( );
}

void cannibalKnockIntro( CLOSURE( void )* closure, bool firstTimeRun )
{
	if( firstTimeRun ) {
		setInitialSceneDescriptionString( "Shortly after sitting down for dinner you hear a knock on your door. You grab the fire poker and set it next to the door before "
			"opening it slightly. Outside you see a short man. He's wearing clean clothes and grins at you. His teeth are sharp and his eyes nearly glow red.\n\nHe bows his "
			"head and wonders if you might have a place for him to stay the night, or if not that at least a meal. He was stranded and needs to rest before contacting a "
			"mechanic in the morning. The smile never leaves his face.\n\n"
			"What do you do?" );
	}

	startScene( );

	pushSimpleChoice( "Tell him that you won't let him in.", closure_Make( cannibalDeny, 0 ) );
	pushSimpleChoice( "Grab your fire poker and threaten him.", closure_Make( cannibalDriveOff, 0 ) );
	pushSimpleChoice( "Let him in.", closure_Make( cannibalLetIn, 0 ) );

	defaultSceneLoop( );
}

void prankLaughNoFairy( CLOSURE( void )* closure, bool firstTimeRun )
{
	if( firstTimeRun ) {
		setInitialSceneDescriptionString( "You can't help but join [CHILD_NAME] in laughing. Filling boots with cheese is pretty absurd. After you're both done "
			"laughing you look at [CHILD_OBJECT_PRONOUN] with a grin. You ask [CHILD_OBJECT_PRONOUN] to help clean you boots and while doing that the both of you can figure out "
			"how to get [SPOUSE_NAME] next. [CHILD_NAME] grins and starts digging gobs of oatmeal out.\n\n"
			"Both of you act completely innocent when [SPOUSE_NAME] passes by an hour later." );
	}

	startScene( );

	pushSimpleChoice( "Clean your boots and plot.", closure_Make( switchToNextPartOfDay, 0 ) );

	defaultSceneLoop( );
}

void prankLaughFairy( CLOSURE( void )* closure, bool firstTimeRun )
{
	if( firstTimeRun ) {
		// the fairy enjoyed that you enjoyed the prank
		addFlagToCompanion( &character, CMP_FAIRY, CF_APPEASED );
		removeFlagFromCompanion( &character, CMP_FAIRY, CF_ANGERED );

		setInitialSceneDescriptionString( "You can't help but join [CHILD_NAME] in laughing. Filling boots with cheese is pretty absurd. After you're both done "
			"laughing you look at [CHILD_OBJECT_PRONOUN] with a grin. You ask [CHILD_OBJECT_PRONOUN] to help clean you boots and while doing that the both of you can figure out "
			"how to get [SPOUSE_NAME] next. [CHILD_NAME] grins and starts digging gobs of cheese out.\n\n"
			"Both of you act completely innocent when [SPOUSE_NAME] passes by an hour later" );
	}

	startScene( );

	pushSimpleChoice( "Clean your boots and plot.", closure_Make( switchToNextPartOfDay, 0 ) );

	defaultSceneLoop( );
}

void prankLaughAndScoldNoFairy( CLOSURE( void )* closure, bool firstTimeRun )
{
	if( firstTimeRun ) {
		setInitialSceneDescriptionString( "You can't help but join [CHILD_NAME] in laughing. Filling boots with oatmeal is pretty absurd. As [CHILD_SUBJECT_PRONOUN] "
			"starts to leave you grab [CHILD_OBJECT_PRONOUN] shirt. You glare and say [CHILD_SUBJECT_PRONOUN] helped make the mess so [CHILD_SUBJECT_PRONOUN] can help "
			"clean it.\n\n"
			"[CHILD_NAME] looks wide eyed at you, feigning innoccence and saying it wasn't them.\n\n"
			"You ask who could have done it then, [BABY_NAME] is too small, and you were with [SPOUSE_NAME] all morning. Not only that, but [CHILD_SUBJECT_PRONOUN] is the only "
			"one with hands small enough to reach into the tips of the boots. So even if ghosts did it then [CHILD_SUBJECT_PRONOUN]'ll still have to help. [CHILD_NAME] pouts "
			"but doesn't talk back." );
	}

	startScene( );

	pushSimpleChoice( "Clean your boots.", closure_Make( switchToNextPartOfDay, 0 ) );

	defaultSceneLoop( );
}

void prankLaughAndScoldFairy( CLOSURE( void )* closure, bool firstTimeRun )
{
	if( firstTimeRun ) {
		// the fairy enjoyed that you enjoyed the prank
		addFlagToCompanion( &character, CMP_FAIRY, CF_APPEASED );
		removeFlagFromCompanion( &character, CMP_FAIRY, CF_ANGERED );

		setInitialSceneDescriptionString( "You can't help but join [CHILD_NAME] in laughing. Filling boots with cheese is pretty absurd. As [CHILD_SUBJECT_PRONOUN] "
			"starts to leave you grab [CHILD_OBJECT_PRONOUN] shirt. You glare and say [CHILD_SUBJECT_PRONOUN] helped make the mess so [CHILD_SUBJECT_PRONOUN] can help "
			"clean it.\n\n"
			"[CHILD_NAME] looks angry and says that they didn't do it. That there was no way [CHILD_SUBJECT_PRONOUN] could find much less melt "
			"that much cheese without someone noticing.\n\n"
			"You ask who could have done it then, [BABY_NAME] is too small, and you were with [SPOUSE_NAME] all morning. Not only that, but [CHILD_SUBJECT_PRONOUN] is the only "
			"one with hands small enough to reach into the tips of the boots. So even if ghosts did it then [CHILD_SUBJECT_PRONOUN]'ll still have to help. [CHILD_NAME] pouts "
			"but doesn't talk back." );

		if( character.arrDisciplineLevels[FS_FAMILY] > 6 ) {
			// people don't like being blamed for stuff they didn't do
			sceneDecreaseDiscipline( FS_FAMILY );
		}
	}

	startScene( );

	pushSimpleChoice( "Clean your boots.", closure_Make( switchToNextPartOfDay, 0 ) );

	defaultSceneLoop( );
}

void prankYellAtChildNoFairy( CLOSURE( void )* closure, bool firstTimeRun )
{

	if( firstTimeRun ) {
		setInitialSceneDescriptionString( "[CHILD_NAME] needs to learn what is acceptable and what isn't. A small prank is fine but this crosses the line. "
			"You sternly tell [CHILD_OBJECT_PRONOUN] that what they did wasn't funny. That you now have to spend all morning cleaning up this mess and your "
			"boots won't be right for a month or more.\n\n"
			"[CHILD_SUBJECT_PRONOUN] looks at their shoes as you yell at [CHILD_OBJECT_PRONOUN]. After you've finished [CHILD_SUBJECT_PRONOUN] apologizes and offers to help clean.\n\n"
			"You are slightly taken a back, but smile and apologize for yelling, but reiterate that this sort of thing is unaccetable." );
	}

	startScene( );

	pushSimpleChoice( "Clean your boots.", closure_Make( switchToNextPartOfDay, 0 ) );

	defaultSceneLoop( );
}

void prankYellAtChildFairy( CLOSURE( void )* closure, bool firstTimeRun )
{
	if( firstTimeRun ) {
		// the fairy thought the cheese was hilarious, she'll just need something funnier (read: more dangerous) next time
		addFlagToCompanion( &character, CMP_FAIRY, CF_ANGERED );
		removeFlagFromCompanion( &character, CMP_FAIRY, CF_APPEASED );

		setInitialSceneDescriptionString( "[CHILD_NAME] needs to learn what is acceptable and what isn't. A small prank is fine but this crosses the line. "
			"You sternly tell [CHILD_OBJECT_PRONOUN] that what they did wasn't funny. That you now have to spend all morning cleaning up this mess and your "
			"boots won't be right for a month or more.\n\n"
			"[CHILD_SUBJECT_PRONOUN] insists that it wasn't [CHILD_OBJECT_PRONOUN]. That there was no way [CHILD_SUBJECT_PRONOUN] could find much less melt "
			"that much cheese without someone noticing.\n\n"
			"You ground [CHILD_OBJECT_PRONOUN] and get to cleaning your boots.");

		if( character.arrDisciplineLevels[FS_FAMILY] > 2 ) {
			// people don't like being blamed for stuff they didn't do
			sceneDecreaseDiscipline( FS_FAMILY );
		}
	}

	startScene( );

	pushSimpleChoice( "Clean your boots.", closure_Make( switchToNextPartOfDay, 0 ) );

	defaultSceneLoop( );
}

void prankIntro( CLOSURE( void )* closure, bool firstTimeRun )
{
	if( firstTimeRun ) {

		addReplacementString( "FILLING", "oatmeal" );
		addReplacementString( "ADDITIONAL_FILLING_TEXT", ", there are at least three breakfasts worth in there" );
		addReplacementString( "ADDITIONAL_LAUGHTER", "" );
		if( isCompanionWithCharacter( &rpgGame, &character, CMP_FAIRY ) ) {
			addReplacementString( "FILLING", "cheese" );
			addReplacementString( "ADDITIONAL_LAUGHTER", "[CHILD_OBJECT_PRONOUN] laughter echoes down from the ceiling. " );
			addReplacementString( "ADDITIONAL_FILLING_TEXT", ", like someone had melted a block of cheddar and poured it into them" );
		}
		setInitialSceneDescriptionString( "After breakfast you go to put on your boots when you feel something slimey in them. Pulling your feet out "
			"you see your socks are covered in [FILLING]. Looking inside your boots you see they're completely filled with [FILLING][ADDITIONAL_FILLING_TEXT]. "
			"[CHILD_NAME] bursts out laughing after seeing the disgusted look on your face. [ADDITIONAL_LAUGHTER]It's going to take "
			"you all morning to clean this out."
			"\n\nWhat do you do?" );
	}

	startScene( );

	if( isCompanionWithCharacter( &rpgGame, &character, CMP_FAIRY ) ) {
		pushSimpleChoice( "Yell at [CHILD_NAME].", closure_Make( prankYellAtChildFairy, 0 ) );
		pushSimpleChoice( "Laugh it off, but scold [CHILD_NAME].", closure_Make( prankLaughAndScoldFairy, 0 ) );
		pushSimpleChoice( "Laugh it off and get to cleaning your boots.", closure_Make( prankLaughFairy, 0 ) );
	} else {
		pushSimpleChoice( "Yell at [CHILD_NAME].", closure_Make( prankYellAtChildNoFairy, 0 ) );
		pushSimpleChoice( "Laugh it off, but scold [CHILD_NAME].", closure_Make( prankLaughAndScoldNoFairy, 0 ) );
		pushSimpleChoice( "Laugh it off and get to cleaning your boots.", closure_Make( prankLaughNoFairy, 0 ) );
	}

	defaultSceneLoop( );
}

// the day will be divided up into two parts, morning and evening
void startDayScene( CLOSURE( void )* closure, bool firstTimeRun )
{
	gameCharacterData.currentDay += 1;

	// run all the day update stuff
	gameCharacterData.daysWithoutFamilyInteraction += 1;
	gameCharacterData.daysWithoutHouseMilk += 1;

	// reset family busy flags
	gameCharacterData.spouseBusyWith = NUM_FARM_STATUS;
	gameCharacterData.childBusyWith = NUM_FARM_STATUS;
	gameCharacterData.currentLocation = LOC_FARMHOUSE;

	// two or more days without family interaction will disappoint the Derbshiur
	if( gameCharacterData.daysWithoutFamilyInteraction >= 2 ) {
		removeFlagFromCompanion( &character, CMP_BABY_SITTER, CF_APPEASED );
	} else {
		addFlagToCompanion( &character, CMP_BABY_SITTER, CF_APPEASED );
	}

	// three or more days without leaving milk will disappoint the Timthirid, 5 or more days will anger them
	if( !isFlagSetOnCompanion( &character, CMP_HOUSE_HOB, CF_ALWAYS_ANGERED ) ) {
		if( gameCharacterData.daysWithoutHouseMilk >= 5 ) {
			removeFlagFromCompanion( &character, CMP_HOUSE_HOB, CF_APPEASED );
			addFlagToCompanion( &character, CMP_HOUSE_HOB, CF_ANGERED );
		} else if( gameCharacterData.daysWithoutHouseMilk >= 3 ) {
			removeFlagFromCompanion( &character, CMP_HOUSE_HOB, CF_APPEASED );
			removeFlagFromCompanion( &character, CMP_HOUSE_HOB, CF_ANGERED );
		} else {
			removeFlagFromCompanion( &character, CMP_HOUSE_HOB, CF_ANGERED );
			addFlagToCompanion( &character, CMP_HOUSE_HOB, CF_APPEASED );
		}
	}

	// see if any part of the farm has deteriorated, various fae will decrease this
	checkForFarmStatusDecay( FS_FARMHOUSE, CMP_HOUSE_HOB ); // makes small repairs around the house
	checkForFarmStatusDecay( FS_ANIMALS, CMP_BUILDER ); // takes care of the animals and the barn
	checkForFarmStatusDecay( FS_CROPS, CMP_DRYAD ); // improves the health of the crops
	checkForFarmStatusDecay( FS_FAMILY, CMP_BABY_SITTER ); // helps keep the baby quiet

	// if the well is dirty or the water fae has not been appeased then the family will get sick
	if( isFlagOn( character.flags, CF_DIRTY_WELL ) ) {
		sceneInflictWound( STAT_SELF, "feel weak and ill" );
		sceneInflictWound( STAT_SPOUSE, "feels weak and ill" );
		sceneInflictWound( STAT_CHILD, "feels weak and ill" );
	} else {
		// everyone has a 50% chance that a wound will heal, 60% if you have the baby sitter
		int chanceHeal = areFlagsSetOnCompanion( &character, CMP_BABY_SITTER, createFlagSet( 2, CF_THERE, CF_APPEASED ) ) ? 60 : 50;
		for( int i = 0; i < NUM_STATS; ++i ) {
			if( ( balancedRandom( 100 ) + 1 ) <= chanceHeal ) {
				sceneHealWound( i );
			}
		}
	}

	// reset the fae action for the morning scene
	character.flags = turnOffFlag( character.flags, CF_PERFORMED_FAE_ACTION );

	character.flags = turnOffFlag( character.flags, CF_IS_AFTERNOON );

	if( gameCharacterData.currentDay == gameCharacterData.wellDay ) {
		character.flags = turnOnFlag( character.flags, CF_DIRTY_WELL );
	}

	if( isDead( ) ) {
		// chance someone dies from bad water
		switchScene( closure_Make( deathScene, 0 ) );
		return;
	}

	// if the farm has fallen apart then die here
	for( int i = 0; i < NUM_FARM_STATUS; ++i ) {
		if( character.arrDisciplineLevels[i] <= 0 ) {
			switchScene( closure_Make( deathScene, 0 ) );
			return;
		}
	}

	if( gameCharacterData.currentDay == gameCharacterData.prankDay ) {
		switchScene( closure_Make( prankIntro, 0 ) );
		return;
	} else {
		switchScene( closure_Make( morningScene, 0 ) );
		return;
	}
}

void addSendOtherChoice( const char* desc, Stats who, FarmStatus where )
{
	CLOSURE( void )* sendOut = closure_Make( sendOtherScene, sizeof( Stats ) + sizeof( FarmStatus ) );
	closure_PushArg( sendOut, &who, sizeof( Stats ) );
	closure_PushArg( sendOut, &where, sizeof( FarmStatus ) );
	pushSimpleChoice( desc, sendOut );
}

void morningScene( CLOSURE( void )* closure, bool firstTimeRun )
{
	if( firstTimeRun ) {
		bool hobAppeased = areFlagsSetOnCompanion( &character, CMP_HOUSE_HOB, createFlagSet( 2, CF_THERE, CF_APPEASED ) );

		if( hobAppeased ) {
			addReplacementString( "CLEANING", "After eating you clean up and brush away some stray acorns that got into the house" );
		} else {
			addReplacementString( "CLEANING", "After eating you clean up" );
		}
		setInitialSceneDescriptionString( "You wake up before the sun has risen has prepare for your day. After taking care of some "
			"morning necessities you and [SPOUSE_NAME] make breakfast. [CLEANING]. ");

		if( gameCharacterData.currentDay == gameCharacterData.wellDay ) {
			appendToSceneDisplayString( "The water you got from the well smells and tastes a bit odd. " );
		}

		if( isFlagOn( character.flags, CF_DIRTY_WELL ) ) {
			// well wasn't taken care of
			appendToSceneDisplayString( "The water you got from the well smells and tastes rancid. {fg=Red}Everybody is sick.{/fg} " );
		}

		appendToSceneDisplayString( "\n\nWhat will you do this morning?" );
	}

	startScene( );

	// check to see if there are any special events this day
	if( ( gameCharacterData.currentDay >= gameCharacterData.treeDay ) && !isFlagOn( character.flags, CF_CUT_TREES ) ) {
		pushSimpleChoice( "Clear out some of the nearby forest for cropland.", closure_Make( youTreeScene, 0 ) );
	}

	if( isFlagOn( character.flags, CF_DIRTY_WELL ) ) {
		pushSimpleChoice( "Check why the well water has been tasting funny.", closure_Make( youWellScene, 0 ) );
	}

	pushSimpleChoice( "Go take care of the animals.", closure_Make( youAnimalsScene, 0 ) );
	pushSimpleChoice( "Go take care of the crops.", closure_Make( youCropsScene, 0 ) );
	pushSimpleChoice( "Spend time cleaning and repairing the house.", closure_Make( youFarmHouseScene, 0 ) );

	// morning, can ask for help
	if( gameCharacterData.spouseBusyWith == NUM_FARM_STATUS ) {
		// spouse isn't busy, ask them to go do something
		addSendOtherChoice( "Ask [SPOUSE_NAME] to take care of the animals.", STAT_SPOUSE, FS_ANIMALS );
		addSendOtherChoice( "Ask [SPOUSE_NAME] to take care of the crops.", STAT_SPOUSE, FS_CROPS );
		addSendOtherChoice( "Ask [SPOUSE_NAME] to take care of the house.", STAT_SPOUSE, FS_FARMHOUSE );
	}

	if( gameCharacterData.childBusyWith == NUM_FARM_STATUS ) {
		// child isn't busy, ask them to go do something
		addSendOtherChoice( "Ask [CHILD_NAME] to take care of the animals.", STAT_CHILD, FS_ANIMALS );
		addSendOtherChoice( "Ask [CHILD_NAME] to take care of the crops.", STAT_CHILD, FS_CROPS );
		addSendOtherChoice( "Ask [CHILD_NAME] to take care of the house.", STAT_CHILD, FS_FARMHOUSE );
	}

	addFaeActionOptions( LOC_FARMHOUSE );

#ifdef _DEBUG
	storedResult = R_SUCCESS;
	pushSimpleChoice( "Read", closure_Make( readSceneOne, 0 ) );
#endif

	defaultSceneLoop( );
}

void afternoonScene( CLOSURE( void )* closure, bool firstTimeRun )
{
	if( firstTimeRun ) {
		bool sitterAngry = isCompanionWithCharacter( &rpgGame, &character, CMP_BABY_SITTER ) && !isFlagSetOnCompanion( &character, CMP_BABY_SITTER, CF_APPEASED );
		bool sitterHappy = isCompanionWithCharacter( &rpgGame, &character, CMP_BABY_SITTER ) && isFlagSetOnCompanion( &character, CMP_BABY_SITTER, CF_APPEASED );

		addReplacementString( "SITTER_ANGRY", "" );
		addReplacementString( "SITTER_HAPPY", "" );
		if( sitterAngry ) {
			addReplacementString( "SITTER_ANGRY", " after clearing some spiderwebs off of the dishes" );
		}
		if( sitterHappy ) {
			addReplacementString( "SITTER_HAPPY", " You can hear [BABY_NAME] laughing from [BABY_POSSESSIVE_PRONOUN] room. " );
		}

		// spouse and child will return from what they were doing if they were asked to do something
		setInitialSceneDescriptionString( "You grab some food[SITTER_ANGRY] and sit down to eat lunch.[SITTER_HAPPY]" );

		appendToSceneDisplayString( arrReportString );

		appendToSceneDisplayString( "\n\nAfter finishing lunch [CHILD_NAME] sits down to start studying, [SPOUSE_NAME] starts some other daily chores, and you try to decide what to do." );

		appendToSceneDisplayString( "\n\nWhat will you do this afternoon?" );
	}

	startScene( );

	if( ( gameCharacterData.currentDay >= gameCharacterData.treeDay ) && !isFlagOn( character.flags, CF_CUT_TREES ) ) {
		pushSimpleChoice( "Clear out some of the nearby forest for cropland.", closure_Make( youTreeScene, 0 ) );
	}

	if( isFlagOn( character.flags, CF_DIRTY_WELL ) ) {
		pushSimpleChoice( "Check why the well water has been tasting funny.", closure_Make( youWellScene, 0 ) );
	}

	pushSimpleChoice( "Go take care of the animals.", closure_Make( youAnimalsScene, 0 ) );
	pushSimpleChoice( "Go take care of the crops.", closure_Make( youCropsScene, 0 ) );
	pushSimpleChoice( "Spend time cleaning and repairing the house.", closure_Make( youFarmHouseScene, 0 ) );
	pushSimpleChoice( "Spend some time with your family.", closure_Make( youFamilyScene, 0 ) );

	addFaeActionOptions( LOC_FARMHOUSE );

	defaultSceneLoop( );
}

void switchToAfternoon( CLOSURE( void )* closure, bool firstTimeRun )
{
	clearSceneDisplayString( );

	// do whatever the other family members were doing, append to the report string
	applyReport( STAT_SPOUSE );
	applyReport( STAT_CHILD );

	arrfree( arrReportString );
	arrReportString = createCopyOfSceneDisplayString( );

	// chance someone dies
	if( isDead( ) ) {
		switchScene( closure_Make( deathScene, 0 ) );
	}

	// set any flags we need to
	character.flags = turnOnFlag( character.flags, CF_IS_AFTERNOON );
	character.flags = turnOffFlag( character.flags, CF_PERFORMED_FAE_ACTION );

	switchScene( closure_Make( afternoonScene, 0 ) );
}

void goodEnd( CLOSURE( void )* closure, bool firstTimeRun )
{
	Companions helpingFae = getRandomAppeasedFae( );

	if( helpingFae != INVALID_COMPANION ) {
		addReplacementString( "FAE_DESCRIPTION", rpgGame.arrCompanions[helpingFae].description );
	}

	topTitle = " Mysterious Saviors ";
	startDraw( window ); {
		drawString( window, 2, 2, safeWriteArea,
			"You're awoken in the middle of the night by the sound of breaking glass. [SPOUSE_NAME] shoots up at the same time you do. You tell [SPOUSE_OBJECT_PRONOUN] "
			"to grab [CHILD_NAME] and get to the nearest neighbor. You make your way through the house as quietly and quickly as possible, eventually getting "
			"to the living room and grab your rifle.\n\n"
			"Lying on the ground, bleeding out is a short man-like thing with a wide grin and red eyes. In it's left claw it's crushed [FAE_DESCRIPTION]. Looking up at "
			"you the bleeding man coughs and insults your mother, then threatens [BABY_NAME]. You can see that he broke through the window and you doubt his "
			"intentions were good. You aim the rifle at his head and pull the trigger. [BABY_NAME] starts crying.\n\n"
			"The next morning [SPOUSE_NAME] and [CHILD_NAME] return. You tell them what happened.\n\n"
			"They are overjoyed that [BABY_NAME] is safe and sound, but wonder what attacked the man.\n\n"
			"A week after reporting this to the police a black vehicle pulls up to your house. Grim men, grey suited men step out and make you sign a document "
			"forbidding anyone in your family from telling anybody about this under penalty of death. They give you a nice sum of money for being cooperative.\n\n"
			"The next few months are stressful. Every sound makes you jump. Slowly you get back into your routine. Knowing that your family is safe and that something "
			"out there is looking out for you and yours eases your worries.\n\n"
			"After years and years you and [SPOUSE_NAME] grow old and are eventually buried on the farm.\n\n\n"
			"The End",
			attr_Clrs( PDC_GREY, PDC_BLACK ) );

		centerStringHoriz( window, safeWriteArea, safeWriteArea.bottom, "Press any key to return to the title screen", attr_Clrs( PDC_YELLOW, PDC_BLACK ) );
	} endDraw( window );

	waitForAnyInput( window );

	switchScene( closure_Make( titleScene, 0 ) );
}

void badEnd( CLOSURE( void )* closure, bool firstTimeRun )
{
	Companions helpingFae = getRandomAppeasedFae( );

	addReplacementString( "INJURED_FAE", "" );
	if( helpingFae != INVALID_COMPANION ) {
		addReplacementString( "INJURED_FAE", "At his feet you see [FAE_DESCRIPTION] lying battered and bloody. " );
		addReplacementString( "FAE_DESCRIPTION", rpgGame.arrCompanions[helpingFae].description );
	}

	topTitle = " Kidnapped Baby ";
	startDraw( window ); {
		drawString( window, 2, 2, safeWriteArea,
			"You're awoken in the middle of the night by the sound of breaking glass. [SPOUSE_NAME] shoots up at the same time you do. You tell [SPOUSE_OBJECT_PRONOUN] "
			"to grab [CHILD_NAME] and get to the nearest neighbor. You make your way through the house as quietly and quickly as possible, eventually getting "
			"to the living room and grab your rifle.\n\n"
			"Making your way to where the sound is your blood runs cold as you realize it's [BABY_NAME]'s room. You charge forward, seeing a short man with red "
			"eyes and wide grin. In one of his hands he holds your baby. [INJURED_FAE][BABY_NAME] squirms and tries to cry but the man is covering [BABY_POSSESSIVE_PRONOUN] mouth.\n\n"
			"You can't get a clear shot. The short man uses your baby like a shield before jumping out the window and running into the night. You give chase but "
			"aren't fast enough.\n\n"
			"The next morning [SPOUSE_NAME] and [CHILD_NAME] return. You tell them what happened.\n\n"
			"A small funeral is held. [SPOUSE_NAME] wants to sell the farm and move somewhere safer. You agree and by the following month you are back in the city "
			"with your old job. Every noise in the night still makes you jump. Most nights [SPOUSE_NAME] cries in [SPOUSE_OBJECT_PRONOUN] sleep.\n\n\n"
			"The End",
			attr_Clrs( PDC_GREY, PDC_BLACK ) );

		centerStringHoriz( window, safeWriteArea, safeWriteArea.bottom, "Press any key to return to the title screen", attr_Clrs( PDC_YELLOW, PDC_BLACK ) );
	} endDraw( window );

	waitForAnyInput( window );

	switchScene( closure_Make( titleScene, 0 ) );
}

void maybeEndGood( CLOSURE( void )* closure, bool firstTimeRun )
{
	topTitle = " Mysterious Saviors ";
	startDraw( window ); {
		drawString( window, 2, 2, safeWriteArea,
			"The thing is still. Blood pooling out from under it. You drop the fire poker and go over to [BABY_NAME] making sure [BABY_SUBJECT_PRONOUN]'s ok.\n\n"
			"You wash up and get the body out of the house. It stinks and you leave it in the woods for the animals to eat. Cleaning the baby's room is going to be quite a chore.\n\n"
			"The next morning [SPOUSE_NAME] and [CHILD_NAME] return. You tell them what happened.\n\n"
			"They are overjoyed that [BABY_NAME] is safe and sound, but wonder what attacked the man.\n\n"
			"A week after reporting this to the police a black vehicle pulls up to your house. Grim men, grey suited men step out and make you sign a document "
			"forbidding anyone in your family from telling anybody about this under penalty of death. They give you a nice sum of money for being cooperative.\n\n"
			"The next few months are stressful. Every sound makes you jump. Slowly you get back into your routine. Knowing that your family is safe and that you "
			"can protect them eases your worries.\n\n"
			"After years and years you and [SPOUSE_NAME] grow old and are eventually buried on the farm.\n\n\n"
			"The End",
			attr_Clrs( PDC_GREY, PDC_BLACK ) );

		centerStringHoriz( window, safeWriteArea, safeWriteArea.bottom, "Press any key to return to the title screen", attr_Clrs( PDC_YELLOW, PDC_BLACK ) );
	} endDraw( window );

	waitForAnyInput( window );

	switchScene( closure_Make( titleScene, 0 ) );
}

void maybeEndBad( CLOSURE( void )* closure, bool firstTimeRun )
{
	topTitle = " At Least You Went Down Swinging ";
	startDraw( window ); {
		drawString( window, 2, 2, safeWriteArea,
			"The creature swipes at your face causing you to stumble backwards and fall. Before you can react he's on you, claws and teeth tearing you apart.\n\n"
			"As you lie bleeding on the floor you see him walk over to the crib and grab [BABY_NAME]. The last thing you see is him grinning and taking a large "
			"bite out of [BABY_OBJECT_PRONOUN].\n\n\n"
			"The End",
			attr_Clrs( PDC_GREY, PDC_BLACK ) );

		centerStringHoriz( window, safeWriteArea, safeWriteArea.bottom, "Press any key to return to the title screen", attr_Clrs( PDC_YELLOW, PDC_BLACK ) );
	} endDraw( window );

	waitForAnyInput( window );

	switchScene( closure_Make( titleScene, 0 ) );
}

static int timesWon;
void addMaybeFightChoices( );

void maybeFightSuccess( CLOSURE( void )* closure, bool firstTimeRun )
{
	if( firstTimeRun ) {

		timesWon += 1;

		if( timesWon == 3 ) {
			setInitialSceneDescriptionString( "This thing is fast but you think you have his number. Raising the poker as to swing he dodges to the side. You quickly pivot "
				"and throw a haymaker with your left hand, catching him square in the jaw and sending him sprawling. Without a thought you leap on top of him and bring down "
				"the fire poker on his head. Repeatedly. Eventually he stops twitching.");
		} else {
			switch( balancedRandom( 3 ) ) {
			case 0:
				setInitialSceneDescriptionString( "You swing the poker at him a few times and he ducks them. You start to repeat with the next swing but change angle mid-swing "
					"cracking him hard in the head. He backs away and his red eyes glow brighter." );
				break;
			case 1:
				setInitialSceneDescriptionString( "He swipes at you but you're able to use the poker to block his claws. As he's recovering from the attack you dart in with your "
					"left hand and grab his collar. Spinning in a full circle you send him flying into the wall. You move to finish him off but he scurries to his feet before "
					"you can get there." );
				break;
			case 2:
				setInitialSceneDescriptionString( "Ducking and weaving he tries to get close to you. A properly timed kick stuns him for a bit. You take advantage of this and "
					"put your full body into a swing. You hear some of his ribs break." );
				break;
			}
		}
	}

	startScene( );

	if( timesWon == 3 ) {
		pushSimpleChoice( "Check on [BABY_NAME].", closure_Make( maybeEndGood, 0 ) );
	} else {
		addMaybeFightChoices( );
	}

	defaultSceneLoop( );
}

void maybeFightCostlySuccess( CLOSURE( void )* closure, bool firstTimeRun )
{
	if( firstTimeRun ) {
		timesWon += 1;

		if( timesWon == 3 ) {
			setInitialSceneDescriptionString( "This thing is fast but you think you have his number. You bring the poker back to swing from the right. He dodges and leaps at you "
				"with extended claws. Once they sink in you bring your left arm around him and drop the poker. You bring your right hand down on his head. Repeatedly. Eventually "
				"he stops twitching." );
			sceneInflictWound( STAT_SELF, "flesh is clawed open" );
			sceneInflictWound( STAT_SELF, "flesh is clawed open" );
		} else {
			switch( balancedRandom( 3 ) ) {
			case 0:
				setInitialSceneDescriptionString( "You swing wildly. He ducks in and claws open your thigh. Your feet give out, but he's slow and you're able to catch him with a "
					"backswing." );
				sceneInflictWound( STAT_SELF, "flesh is clawed open" );
				sceneInflictWound( STAT_SELF, "flesh is clawed open" );
				break;
			case 1:
				setInitialSceneDescriptionString( "He moves quickly, ducking in and out of range. Eventually you overextend yourself and he capitalizes on this, leaving you with "
					"wounds on your arm. He gets greedy and tries to keep slashing at you but your able to catch him in the chin with an uppercut." );
				sceneInflictWound( STAT_SELF, "flesh is clawed open" );
				sceneInflictWound( STAT_SELF, "flesh is clawed open" );
				break;
			case 2:
				setInitialSceneDescriptionString( "He leaps at you grabbing onto your forearm with his teeth and claws. You swing him around wildly, eventually smashing him into a "
					"wall which causes him to let go.");
				sceneInflictWound( STAT_SELF, "were bitten" );
				sceneInflictWound( STAT_SELF, "flesh is clawed open" );
				break;
			}
		}

		// want a special death scene for this one
		if( isDead( ) ) {
			switchScene( closure_Make( maybeEndBad, 0 ) );
			return;
		}
	}

	startScene( );

	if( timesWon == 3 ) {
		pushSimpleChoice( "Check on [BABY_NAME].", closure_Make( maybeEndGood, 0 ) );
	} else {
		addMaybeFightChoices( );
	}

	defaultSceneLoop( );
}

void maybeFightFailure( CLOSURE( void )* closure, bool firstTimeRun )
{
	if( firstTimeRun ) {

		switch( balancedRandom( 3 ) ) {
		case 0:
			setInitialSceneDescriptionString( "You swing wildly. He's able to dodge your clumsy blows and claws open your calves." );
			sceneInflictWound( STAT_SELF, "flesh is clawed open" );
			sceneInflictWound( STAT_SELF, "flesh is clawed open" );
			break;
		case 1:
			setInitialSceneDescriptionString( "He moves quickly, ducking in and out of range. Eventually you overextend yourself and he capitalizes on this, biting down on your "
				"arm while clawing at your face." );
			sceneInflictWound( STAT_SELF, "were bitten" );
			sceneInflictWound( STAT_SELF, "flesh is clawed open" );
			break;
		case 2:
			setInitialSceneDescriptionString( "He leaps over you. You quickly turn and try to find him, but before you can he attacks. He leaps again, only this time landing on "
				"your chest. His claws find flesh before he kicks off and lands gracefully on the ground." );
			sceneInflictWound( STAT_SELF, "flesh is clawed open" );
			sceneInflictWound( STAT_SELF, "flesh is clawed open" );
			break;
		}

		// want a special death scene for this one
		if( isDead( ) ) {
			switchScene( closure_Make( maybeEndBad, 0 ) );
			return;
		}
	}

	startScene( );

	addMaybeFightChoices( );

	defaultSceneLoop( );
}

void addMaybeFightChoices( )
{
	int choice = balancedRandom( 3 );

	if( choice == 0 ) {
		pushSkillBasedChoice( "Kill him.", SKL_FAMILY, STAT_SELF, VERY_HARD_TEST,
			closure_Make( maybeFightSuccess, 0 ), closure_Make( maybeFightCostlySuccess, 0 ), closure_Make( maybeFightFailure, 0 ) );
	} else if( choice == 1 ) {
		pushSkillBasedChoice( "Stop him.", SKL_FAMILY, STAT_SELF, VERY_HARD_TEST,
			closure_Make( maybeFightSuccess, 0 ), closure_Make( maybeFightCostlySuccess, 0 ), closure_Make( maybeFightFailure, 0 ) );
	} else {
		pushSkillBasedChoice( "Hurt him.", SKL_FAMILY, STAT_SELF, VERY_HARD_TEST,
			closure_Make( maybeFightSuccess, 0 ), closure_Make( maybeFightCostlySuccess, 0 ), closure_Make( maybeFightFailure, 0 ) );
	}
}

void maybeEndIntro( CLOSURE( void )* closure, bool firstTimeRun )
{
	if( firstTimeRun ) {
		timesWon = 0;

		Companions helpingFae = getRandomAppeasedFae( );
		addReplacementString( "FAE_DESCRIPTION", rpgGame.arrCompanions[helpingFae].description );

		setInitialSceneDescriptionString( "You're awoken in the middle of the night by the sound of breaking glass. [SPOUSE_NAME] shoots up at the same time you do. You tell [SPOUSE_OBJECT_PRONOUN] "
			"to grab [CHILD_NAME] and get to the nearest neighbor. While making your way to the living room to grab your rifle you hear [BABY_NAME] start crying and screaming. Grabbing the the closest "
			"thing to you, you run as fast as possible.\n\n"
			"Smashing shoulder first through the door you see a short squat man in nice clothes backhanding [FAE_DESCRIPTION] to the ground and turn his red eyes to you. You ready the fire poker.\n\n"
			"What do you do?" );
	}

	startScene( );

	addMaybeFightChoices( );

	defaultSceneLoop( );
}

void switchToNight( CLOSURE( void )* closure, bool firstTimeRun )
{
	// see if any of the visitor scenes should be shown, if not switch to the next day
	if( gameCharacterData.currentDay == gameCharacterData.hungryManDay && isCompanionWithCharacter( &rpgGame, &character, CMP_HUNGRY ) ) {
		switchScene( closure_Make( hungryManIntro, 0 ) );
	} else if( gameCharacterData.currentDay == gameCharacterData.cannibalDoorDay ) {
		switchScene( closure_Make( cannibalKnockIntro, 0 ) );
	} else if( gameCharacterData.currentDay == gameCharacterData.cannibalKidnapDay ) {
		// end of the game, three options here
		int count = countAppeasedFae( );
		if( count >= 3 ) {
			// good end, friendlies drive off cannibal
			switchScene( closure_Make( goodEnd, 0 ) );
		} else if( count == 2 ) {
			// possibly good, friendlies delay cannibal
			switchScene( closure_Make( maybeEndIntro, 0 ) );
		} else {
			// bad end, cannibal eats baby
			switchScene( closure_Make( badEnd, 0 ) );
		}
	} else {
		switchScene( closure_Make( startDayScene, 0 ) );
	}
}

void switchToNextPartOfDay( CLOSURE( void )* closure, bool firstTimeRun )
{
	if( isFlagOn( character.flags, CF_IS_AFTERNOON ) ) {
		switchScene( closure_Make( switchToNight, 0 ) );
	} else {
		switchScene( closure_Make( switchToAfternoon, 0 ) );
	}
}

void childDeathScene( CLOSURE( void )* closure, bool firstTimeRun )
{
	// child goes missing, move back to city
	topTitle = " Absolute Silence Leads To Sadness ";
	startDraw( window ); {
		drawString( window, 2, 2, safeWriteArea,
			"When [CHILD_NAME] doesn't return that night you leave [SPOUSE_NAME] with [BABY_NAME] and go out searching. The days pass as you search with "
			"no sign of what happened. No scraps of cloth, no body. The uncertainty gnaws at you. The police are no help, and your neighbors saw nothing.\n\n"
			"Months have passed. The farm is falling into disrepair, the crops are wilting in the field, and you can't bring yourself to do anything. [SPOUSE_NAME] "
			"helps occasionally but it's not enough. This is farm is haunted by your memories of [CHILD_NAME]. You have to sell it.\n\n"
			"It takes a few more months to sell the farm and animals. You can't get away from here fast enough.\n\n"
			"By the end of the year you have your job as a clerk back. The bags under your eyes never go away."
			"\n\n\n"
			"The End",
			attr_Clrs( PDC_GREY, PDC_BLACK ) );

		centerStringHoriz( window, safeWriteArea, safeWriteArea.bottom, "Press any key to return to the title screen", attr_Clrs( PDC_YELLOW, PDC_BLACK ) );
	} endDraw( window );

	waitForAnyInput( window );

	switchScene( closure_Make( titleScene, 0 ) );
}

void spouseDeathScene( CLOSURE( void )* closure, bool firstTimeRun )
{
	// spouse goes missing, move back to city
	topTitle = " Absolute Silence Leads To Sadness ";
	startDraw( window ); {
		drawString( window, 2, 2, safeWriteArea,
			"When [SPOUSE_NAME] doesn't return that night you leave [CHILD_NAME] with [BABY_NAME] and go out searching. The days pass as you search with "
			"no sign of what happened. No scraps of cloth, no body. The uncertainty gnaws at you. The police are no help, and your neighbors saw nothing.\n\n"
			"Months have passed. The farm is falling into disrepair, the crops are wilting in the field, and you can't bring yourself to do anything. [CHILD_NAME] "
			"helps occasionally but it's not enough. This is farm is haunted by your memories of [SPOUSE_NAME]. You have to sell it.\n\n"
			"It takes a few more months to sell the farm and animals. You can't get away from here fast enough.\n\n"
			"By the end of the year you have your job as a clerk back. The bags under your eyes never go away."
			"\n\n\n"
			"The End",
			attr_Clrs( PDC_GREY, PDC_BLACK ) );

		centerStringHoriz( window, safeWriteArea, safeWriteArea.bottom, "Press any key to return to the title screen", attr_Clrs( PDC_YELLOW, PDC_BLACK ) );
	} endDraw( window );

	waitForAnyInput( window );

	switchScene( closure_Make( titleScene, 0 ) );
}

void youDeathScene( CLOSURE( void )* closure, bool firstTimeRun )
{
	// you die
	topTitle = " There Will Be Plenty Of Time To Sleep When You're Dead ";
	startDraw( window ); {
		drawString( window, 2, 2, safeWriteArea,
			"You're unsure of how long you've been here. Waiting in the blackness. There is no time and no space. With no frame of reference "
			"orientation is meaningless.\n\n"
			"Slowly and almost imperceptibly these come back. Below you, always below no matter which way you turn, grows a green glow. It's "
			"neither cold nor warm, but peaceful.\n\n"
			"You float down into it. You both welcome it and are welcomed into it. You both imbibe and are imbibed by it."
			"\n\n\n"
			"The End",
			attr_Clrs( PDC_GREY, PDC_BLACK ) );

		centerStringHoriz( window, safeWriteArea, safeWriteArea.bottom, "Press any key to return to the title screen", attr_Clrs( PDC_YELLOW, PDC_BLACK ) );
	} endDraw( window );

	waitForAnyInput( window );

	switchScene( closure_Make( titleScene, 0 ) );
}

void farmhouseDeathScene( CLOSURE( void )* closure, bool firstTimeRun )
{
	// termites collapse the house, can't support the farm any more, have to move back to the city
	topTitle = " Home Is Where The Heart Is ";
	startDraw( window ); {
		drawString( window, 2, 2, safeWriteArea,
			"You're unsure of what happened at first. There was a great crash as the house collapsed around you. Luckily everyone survived. But without "
			"a house how will you survive the winter? You have no money to rebuild so you have no choice but to sell the farm. Looking through "
			"the remains of your home you see that termites had been eating away at it. You should have paid more attention.\n\n"
			"It takes a few months to sell the farm. Each day is painful as you watch your dream fade.\n\n"
			"By the end of the year you have your job as a clerk back. The bags under your eyes never go away."
			"\n\n\n"
			"The End",
			attr_Clrs( PDC_GREY, PDC_BLACK ) );

		centerStringHoriz( window, safeWriteArea, safeWriteArea.bottom, "Press any key to return to the title screen", attr_Clrs( PDC_YELLOW, PDC_BLACK ) );
	} endDraw( window );

	waitForAnyInput( window );

	switchScene( closure_Make( titleScene, 0 ) );
}

void animalsDeathScene( CLOSURE( void )* closure, bool firstTimeRun )
{
	// animals die, can't support the farm any more, have to move back to the city
	topTitle = " We Can Judge The Heart Of A Man By His Treatment Of Animals ";
	startDraw( window ); {
		drawString( window, 2, 2, safeWriteArea,
			"You crawl out of bed in the morning, [SPOUSE_NAME] behind you. Looking out over your barn you see it empty. All the animals are dying or "
			"have died. Without them the farm won't be able to survive. They were the fuel and the muscles which drove it forward.\n\n"
			"It takes a few months to sell the farm. Each day is painful as you watch your dream fade.\n\n"
			"By the end of the year you have your job as a clerk back. The bags under your eyes never go away."
			"\n\n\n"
			"The End",
			attr_Clrs( PDC_GREY, PDC_BLACK ) );

		centerStringHoriz( window, safeWriteArea, safeWriteArea.bottom, "Press any key to return to the title screen", attr_Clrs( PDC_YELLOW, PDC_BLACK ) );
	} endDraw( window );

	waitForAnyInput( window );

	switchScene( closure_Make( titleScene, 0 ) );
}

void cropsDeathScene( CLOSURE( void )* closure, bool firstTimeRun )
{
	// crops die, can't support the farm any more, have to move back to the city
	topTitle = " Don't Judge Each Day By The Harvest You Reap ";
	startDraw( window ); {
		drawString( window, 2, 2, safeWriteArea,
			"You crawl out of bed in the morning, [SPOUSE_NAME] behind you. Looking out the window you see your fields barren. Nothing grows there buy weeds."
			"You sigh as [SPOUSE_NAME] hugs you from behind. Without the crops this farm will never support your family.\n\n"
			"It takes a few months to sell the farm and animals. Each day is painful as you watch your dream fade.\n\n"
			"By the end of the year you have your job as a clerk back. The bags under your eyes never go away."
			"\n\n\n"
			"The End",
			attr_Clrs( PDC_GREY, PDC_BLACK ) );

		centerStringHoriz( window, safeWriteArea, safeWriteArea.bottom, "Press any key to return to the title screen", attr_Clrs( PDC_YELLOW, PDC_BLACK ) );
	} endDraw( window );

	waitForAnyInput( window );

	switchScene( closure_Make( titleScene, 0 ) );
}

void familyDeathScene( CLOSURE( void )* closure, bool firstTimeRun )
{
	// family leaves you, you fester on the farm
	topTitle = " One Is The Loneliest Number ";
	startDraw( window ); {
		drawString( window, 2, 2, safeWriteArea,
			"You wake up a bit before the dawn ready to start the day. You roll over and find the bed cold and empty. Calling out for [SPOUSE_NAME] you make "
			"your way to the kitchen. The stove hasn't been started and there's no signs of movement. On the table you find a note. This isn't working out "
			"[SPOUSE_SUBJECT_PRONOUN] they wrote, [SPOUSE_SUBJECT_PRONOUN] can't live like this. They've all gone back to the city.\n\n"
			"You find some solace in work, some in the bottom of a bottle. But you try to keep things working by yourself. But it's too hard.\n\n"
			"No human finds your body or even knows that you're gone. Those who aren't human mourn your passing."
			"\n\n\n"
			"The End",
			attr_Clrs( PDC_GREY, PDC_BLACK ) );

		centerStringHoriz( window, safeWriteArea, safeWriteArea.bottom, "Press any key to return to the title screen", attr_Clrs( PDC_YELLOW, PDC_BLACK ) );
	} endDraw( window );

	waitForAnyInput( window );

	switchScene( closure_Make( titleScene, 0 ) );
}

void mysterDeathScene( CLOSURE( void )* closure, bool firstTimeRun )
{
	topTitle = " Another Missing Family ";
	startDraw( window ); {
		drawString( window, 2, 2, safeWriteArea,
			"There had been a family that lived on old farmland. They had escaped from the pollution of the city. That farmland had been part of kingdom "
			"older than any of their ancestors. The family isn't there anymore. They aren't anywhere anymore."
			"\n\n\n"
			"The End",
			attr_Clrs( PDC_GREY, PDC_BLACK ) );

		centerStringHoriz( window, safeWriteArea, safeWriteArea.bottom, "Press any key to return to the title screen", attr_Clrs( PDC_YELLOW, PDC_BLACK ) );
	} endDraw( window );

	waitForAnyInput( window );

	switchScene( closure_Make( titleScene, 0 ) );
}

void deathScene( CLOSURE( void )* closure, bool firstTimeRun )
{
	// different types of deaths, farm upkeep and wounds
	//  wounds death scene, other person goes missing
	//  wounds death scene, you die
	//  farmhouse collapse scene
	//  animals die scene
	//  crops wither scene
	//  family leaves you scene
	if( isDeadFromStat( STAT_SELF ) ) switchScene( closure_Make( youDeathScene, 0 ) );
	else if( isDeadFromStat( STAT_CHILD ) ) switchScene( closure_Make( childDeathScene, 0 ) );
	else if( isDeadFromStat( STAT_SPOUSE ) ) switchScene( closure_Make( spouseDeathScene, 0 ) );
	else if( isDeadFromFarmStatus( FS_FAMILY ) ) switchScene( closure_Make( familyDeathScene, 0 ) );
	else if( isDeadFromFarmStatus( FS_CROPS ) ) switchScene( closure_Make( cropsDeathScene, 0 ) );
	else if( isDeadFromFarmStatus( FS_ANIMALS ) ) switchScene( closure_Make( animalsDeathScene, 0 ) );
	else if( isDeadFromFarmStatus( FS_FARMHOUSE ) ) switchScene( closure_Make( farmhouseDeathScene, 0 ) );
	else switchScene( closure_Make( mysterDeathScene, 0 ) );
}