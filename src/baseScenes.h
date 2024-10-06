#ifndef BASE_SCENES_H
#define BASE_SCENES_H

#include <stdbool.h>

#include "closures.h"

void startDayScene( CLOSURE( void )* closure, bool firstTimeRun );
void morningScene( CLOSURE( void )* closure, bool firstTimeRun );
void afternoonScene( CLOSURE( void )* closure, bool firstTimeRun );
void deathScene( CLOSURE( void )* closure, bool firstTimeRun );

#endif // inclusion guard