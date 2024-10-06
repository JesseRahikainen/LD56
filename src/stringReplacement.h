#ifndef STRING_REPLACEMENT_H
#define STRING_REPLACEMENT_H

#include <stdbool.h>

typedef enum {
	CAPTYPE_IGNORE,
	CAPTYPE_STANDARD,
	CAPTYPE_TITLE
} CapitalizationType;

char* runReplacements( const char* baseStr, CapitalizationType type, void* data ); // string is a dynamic array, call arrfree() on it when you're done with it
void addReplacementString( const char* replace, const char* with ); // any sub-string surrounded by {} is checked for replacement, if an existing replacement using replace exists this will change it
bool hasReplacementString( const char* replace );
bool removeReplacementString( const char* replace );
void addReplacementFunc( const char* replace, const char* ( *func )( void* data ) );
bool hasReplacementFunc( const char* replace );
bool removeReplacementFunc( const char* replace );
void runCapitalization( char* string, CapitalizationType type );

#endif // inclusion guard