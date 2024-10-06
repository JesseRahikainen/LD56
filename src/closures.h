#ifndef CLOSURES_H
#define CLOSURES_H

/*
Example usage:

CLOSURE_TEMPLATE( int );

int testSumInts( CLOSURE( int )* closure )
{
	// assume we're getting the number of ints first, then the list of ints
	va_list va;
	va_start( va, closure->args ); // points to the address of the first argument
	int size = va_arg( va, int );
	va_end( va );

	int sum = 0;
	for( int i = 0; i < size; ++i ) {
		sum += va_arg( va, int );
	}
	return sum;
}

void test( void )
{
	int values[] = { 0, 1, 2, 3, 4 };
	int numValues = sizeof( values ) / sizeof( values[0] );

	CLOSURE( int )* sum = closure_Make( testSumInts, sizeof( int ) + sizeof( values ) );
	closure_PushArg( sum, &numValues, sizeof( numValues ) );
	for( int i = 0; i < numValues; ++i ) closure_PushArg( sum, &values[i], sizeof( values[i] ) );

	int total = sum->func( sum );

	free( sum );
}
*/


// taken from: https://stackoverflow.com/questions/4393716/is-there-a-a-way-to-achieve-closures-in-c

#include <stdint.h>

// use for the type of return value you want for the closure function, so CLOSURE(void) for nothing, CLOSURE(int) for int, etc.
#define CLOSURE(t) ClosureStruct_##t
#define CLOSURE_TEMPLATE(t)			\
typedef struct {					\
	t (*func)();					\
	size_t maxArgsSize;				\
	size_t argsInUse;				\
	uint8_t* args;					\
} CLOSURE(t)

// just a base type to use
CLOSURE_TEMPLATE(void);

// you're responsible for cleaning this up using CLOSURE_FREE()
void* closure_Make( void ( *func )( ), size_t maxArgsSize );
void* closure_PushArg( void* closure, void* arg, size_t argSize );
void* closure_Copy( void* closure );

#if defined(CLOSURE_ALLOC) && !defined(CLOSURE_FREE) || !defined(CLOSURE_ALLOC) && defined(CLOSURE_FREE)
#error "You must define both CLOSURE_ALLOC and CLOSURE_FREE, or neither."
#endif
#if !defined(CLOSURE_ALLOC) && !defined(CLOSURE_FREE)
#include <stdlib.h>
#define CLOSURE_ALLOC(s)	malloc(s)
#define CLOSURE_FREE(s)		free(s)
#endif

#ifdef CLOSURES_IMPLEMENTATION

#include <string.h>

void* closure_Make( void ( *func )( ), size_t maxArgsSize )
{
	CLOSURE( void )* closure = CLOSURE_ALLOC( sizeof( CLOSURE( void ) ) + maxArgsSize );
	if( closure ) {
		closure->func = func;
		closure->maxArgsSize = maxArgsSize;
		closure->argsInUse = 0;
		closure->args = (uint8_t*)( closure + 1 ); // data is stored after closure pointer
	}
	return closure;
}

void* closure_PushArg( void* closure, void* arg, size_t argSize )
{
	if( closure ) {
		CLOSURE( void )* c = ( CLOSURE( void )* )closure;
		if( c->argsInUse + argSize <= c->maxArgsSize ) {
			memcpy( c->args + c->argsInUse, arg, argSize );
			c->argsInUse += argSize;
		}
	}
	return closure;
}

void* closure_Copy( void* closure )
{
	if( !closure ) return NULL;

	CLOSURE( void )* c = ( CLOSURE( void )* )closure;
	CLOSURE( void )* newClosure = CLOSURE_ALLOC( sizeof( CLOSURE( void ) ) + c->maxArgsSize );
	if( newClosure ) {
		newClosure->func = c->func;
		newClosure->maxArgsSize = c->maxArgsSize;
		newClosure->argsInUse = c->argsInUse;
		newClosure->args = (uint8_t*)( newClosure + 1 ); // data is stored after closure pointer
		memcpy( newClosure->args, c->args, newClosure->maxArgsSize );
	}
	return newClosure;
}

#endif // implementation

#endif // inclusion guard
