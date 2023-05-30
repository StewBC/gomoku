/*---------------------------------------------------------------------------*\
  Gomoku By Stefam Wessels, 2023
  This is free and unencumbered software released into the public domain.
\*---------------------------------------------------------------------------*/

#ifndef _PLAT_H_
#define _PLAT_H_

#include "globals.h"

#define SET_BIT(x)                (1<<(x))
// Keys are mapped to a mask that has these bits set if corresponding keys were pressed in plat_ReadKeys
#define INPUT_UP                  SET_BIT(0)
#define INPUT_RIGHT               SET_BIT(1)
#define INPUT_DOWN                SET_BIT(2)
#define INPUT_LEFT                SET_BIT(3)
#define INPUT_ESCAPE              SET_BIT(4)
#define INPUT_SELECT              SET_BIT(5)
#define INPUT_UNDO                SET_BIT(6)
#define INPUT_REDO                SET_BIT(7)

// Do all platform specific initialisation here
void plat_Init();
// If plat_CanQuit returns 1, when the app quits, this is called to clean up/shut the app down
void plat_Shutdown();
// Return 1 if the app can quit (to OS for example) or 0 if there should be no Quit option on the menu
uchar plat_CanQuit();
// Called to seed random number generator
void plat_SeedRandom();
// Return a random number between 0 and max-1
uchar plat_Random(uchar max);
// Clear the whole screen
void plat_ClearScreen();
// Show the text at colX but rawY is a pixel value.  Style is 0..3 for white/black and inverse or other versions
void plat_ShowText(uint rawY, uchar colX, uchar style, char* text);
// Show some sort of title/splash screen
uchar plat_ShowSplash();
// Take helpStrings and present to the user in a nice looking way
void plat_ShowHelp();
// Show the Menu and return the selection - see plat*.c for more info.  Return 255 for timeout and 254 on INPUT_ESCAPE
uchar plat_Menu(Menu *m, uchar timeout);
// Draw the whole board, incl. any played pieces
void plat_ShowBoard();
// Update the move_y/move_x location of the board (win-cmd simply calls plat_ShowBoard)
void plat_ShowMove();
// Show or hide the cursor ay row y, col x.  On true is show.  Always done in pairs so XOR okay and on ignored
void plat_SelectCursor(uchar y, uchar x, uchar on);
// Show however much of the last moves as works on the platform. Extract moves from stack[]
void plat_LogMove();
// Winner 0 - draw, otherwise player 1 or 2
void plat_Winner(uchar winner);
// Set current_time to 0 (current_time is supposed to count in seconds)
void plat_TimeReset();
// Return current_time
plat_TimeType plat_TimeGet();
// Current_ticks++ and when current_ticks > time_scale, current_time++
void plat_TimeTick();
// Set time_scale (how many TimeTicks increment current_time which is supposed to count in seconds)
void plat_TimeScale(plat_TimeType aScle);
// Return true when current_time > aTime + elpased 
uchar plat_TimePassed(plat_TimeType aTime, uchar elapsed);
// Return true if plat_ReadKeys will not block (so a key was pressed)
uchar plat_CheckKey();
// Read a key and turn it into a INPUT_* mask if appropriate.  Return -1 if timeout current_time ticks passed
int plat_ReadKeys(uchar timeout);

#endif //_PLAT_H_
