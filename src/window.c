#include "window.h"

#include <string.h>
#include <stdio.h>
#include <assert.h>

#include "utils.h"

#include "stringReplacement.h"
#include "memory.h"

#define SCREEN_POS( x, y ) ( ( x ) + ( SCREEN_WIDTH * ( y ) ) )

char* topTitle = NULL;

typedef struct Window {
	HANDLE write;
	HANDLE read;
	CHAR_INFO buffer[SCREEN_WIDTH * SCREEN_HEIGHT];
} Window;

const WORD BG_BLACK = 0;
const WORD BG_DARK_GREY = BACKGROUND_INTENSITY;
const WORD BG_DARK_BLUE = BACKGROUND_BLUE;
const WORD BG_BLUE = BACKGROUND_BLUE | BACKGROUND_INTENSITY;
const WORD BG_DARK_GREEN = BACKGROUND_GREEN;
const WORD BG_GREEN = BACKGROUND_GREEN | BACKGROUND_INTENSITY;
const WORD BG_DARK_CYAN = BACKGROUND_GREEN | BACKGROUND_BLUE;
const WORD BG_CYAN = BACKGROUND_GREEN | BACKGROUND_BLUE | BACKGROUND_INTENSITY;
const WORD BG_MAROON = BACKGROUND_RED;
const WORD BG_RED = BACKGROUND_RED | BACKGROUND_INTENSITY;
const WORD BG_PURPLE = BACKGROUND_RED | BACKGROUND_BLUE;
const WORD BG_MAGENTA = BACKGROUND_RED | BACKGROUND_BLUE | BACKGROUND_INTENSITY;
const WORD BG_BROWN = BACKGROUND_RED | BACKGROUND_GREEN;
const WORD BG_YELLOW = BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_INTENSITY;
const WORD BG_GREY = BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE;
const WORD BG_WHITE = BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE | BACKGROUND_INTENSITY;

const WORD FG_BLACK = 0;
const WORD FG_DARK_GREY = FOREGROUND_INTENSITY;
const WORD FG_DARK_BLUE = FOREGROUND_BLUE;
const WORD FG_BLUE = FOREGROUND_BLUE | FOREGROUND_INTENSITY;
const WORD FG_DARK_GREEN = FOREGROUND_GREEN;
const WORD FG_GREEN = FOREGROUND_GREEN | FOREGROUND_INTENSITY;
const WORD FG_DARK_CYAN = FOREGROUND_GREEN | FOREGROUND_BLUE;
const WORD FG_CYAN = FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY;
const WORD FG_MAROON = FOREGROUND_RED;
const WORD FG_RED = FOREGROUND_RED | FOREGROUND_INTENSITY;
const WORD FG_PURPLE = FOREGROUND_RED | FOREGROUND_BLUE;
const WORD FG_MAGENTA = FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_INTENSITY;
const WORD FG_BROWN = FOREGROUND_RED | FOREGROUND_GREEN;
const WORD FG_YELLOW = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY;
const WORD FG_GREY = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
const WORD FG_WHITE = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY;

const WORD BG_FILTER = BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE | BACKGROUND_INTENSITY;
const WORD FG_FILTER = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY;


char tempStrBuffer[4096];
char otherTempStrBuffer[4096];

SMALL_RECT fromAPI( SmallRect sr )
{
	SMALL_RECT newSR = { sr.left, sr.top, sr.right, sr.bottom };
	return newSR;
}

SmallRect fromWindows( SMALL_RECT sr )
{
	SmallRect newSR = { sr.Left, sr.Top, sr.Right, sr.Bottom };
	return newSR;
}

SmallRect windowSize = { 0, 0, SCREEN_WIDTH - 1, SCREEN_HEIGHT - 1 };
SmallRect renderArea = { 1, 1, SCREEN_WIDTH - 2, SCREEN_HEIGHT - 2 };
SmallRect safeWriteArea = { 2, 2, SCREEN_WIDTH - 3, SCREEN_HEIGHT - 3 };
Coord topLeft = { 0, 0 };
Coord bufferSize = { SCREEN_WIDTH, SCREEN_HEIGHT };

bool contains( Coord c, SmallRect area )
{
	return ( ( c.x <= area.right ) && ( c.x >= area.left ) && ( c.y <= area.bottom ) && ( c.y >= area.top ) );
}

