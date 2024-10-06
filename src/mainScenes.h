#ifndef MAIN_SCENES_H
#define MAIN_SCENES_H

#include <stdbool.h>
#include "closures.h"

void titleScene( CLOSURE( void )* closure, bool firstTimeRun );
void introScene( CLOSURE( void )* closure, bool firstTimeRun );
void helpScene( CLOSURE( void )* closure, bool firstTimeRun );

#endif // inclusion guard