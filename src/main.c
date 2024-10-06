

#include <time.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <assert.h>

#include "memory.h"

#include "window.h"
#include "stringReplacement.h"

#include "game.h"
#include "rpg.h"
#include "utils.h"
#include "mainScenes.h"
#include "baseScenes.h"

#include <Windows.h>

typedef enum {
	GDR_MALE,
	GDR_FEMALE,
	GDR_INDETERMINATE
} Gender;

const char* subjectPronoun( Gender g )
{
	switch( g ) {
	case GDR_MALE:
		return "he";
	case GDR_FEMALE:
		return "she";
	case GDR_INDETERMINATE:
		return "they";
	default:
		return "it";
	}
}

const char* objectPronoun( Gender g )
{
	switch( g ) {
	case GDR_MALE:
		return "him";
	case GDR_FEMALE:
		return "her";
	case GDR_INDETERMINATE:
		return "they";
	default:
		return "it";
	}
}

const char* possessivePronoun( Gender g )
{
	switch( g ) {
	case GDR_MALE:
		return "his";
	case GDR_FEMALE:
		return "hers";
	case GDR_INDETERMINATE:
		return "their";
	default:
		return "its";
	}
}

int main( int argc, char** argv )
{
	mem_Init( 1024 * 1025 * 16 );

	window = createWindow( "Ridianfain Fair Folk" );

	srand( (unsigned int)time( NULL ) );
	topTitle = NULL;

	// setup all the game data
	setupGame( );

	addReplacementString( "YOUR_NAME", "Briugu" );
	addReplacementString( "SPOUSE_NAME", "Ceile" );
	addReplacementString( "CHILD_NAME", "Lenab" );
	addReplacementString( "BABY_NAME", "Tru" );

	// random pronouns for family
	Gender spouseGender = rand( ) % GDR_INDETERMINATE;
	addReplacementString( "SPOUSE_SUBJECT_PRONOUN", subjectPronoun( spouseGender ) );
	addReplacementString( "SPOUSE_OBJECT_PRONOUN", objectPronoun( spouseGender ) );
	addReplacementString( "SPOUSE_POSSESSIVE_PRONOUN", possessivePronoun( spouseGender ) );

	Gender childGender = rand( ) % GDR_INDETERMINATE;
	addReplacementString( "CHILD_SUBJECT_PRONOUN", subjectPronoun( childGender ) );
	addReplacementString( "CHILD_OBJECT_PRONOUN", objectPronoun( childGender ) );
	addReplacementString( "CHILD_POSSESSIVE_PRONOUN", possessivePronoun( childGender ) );

	Gender babyGender = rand( ) % GDR_INDETERMINATE;
	addReplacementString( "BABY_SUBJECT_PRONOUN", subjectPronoun( babyGender ) );
	addReplacementString( "BABY_OBJECT_PRONOUN", objectPronoun( babyGender ) );
	addReplacementString( "BABY_POSSESSIVE_PRONOUN", possessivePronoun( babyGender ) );

	setupRichTextParsing( );

	switchScene( closure_Make( titleScene, 0 ) );
	//switchScene( closure_Make( introScene, 0 ) );
	//switchScene( closure_Make( baseScene, 0 ) );

	while( 1 ) {
		runCurrentScene( );
	}

	destroyWindow( window );

	return 0;
}