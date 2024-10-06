#ifndef WINDOW_H
#define WINDOW_H

// basic window handling, drawing and input

#include <Windows.h>
#include <stdbool.h>
#include <stdint.h>

extern char* topTitle;

typedef enum {
	IN_NONE,
	IN_UP,
	IN_DOWN,
	IN_LEFT,
	IN_RIGHT,
	IN_ENTER,
	IN_SPACE,
	IN_PLUS,
	IN_MINUS,
	IN_OPT_1,
	IN_OPT_2,
	IN_OPT_3,
	IN_OPT_4,
	IN_OPT_5,
	IN_OPT_6,
	IN_OPT_7,
	IN_OPT_8,
	IN_OPT_9,
	IN_C,
	IN_H,
	IN_OTHER
} Input;

#define SCREEN_WIDTH 80
#define SCREEN_HEIGHT 50

typedef struct Window Window;

// each character spot has a set of attributes associated with it
//  foreground color - RGB
//  background color - RGB
// these aren't currently supported but could be:
//  blinking - frequency (seconds), frequency offset (seconds)
// the ones below aren't supported on this platform:
//  bold - bool
//  italic - bool
//  scale - x (scalar), y (scalar)
//  wave - frequency (seconds), frequency offset (seconds), magnitude (pixels)
//  wiggle - frequency (seconds), frequency offset (seconds), magnitude (pixels)

typedef struct {
	WORD attributes;
} Attributes;

typedef enum {
	PDC_BLACK,
	PDC_DARK_GREY,
	PDC_DARK_BLUE,
	PDC_BLUE,
	PDC_DARK_GREEN,
	PDC_GREEN,
	PDC_DARK_CYAN,
	PDC_CYAN,
	PDC_MAROON,
	PDC_RED,
	PDC_PURPLE,
	PDC_MAGENTA,
	PDC_BROWN,
	PDC_YELLOW,
	PDC_GREY,
	PDC_WHITE
} PredefinedColors;

Attributes attr_Clrs( PredefinedColors foreground, PredefinedColors background );
Attributes attr_FGClr( PredefinedColors foreground );

// ═  205
// ║  186
// ╗  187
// ╚  200
// ╝  188
// ╔  201
// ╦  203
// ╩  202
// ╠  204
// ╣  185
// ╬  206
// /n 10
// _  32

typedef struct {
	int16_t left;
	int16_t top;
	int16_t right;
	int16_t bottom;
} SmallRect;

typedef struct {
	int16_t x;
	int16_t y;
} Coord;

extern SmallRect windowSize;
extern SmallRect renderArea;
extern SmallRect safeWriteArea;
extern Coord topLeft;
extern Coord bufferSize;

Window* createWindow( const char* title );
void destroyWindow( Window* screen );
bool contains( Coord c, SmallRect area );
void simplePutChar( Window* screen, char c, Attributes attr, int16_t x, int16_t y );
char simpleGetChar( Window* screen, int16_t x, int16_t y );
void putChar( Window* screen, char c, Attributes attr, Coord pos, SmallRect border );
Coord vdrawStringIgnoreSize( Window* screen, int16_t x, int16_t y, SmallRect border, const char* str, Attributes attributes, va_list args );
Coord drawStringIgnoreSize( Window* screen, int16_t x, int16_t y, SmallRect border, const char* str, Attributes attributes, ... );
Coord drawString( Window* screen, int16_t x, int16_t y, SmallRect border, const char* str, Attributes attributes, ... );
void centerStringHoriz( Window* screen, SmallRect area, int16_t y, const char* str, Attributes attributes, ... ); // strings with rich text tags won't center correctly
void drawBorder( Window* screen );
void startDraw( Window* screen );
void endDraw( Window* screen );
Input getNextInput( Window* screen );
void waitForAnyInput( Window* screen );
void eatAllInputs( Window* screen );
void drawDebugRect( Window* screen, SmallRect rect, Attributes attributes );

void setupRichTextParsing( void );

#endif // inclusion guard