void simplePutChar( Window* screen, char c, Attributes attr, int16_t x, int16_t y )
{
	assert( x >= 0 && x < SCREEN_WIDTH );
	assert( y >= 0 && y < SCREEN_HEIGHT );
	int idx = SCREEN_POS( x, y );
	screen->buffer[idx].Attributes = attr.attributes;
	screen->buffer[idx].Char.AsciiChar = c;
}

char simpleGetChar( Window* screen, int16_t x, int16_t y )
{
	int idx = SCREEN_POS( x, y );
	return screen->buffer[idx].Char.AsciiChar;
}

void putChar( Window* screen, char c, Attributes attr, Coord pos, SmallRect border )
{
	if( contains( pos, border ) ) {
		int idx = SCREEN_POS( pos.x, pos.y );
		screen->buffer[idx].Attributes = attr.attributes;
		screen->buffer[idx].Char.AsciiChar = c;
	}
}

typedef struct {
	WORD* arrFGStack;
	WORD* arrBGStack;
} StringRenderState;

StringRenderState createStringRenderState( Attributes attributes )
{
	StringRenderState state;

	state.arrFGStack = NULL;
	state.arrBGStack = NULL;

	WORD fg = attributes.attributes & FG_FILTER;
	WORD bg = attributes.attributes & BG_FILTER;

	arrpush( state.arrFGStack, fg );
	arrpush( state.arrBGStack, bg );

	return state;
}

void cleanStringRenderState( StringRenderState* state )
{
	arrfree( state->arrFGStack );
	arrfree( state->arrBGStack );
}


void pushFGColor( StringRenderState* state, WORD fg )
{
	arrpush( state->arrFGStack, fg );
}

void popFGColor( StringRenderState* state )
{
	if( arrlen( state->arrFGStack ) > 1 ) {
		arrpop( state->arrFGStack );
	}
}

void pushBGColor( StringRenderState* state, WORD bg )
{
	arrpush( state->arrBGStack, bg );
}

void popBGColor( StringRenderState* state )
{
	if( arrlen( state->arrBGStack ) > 1 ) {
		arrpop( state->arrBGStack );
	}
}

void pushBGBlack( StringRenderState* state )
{
	pushBGColor( state, BG_BLACK );
}

void pushBGDarkGrey( StringRenderState* state )
{
	pushBGColor( state, BG_DARK_GREY );
}

void pushBGDarkBlue( StringRenderState* state )
{
	pushBGColor( state, BG_DARK_BLUE );
}

void pushBGBlue( StringRenderState* state )
{
	pushBGColor( state, BG_BLUE );
}

void pushBGDarkGreen( StringRenderState* state )
{
	pushBGColor( state, BG_DARK_GREEN );
}

void pushBGGreen( StringRenderState* state )
{
	pushBGColor( state, BG_GREEN );
}

void pushBGDarkCyan( StringRenderState* state )
{
	pushBGColor( state, BG_DARK_CYAN );
}

void pushBGCyan( StringRenderState* state )
{
	pushBGColor( state, BG_CYAN );
}

void pushBGMaroon( StringRenderState* state )
{
	pushBGColor( state, BG_MAROON );
}

void pushBGRed( StringRenderState* state )
{
	pushBGColor( state, BG_RED );
}

void pushBGPurple( StringRenderState* state )
{
	pushBGColor( state, BG_PURPLE );
}

void pushBGMagenta( StringRenderState* state )
{
	pushBGColor( state, BG_MAGENTA );
}

void pushBGBrown( StringRenderState* state )
{
	pushBGColor( state, BG_BROWN );
}

void pushBGYellow( StringRenderState* state )
{
	pushBGColor( state, BG_YELLOW );
}

void pushBGGrey( StringRenderState* state )
{
	pushBGColor( state, BG_GREY );
}

void pushBGWhite( StringRenderState* state )
{
	pushBGColor( state, BG_WHITE );
}

void pushFGBlack( StringRenderState* state )
{
	pushFGColor( state, FG_BLACK );
}

void pushFGDarkGrey( StringRenderState* state )
{
	pushFGColor( state, FG_DARK_GREY );
}

void pushFGDarkBlue( StringRenderState* state )
{
	pushFGColor( state, FG_DARK_BLUE );
}

void pushFGBlue( StringRenderState* state )
{
	pushFGColor( state, FG_BLUE );
}

void pushFGDarkGreen( StringRenderState* state )
{
	pushFGColor( state, FG_DARK_GREEN );
}

