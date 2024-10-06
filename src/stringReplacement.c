#include "stringReplacement.h"

#include <string.h>
#include <ctype.h>

#include "utils.h"

#include "memory.h"

struct { const char* key; const char* value; } *replacementStringHash = NULL;
void addReplacementString( const char* replace, const char* with )
{
	if( replacementStringHash == NULL ) {
		sh_new_strdup( replacementStringHash );
	}
	shput( replacementStringHash, replace, with );
}

bool hasReplacementString( const char* replace )
{
	const char* replacement = shget( replacementStringHash, replace );
	return replacement != NULL;
}

bool removeReplacementString( const char* replace )
{
	return shdel( replacementStringHash, replace ) == 1;
}

struct { char* key; const char* ( *value )( void* data ); } *replacementFuncHash = NULL;
void addReplacementFunc( const char* replace, const  char* ( *func )( void* data ) )
{
	if( replacementFuncHash == NULL ) {
		sh_new_strdup( replacementFuncHash );
	}
	shput( replacementFuncHash, replace, func );
}

bool hasReplacementFunc( const char* replace )
{
	char* ( *func )( void* data ) = shget( replacementFuncHash, replace );
	return func != NULL;
}

bool removeReplacementFunc( const char* replace )
{
	return shdel( replacementFuncHash, replace ) == 1;
}

char* replacementPass( const char* baseStr, void* data, bool* outAnyReplaced )
{
	if( outAnyReplaced != NULL ) *outAnyReplaced = false;
	size_t strLen = strlen( baseStr );
	char* str = mem_Allocate( strLen + 1 );
	strcpy_s( str, strLen + 1, baseStr );

	bool inReplacementTag = false;
	char* outString = NULL;

	size_t chunkStart = 0;
	size_t chunkEnd = 0;

	for( size_t i = 0; i < strLen; ++i ) {
		if( inReplacementTag ) {
			// inside something that might be a replacement tag
			if( str[i] == ']' ) {
				// reached the end of the tag, check to see if the replacement exists, and if it does then replace it
				//  if not then just output it

				str[i] = 0;
				char* testStr = &( str[chunkStart] );
				const char* replacement = shget( replacementStringHash, testStr );
				char* ( *replacementFunc )( void* data ) = shget( replacementFuncHash, testStr );
				if( replacement != NULL ) {
					size_t len = strlen( replacement );
					char* start = arraddnptr( outString, len );
					memcpy( start, replacement, len );

					if( outAnyReplaced != NULL ) *outAnyReplaced = true;
				} else if( replacementFunc != NULL ) {
					char* funcResult = replacementFunc( data );
					size_t len = strlen( funcResult );
					char* start = arraddnptr( outString, len );
					memcpy( start, funcResult, len );

					if( outAnyReplaced != NULL ) *outAnyReplaced = true;
				} else {
					// dump out the original data
					arrpush( outString, '[' );

					size_t len = i - chunkStart;
					char* start = arraddnptr( outString, len );
					memcpy( start, testStr, i - chunkStart );

					arrpush( outString, ']' );
				}

				inReplacementTag = false;
				chunkStart = i + 1;
			}
		} else {
			if( str[i] == '[' ) {
				// reached the start of a replacement tag
				//  dump out the last section of the string
				char* start = arraddnptr( outString, i - chunkStart );
				memcpy( start, &( str[chunkStart] ), i - chunkStart );

				//  change state
				inReplacementTag = true;
				str[i] = 0;
				chunkStart = i + 1;
			}
		}
	}

	// if there's a chunk left to output then do it, make sure you don't go past outBufferSize
	if( chunkStart < strLen ) {
		if( inReplacementTag ) {
			arrpush( outString, '[' );
		}
		size_t len = strLen - chunkStart;
		char* start = arraddnptr( outString, len );
		memcpy( start, &( str[chunkStart] ), len );
	}

	arrpush( outString, 0 );
	mem_Release( str );

	return outString;
}

char* runReplacements( const char* baseStr, CapitalizationType type, void* data )
{
	char* result = NULL;
	const char* current = baseStr;
	bool anyReplaced = false;
	bool firstPass = true;

	// do replacement passes until there's nothing left to replace, allows us to do embedded replacements
	do {
		result = replacementPass( current, data, &anyReplaced );
		if( firstPass ) {
			firstPass = false;
		} else {
			arrfree( current );
		}
		current = result;
	} while( anyReplaced );

	runCapitalization( result, type );

	return result;
}

// make sure all the words in the string are properly capitalized, shouldn't involve changing the size of the string
void runCapitalization( char* string, CapitalizationType type )
{
	if( type == CAPTYPE_IGNORE ) return;

	bool capitalize = true;
	for( size_t i = 0; i < strlen( string ); ++i ) {
		if( capitalize ) {
			if( !isspace( string[i] ) ) {
				string[i] = toupper( string[i] );
				capitalize = false;
			}
		} else {
			capitalize = ( string[i] == '.' || string[i] == '!' || string[i] == '?' ) || ( ( type == CAPTYPE_TITLE && isspace( string[i] ) ) );
		}
	}
}