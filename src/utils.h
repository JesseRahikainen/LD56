#ifndef UTILS_H
#define UTILS_H

#include <stdint.h>
#include <stdbool.h>

#include "memory.h"

#if !defined(CLOSURE_FREE) && !defined(CLOSURE_FREE)
#define CLOSURE_ALLOC(s)	mem_Allocate(s)
#define CLOSURE_FREE(p)		mem_Release(p)
#endif
#include "closures.h"

#if !defined(STBDS_REALLOC) && !defined(STBDS_FREE)
#define STBDS_REALLOC(c,p,s) mem_Resize(p,s)
#define STBDS_FREE(c,p)      mem_Release(p)
#endif
#include "stb_ds.h"

#define SHUFFLE( a, type, iter ) { \
		for( int i##iter = ARRAYSIZE( a ) - 1; i##iter > 0; --i##iter ) { \
			int swap = balancedRandom( i##iter + 1 ); \
			type temp = a[i##iter]; a[i##iter] = a[swap]; a[swap] = temp; \
		} \
	}

#define SHUFFLE_SB( a, type ) { \
		for( size_t i = ( arrlenu( a ) - 1 ); i > 0; --i ) { \
			\
				size_t swap = balancedRandom( i + 1 ); \
				type temp = a[i]; a[i] = a[swap]; a[swap] = temp; \
		} \
	}

uint32_t turnOnFlag( uint32_t flags, uint32_t id );
uint32_t createFlagSet( uint32_t numFlags, ... );
uint32_t turnOffFlag( uint32_t flags, uint32_t id );
bool isFlagOn( uint32_t flags, uint32_t id );

int balancedRandom( int range );

typedef struct {
	int weightTotal;
	int chosenId;
} InfiniteListSelector;

void infiniteListSelector_Init( InfiniteListSelector* selector, int invalidId );
void infiniteListSelector_Choose( InfiniteListSelector* selector, int itemId, uint32_t itemWeight );

char* appendToSBString( char* sbString, char* appendText );

#endif // inclusion guard