void pushFGGreen( StringRenderState* state )
{
	pushFGColor( state, FG_GREEN );
}

void pushFGDarkCyan( StringRenderState* state )
{
	pushFGColor( state, FG_DARK_CYAN );
}

void pushFGCyan( StringRenderState* state )
{
	pushFGColor( state, FG_CYAN );
}

void pushFGMaroon( StringRenderState* state )
{
	pushFGColor( state, FG_MAROON );
}

void pushFGRed( StringRenderState* state )
{
	pushFGColor( state, FG_RED );
}

void pushFGPurple( StringRenderState* state )
{
	pushFGColor( state, FG_PURPLE );
}

void pushFGMagenta( StringRenderState* state )
{
	pushFGColor( state, FG_MAGENTA );
}

void pushFGBrown( StringRenderState* state )
{
	pushFGColor( state, FG_BROWN );
}

void pushFGYellow( StringRenderState* state )
{
	pushFGColor( state, FG_YELLOW );
}

void pushFGGrey( StringRenderState* state )
{
	pushFGColor( state, FG_GREY );
}

void pushFGWhite( StringRenderState* state )
{
	pushFGColor( state, FG_WHITE );
}

typedef void ( *RichTextCommand )( StringRenderState* state );
struct { char* key; RichTextCommand value; }* richTextCommands = NULL;

void setupRichTextParsing( void )
{
	sh_new_strdup( richTextCommands );

	// command tags should never have spaces in them, if they do they won't work correctly
	shput( richTextCommands, "{bg=Black}", pushBGBlack );
	shput( richTextCommands, "{bg=DarkGrey}", pushBGDarkGrey );
	shput( richTextCommands, "{bg=DarkBlue}", pushBGDarkBlue );
	shput( richTextCommands, "{bg=Blue}", pushBGBlue );
	shput( richTextCommands, "{bg=DarkGreen}", pushBGDarkGreen );
	shput( richTextCommands, "{bg=Green}", pushBGGreen );
	shput( richTextCommands, "{bg=DarkCyan}", pushBGDarkCyan );
	shput( richTextCommands, "{bg=Cyan}", pushBGCyan );
	shput( richTextCommands, "{bg=Maroon}", pushBGMaroon );
	shput( richTextCommands, "{bg=Red}", pushBGRed );
	shput( richTextCommands, "{bg=Purple}", pushBGPurple );
	shput( richTextCommands, "{bg=Magenta}", pushBGMagenta );
	shput( richTextCommands, "{bg=Brown}", pushBGBrown );
	shput( richTextCommands, "{bg=Yellow}", pushBGYellow );
	shput( richTextCommands, "{bg=Grey}", pushBGGrey );
	shput( richTextCommands, "{bg=White}", pushBGWhite );
	shput( richTextCommands, "{fg=Black}", pushFGBlack );
	shput( richTextCommands, "{fg=DarkGrey}", pushFGDarkGrey );
	shput( richTextCommands, "{fg=DarkBlue}", pushFGDarkBlue );
	shput( richTextCommands, "{fg=Blue}", pushFGBlue );
	shput( richTextCommands, "{fg=DarkGreen}", pushFGDarkGreen );
	shput( richTextCommands, "{fg=Green}", pushFGGreen );
	shput( richTextCommands, "{fg=DarkCyan}", pushFGDarkCyan );
	shput( richTextCommands, "{fg=Cyan}", pushFGCyan );
	shput( richTextCommands, "{fg=Maroon}", pushFGMaroon );
	shput( richTextCommands, "{fg=Red}", pushFGRed );
	shput( richTextCommands, "{fg=Purple}", pushFGPurple );
	shput( richTextCommands, "{fg=Magenta}", pushFGMagenta );
	shput( richTextCommands, "{fg=Brown}", pushFGBrown );
	shput( richTextCommands, "{fg=Yellow}", pushFGYellow );
	shput( richTextCommands, "{fg=Grey}", pushFGGrey );
	shput( richTextCommands, "{fg=White}", pushFGWhite );
	shput( richTextCommands, "{/fg}", popFGColor );
	shput( richTextCommands, "{/bg}", popBGColor );
}

