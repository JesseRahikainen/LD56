#define CLOSURES_IMPLEMENTATION
#define CLOSURE_ALLOC(s)	mem_Allocate(s)
#define CLOSURE_FREE(p)		mem_Release(p)

#define STB_DS_IMPLEMENTATION
#define STBDS_REALLOC(c,p,s) mem_Resize(p,s)
#define STBDS_FREE(c,p)      mem_Release(p)
#include "utils.h"

#include <stdarg.h>
#include <stdlib.h>

uint32_t turnOnFlag( uint32_t flags, uint32_t id )
{
	if( id >= 32 ) return flags;
	return flags | ( 1 << id );
}

uint32_t createFlagSet( uint32_t numFlags, ... )
{
	uint32_t flags = 0;
	va_list args;
	va_start( args, numFlags ); {
		for( uint32_t i = 0; i < numFlags; ++i ) {
			uint32_t id = va_arg( args, uint32_t );
			flags = turnOnFlag( flags, id );
		}
	} va_end( args );
	return flags;
}

uint32_t turnOffFlag( uint32_t flags, uint32_t id )
{
	if( id >= 32 ) return flags;
	return flags & ~( 1 << id );
}

bool isFlagOn( uint32_t flags, uint32_t id )
{
	if( id >= 32 ) return false;
	return ( ( flags & ( 1 << id ) ) > 0 );
}

int balancedRandom( int range )
{
	int fullRanges = RAND_MAX / range;
	int maxRoll = range * fullRanges;
	int roll;
	do {
		roll = rand( );
	} while( roll > maxRoll );
	return roll % range;
}

void infiniteListSelector_Init( InfiniteListSelector* selector, int invalidId )
{
	assert( selector != NULL );
	selector->chosenId = invalidId;
	selector->weightTotal = 0;
}

void infiniteListSelector_Choose( InfiniteListSelector* selector, int itemId, uint32_t itemWeight )
{
	assert( selector != NULL );
	if( itemWeight == 0 ) return;

	uint32_t totalWeight = selector->weightTotal + itemWeight;
	if( balancedRandom( totalWeight + 1 ) > selector->weightTotal ) {
		selector->chosenId = itemId;
	}
	selector->weightTotal = totalWeight;
}

char* appendToSBString( char* sbString, char* appendText )
{
	if( !appendText ) return sbString;
	arrpop( sbString );

	size_t appendLen = strlen( appendText ) + 1; // include terminating null
	char* start = arraddnptr( sbString, appendLen );
	memcpy( start, appendText, sizeof( char ) * appendLen );
	return sbString;
}