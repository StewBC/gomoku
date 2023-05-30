/*---------------------------------------------------------------------------*\
  Gomoku By Stefam Wessels, 2023
  This is free and unencumbered software released into the public domain.
\*---------------------------------------------------------------------------*/

#ifndef _GLOBALS_H_
#define _GLOBALS_H_

// uchar and schar used for just about everything
typedef unsigned char uchar;
typedef signed char schar;
// uint only used as the Y in plat_ShowText
typedef unsigned int uint;
typedef unsigned long plat_TimeType;

typedef struct 
{
    uchar count;        // Number of menu items cc65 cannot start with char*[]
    char *MenuItems[];  // The menu items text
} MenuItems;

typedef struct
{
    uchar item;         // The menuItem that offers a choice between some settings
    MenuItems* select;  // The settings' names
} ItemChoices;

typedef struct
{
    char *Title;                // Menu has a title
    MenuItems *menuItems;       // and items
    uchar numChoices;           // and some number of items have choices
    ItemChoices *itemChoices;   // and which items have which choices
    uchar activeChoice[];       // and what are the currently selected choices
} Menu;

#define BOARD_X 15          // size of the board
#define BOARD_Y 15
#define WIN_LEN 5           // How many in a row needed
#define STACK_SIZE 256      // size of undo/redo stack

// Global variables that are also needed by the platform specific code
extern uchar board[BOARD_Y][BOARD_X];
extern uchar playerType[2];
extern uchar moveStack[STACK_SIZE];
extern uchar sp;
extern schar player, move_y, move_x;

#endif // _GLOBALS_H_