Coord vdrawStringIgnoreSize( Window* screen, int16_t x, int16_t y, SmallRect border, const char* str, Attributes attributes, va_list args )
{
	if( str == NULL ) {
		Coord out;
		out.x = x;
		out.y = y;
		return out;
	}

	StringRenderState state = createStringRenderState( attributes );
	WORD baseAttr = attributes.attributes & ~( BG_FILTER | FG_FILTER );
	Attributes currAttr = attributes;

	// todo: want to move runReplacements() out of here
	char* replacedString = runReplacements( str, CAPTYPE_IGNORE, NULL );
	vsnprintf( otherTempStrBuffer, ARRAYSIZE( otherTempStrBuffer ), str, args );
	arrfree( replacedString );

	int16_t startX = x;
	size_t len = strlen( otherTempStrBuffer );
	Coord endPoint;
	endPoint.x = x;
	endPoint.y = y;
	bool inRichTextTag = false;
	size_t tagStart;

	for( size_t i = 0; i < len; ++i ) {
		if( inRichTextTag ) {
			if( otherTempStrBuffer[i] == '}' ) {
				size_t len = i - tagStart + 1;
				char* tag = mem_Allocate( len + 1 );
				memcpy( tag, &(otherTempStrBuffer[tagStart]), len );
				tag[len] = 0;
				RichTextCommand command = shget( richTextCommands, tag );
				if( command != NULL ) {
					command( &state );
				}
				mem_Release( tag );

				// update the attributes based on the stack
				currAttr.attributes = baseAttr | arrlast( state.arrFGStack ) | arrlast( state.arrBGStack );

				inRichTextTag = false;
			}
		} else {
			if( otherTempStrBuffer[i] == '{' ) {
				inRichTextTag = true;
				tagStart = i;
			} else {
				if( otherTempStrBuffer[i] != '\n' ) {
					putChar( screen, otherTempStrBuffer[i], currAttr, endPoint, border );
					++endPoint.x;
				}

				if( ( otherTempStrBuffer[i] == '\n' ) || ( endPoint.x > border.right ) ) {
					endPoint.x = startX;
					++endPoint.y;
				}

				if( endPoint.y > border.bottom ) {
					return endPoint;
				}
			}
		}
	}

	cleanStringRenderState( &state );

	return endPoint;
}

Coord drawStringIgnoreSize( Window* screen, int16_t x, int16_t y, SmallRect border, const char* str, Attributes attributes, ... )
{
	Coord c;
	va_list args;
	va_start( args, attributes );
	c = vdrawStringIgnoreSize( screen, x, y, border, str, attributes, args );
	va_end( args );
	return c;
}

void strlenMinusTags( const char* str, size_t* outTotal, size_t* outMinusTags )
{
	// all text between '{' and '}' are considered in a tag, except if the you have '/{'
	( *outTotal ) = 0;
	( *outMinusTags ) = 0;

	bool inTag = false;
	while( *str ) {
		++( *outTotal );

		if( *str == '{' ) {
			inTag = true;
		} else if( *str == '}' ) {
			inTag = false;
		} else if( !inTag ) {
			++( *outMinusTags );
		}

		++str;
	}
}

size_t minimum( size_t a, size_t b )
{
	return a < b ? a : b;
}

Coord vdrawString( Window* screen, int16_t x, int16_t y, SmallRect border, const char* str, Attributes attributes, va_list args )
{
	if( str == NULL ) {
		Coord out;
		out.x = x;
		out.y = y;
		return out;
	}

	StringRenderState state = createStringRenderState( attributes );
	WORD baseAttr = attributes.attributes & ~( BG_FILTER | FG_FILTER );
	Attributes currAttr = attributes;

	vsnprintf( otherTempStrBuffer, ARRAYSIZE( otherTempStrBuffer ), str, args );
	
	// todo: want to move runReplacements() out of here
	char* replacedString = runReplacements( otherTempStrBuffer, CAPTYPE_IGNORE, NULL );
	assert( ARRAYSIZE( otherTempStrBuffer ) >= arrlenu( replacedString ) ); // notification if we need to increase size of otherTempStrBuffer
	memcpy( otherTempStrBuffer, replacedString, minimum( ARRAYSIZE( otherTempStrBuffer ), arrlenu( replacedString ) ) );
	arrfree( replacedString );

#define NEWLINE { endPoint.x = startX; ++endPoint.y; }
	Coord endPoint;
	int16_t startX = x;
	size_t len = strlen( otherTempStrBuffer );
	size_t strPos = 0;
	endPoint.x = x;
	endPoint.y = y;

	strncpy_s( &( tempStrBuffer[0] ), ARRAYSIZE( tempStrBuffer ), otherTempStrBuffer, ARRAYSIZE( otherTempStrBuffer ) );

	if( otherTempStrBuffer[0] == ' ' ) {
		putChar( screen, ' ', currAttr, endPoint, border );
		++( endPoint.x );
		++strPos;
		if( endPoint.x > border.right ) {
			NEWLINE
		}
	}

	char* context = NULL;
	char* tok = strtok_s( tempStrBuffer, " ", &context );

	while( tok != NULL ) {
		size_t tokLen;
		size_t printTokLen;
		strlenMinusTags( tok, &tokLen, &printTokLen );

		// first see if there's enough room to write the word
		if( (size_t)( border.right - endPoint.x + 1 ) < printTokLen ) {
			endPoint.x = startX;
			++endPoint.y;
		} else {
			bool inTag = false;
			size_t tagStart = 0;

			for( size_t i = 0; i < tokLen; ++i ) {
				if( tok[i] == '\n' ) {
					NEWLINE
				} else {
					if( inTag ) {
						if( tok[i] == '}' ) {
							size_t len = i - tagStart + 1;
							char* tag = mem_Allocate( len + 1 );
							memcpy( tag, &( tok[tagStart] ), len );
							tag[len] = 0;
							RichTextCommand command = shget( richTextCommands, tag );
							if( command != NULL ) {
								command( &state );
							}
							mem_Release( tag );

							// update the attributes based on the stack
							currAttr.attributes = baseAttr | arrlast( state.arrFGStack ) | arrlast( state.arrBGStack );

							inTag = false;
						}
					} else {
						if( tok[i] == '{' ) {
							inTag = true;
							tagStart = i;
						} else {
							putChar( screen, tok[i], currAttr, endPoint, border );
							++endPoint.x;
						}
					}
					++strPos;
				}
			}

			// if we're not at the end of the string, that means we ran into a space, write it out
			if( otherTempStrBuffer[strPos] != 0 ) {
				putChar( screen, ' ', currAttr, endPoint, border );
				++endPoint.x;
				++strPos;
			}
			tok = strtok_s( NULL, " ", &context );
		}

		if( endPoint.x > border.right ) {
			NEWLINE
		}

		if( endPoint.y > border.bottom ) {
			return endPoint;
		}
	}

	cleanStringRenderState( &state );

	return endPoint;

#undef NEWLINE
}

Coord drawString( Window* screen, int16_t x, int16_t y, SmallRect border, const char* str, Attributes attributes, ... )
{
	Coord c;
	va_list args;
	va_start( args, attributes );
	c = vdrawString( screen, x, y, border, str, attributes, args );
	va_end( args );
	return c;
}

void centerStringHoriz( Window* screen, SmallRect area, int16_t y, const char* str, Attributes attributes, ... )
{
	va_list args;
	va_start( args, attributes );
	// want to measure the string by it's longest line
	strncpy_s( &( tempStrBuffer[0] ), ARRAYSIZE( tempStrBuffer ), str, strlen( str ) );
	int16_t x = 0;
	size_t maxLen = 0;
	char* context = NULL;
	char* tok = strtok_s( tempStrBuffer, "\n", &context );
	while( tok != NULL ) {
		size_t len = strlen( tok );
		if( len > maxLen ) {
			maxLen = len;
			size_t strMid = len / 2;
			int16_t renderMid = ( ( area.right + area.left ) / 2 );
			x = renderMid - (int16_t)strMid;
		}

		tok = strtok_s( NULL, "\n", &context );
	}

	vdrawStringIgnoreSize( screen, x, y, area, str, attributes, args );
	va_end( args );
}

void drawBorder( Window* screen )
{
	Attributes attr = attr_Clrs( PDC_GREY, PDC_BLACK );
	// draw corners
	simplePutChar( screen, 201, attr, 0, 0 );
	simplePutChar( screen, 200, attr, 0, SCREEN_HEIGHT - 1 );
	simplePutChar( screen, 187, attr, SCREEN_WIDTH - 1, 0 );
	simplePutChar( screen, 188, attr, SCREEN_WIDTH - 1, SCREEN_HEIGHT - 1 );

	// draw top and bottom
	for( int x = 1; x < SCREEN_WIDTH - 1; ++x ) {
		simplePutChar( screen, 205, attr, x, SCREEN_HEIGHT - 1 );
		simplePutChar( screen, 205, attr, x, 0 );
	}

	// draw left and right
	for( int y = 1; y < SCREEN_HEIGHT - 1; ++y ) {
		simplePutChar( screen, 186, attr, 0, y );
		simplePutChar( screen, 186, attr, SCREEN_WIDTH - 1, y );
	}

	// draw top label
	if( topTitle != NULL ) {
		centerStringHoriz( screen, windowSize, 0, topTitle, attr_Clrs( PDC_WHITE, PDC_BLACK ) );
	}
}

void startDraw( Window* screen)
{
	memset( screen->buffer, 0, sizeof( screen->buffer[0] ) * ARRAYSIZE( screen->buffer ) );
	drawBorder( screen );
}

void endDraw( Window* screen )
{
	COORD localBufferSize = { (SHORT)bufferSize.x, (SHORT)bufferSize.y };
	COORD localTopLeft = { (SHORT)topLeft.x, (SHORT)topLeft.y };
	SMALL_RECT localWindowSize = fromAPI( windowSize );
	WriteConsoleOutput( screen->write, screen->buffer, localBufferSize, localTopLeft, &localWindowSize );
	windowSize = fromWindows( localWindowSize );
}

Input getNextInput( Window* screen )
{
	DWORD evtCnt = 0;
	DWORD evtRead = 0;

	GetNumberOfConsoleInputEvents( screen->read, &evtCnt );
	if( evtCnt <= 0 ) {
		return IN_NONE;
	}

	INPUT_RECORD inputRec;
	ReadConsoleInput( screen->read, &inputRec, 1, &evtRead );
	if( evtRead <= 0 ) {
		return IN_NONE;
	}

	if( ( inputRec.EventType == KEY_EVENT ) && inputRec.Event.KeyEvent.bKeyDown ) {
		if( inputRec.Event.KeyEvent.wVirtualKeyCode == VK_UP ) {
			return IN_UP;
		} else if( inputRec.Event.KeyEvent.wVirtualKeyCode == VK_DOWN ) {
			return IN_DOWN;
		} else if( inputRec.Event.KeyEvent.wVirtualKeyCode == VK_LEFT ) {
			return IN_LEFT;
		} else if( inputRec.Event.KeyEvent.wVirtualKeyCode == VK_RIGHT ) {
			return IN_RIGHT;
		} else if( inputRec.Event.KeyEvent.wVirtualKeyCode == VK_RETURN ) {
			return IN_ENTER;
		} else if( inputRec.Event.KeyEvent.wVirtualKeyCode == VK_SPACE ) {
			return IN_SPACE;
		} else if( ( inputRec.Event.KeyEvent.wVirtualKeyCode == VK_ADD ) ||
			( inputRec.Event.KeyEvent.uChar.AsciiChar == '=' ) ||
			( inputRec.Event.KeyEvent.uChar.AsciiChar == '+' ) ) {
			return IN_PLUS;
		} else if( ( inputRec.Event.KeyEvent.wVirtualKeyCode == VK_SUBTRACT ) ||
			( inputRec.Event.KeyEvent.uChar.AsciiChar == '-' ) ||
			( inputRec.Event.KeyEvent.uChar.AsciiChar == '_' ) ) {
			return IN_MINUS;
		} else if( ( inputRec.Event.KeyEvent.uChar.AsciiChar >= '1' ) &&
			( inputRec.Event.KeyEvent.uChar.AsciiChar <= '9' ) ) {
			return ( IN_OPT_1 + ( inputRec.Event.KeyEvent.uChar.AsciiChar - '1' ) );
		} else if( ( inputRec.Event.KeyEvent.uChar.AsciiChar == 'c' ) ||
			( inputRec.Event.KeyEvent.uChar.AsciiChar == 'C' ) ) {
			return IN_C;
		} else if( ( inputRec.Event.KeyEvent.uChar.AsciiChar == 'h' ) ||
			( inputRec.Event.KeyEvent.uChar.AsciiChar == 'H' ) ) {
			return IN_H;
		}
		return IN_OTHER;
	}

	return IN_NONE;
}

void waitForAnyInput( Window* screen )
{
	Input input;
	do {
		input = getNextInput( screen );
	} while( input == IN_NONE );
}

void eatAllInputs( Window* screen )
{
	Input input;
	do {
		input = getNextInput( screen );
	} while( input != IN_NONE );
}

void drawDebugRect( Window* screen, SmallRect rect, Attributes attributes )
{
	mem_Verify( );
	for( SHORT y = rect.top; y <= rect.bottom; ++y ) {
		mem_Verify( );
		for( SHORT x = rect.left; x <= rect.right; ++x ) {
			mem_Verify( );
			if( ( x + y ) % 2 == 0 ) {
				mem_Verify( );
				simplePutChar( screen, '#', attributes, x, y );
				mem_Verify( );
			} else {
				mem_Verify( );
				simplePutChar( screen, '.', attributes, x, y );
				mem_Verify( );
			}
		}
	}
}

Window* createWindow( const char* title )
{
	Window* win = mem_Allocate( sizeof( Window ) );
	if( win == NULL ) return NULL;

	win->write = GetStdHandle( STD_OUTPUT_HANDLE );
	win->read = GetStdHandle( STD_INPUT_HANDLE );

	COORD localBufferSize = { (SHORT)bufferSize.x, (SHORT)bufferSize.y };

	SMALL_RECT localWindowSize = fromAPI( windowSize );
	SetConsoleWindowInfo( win->write, TRUE, &localWindowSize );
	windowSize = fromWindows( localWindowSize );
	SetConsoleScreenBufferSize( win->write, localBufferSize );

	SetConsoleTitle( title );

	return win;
}

void destroyWindow( Window* screen )
{
	mem_Release( screen );
}

Attributes attr_Clrs( PredefinedColors foreground, PredefinedColors background )
{
	Attributes attr = attr_FGClr( foreground );

	switch( background ) {
	case PDC_BLACK: attr.attributes |= BG_BLACK; break;
	case PDC_DARK_GREY: attr.attributes |= BG_DARK_GREY; break;
	case PDC_DARK_BLUE: attr.attributes |= BG_DARK_BLUE; break;
	case PDC_BLUE: attr.attributes |= BG_BLUE; break;
	case PDC_DARK_GREEN: attr.attributes |= BG_DARK_GREEN; break;
	case PDC_GREEN: attr.attributes |= BG_GREEN; break;
	case PDC_DARK_CYAN: attr.attributes |= BG_DARK_CYAN; break;
	case PDC_CYAN: attr.attributes |= BG_CYAN; break;
	case PDC_MAROON: attr.attributes |= BG_MAROON; break;
	case PDC_RED: attr.attributes |= BG_RED; break;
	case PDC_PURPLE: attr.attributes |= BG_PURPLE; break;
	case PDC_MAGENTA: attr.attributes |= BG_MAGENTA; break;
	case PDC_BROWN: attr.attributes |= BG_BROWN; break;
	case PDC_YELLOW: attr.attributes |= BG_YELLOW; break;
	case PDC_GREY: attr.attributes |= BG_GREY; break;
	case PDC_WHITE: attr.attributes |= BG_WHITE;  break;
	}

	return attr;
}

Attributes attr_FGClr( PredefinedColors foreground )
{
	Attributes attr;
	attr.attributes = 0;

	switch( foreground ) {
	case PDC_BLACK: attr.attributes = FG_BLACK; break;
	case PDC_DARK_GREY: attr.attributes = FG_DARK_GREY; break;
	case PDC_DARK_BLUE: attr.attributes = FG_DARK_BLUE; break;
	case PDC_BLUE: attr.attributes = FG_BLUE; break;
	case PDC_DARK_GREEN: attr.attributes = FG_DARK_GREEN; break;
	case PDC_GREEN: attr.attributes = FG_GREEN; break;
	case PDC_DARK_CYAN: attr.attributes = FG_DARK_CYAN; break;
	case PDC_CYAN: attr.attributes = FG_CYAN; break;
	case PDC_MAROON: attr.attributes = FG_MAROON; break;
	case PDC_RED: attr.attributes = FG_RED; break;
	case PDC_PURPLE: attr.attributes = FG_PURPLE; break;
	case PDC_MAGENTA: attr.attributes = FG_MAGENTA; break;
	case PDC_BROWN: attr.attributes = FG_BROWN; break;
	case PDC_YELLOW: attr.attributes = FG_YELLOW; break;
	case PDC_GREY: attr.attributes = FG_GREY; break;
	case PDC_WHITE: attr.attributes = FG_WHITE;  break;
	}

	return attr;
}

void testShit( Attributes attr )
{
	int x = 0;
	attr.attributes = 0;
	x = 0;
}