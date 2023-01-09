/*---------------------------------------------------------------------------*\
  Gomoku By Stefam Wessels, 2023
  This is free and unencumbered software released into the public domain.
\*---------------------------------------------------------------------------*/

#include <apple2.h>
#include <stdlib.h>
#include <stdio.h>
#include <conio.h>
#include <string.h>
#include <peekpoke.h>
#include "../globals.h"
#include "../plat.h"
#include "data.h"

#define TXTCLR              0xC050      /* Display graphics */
#define TXTSET              0xC051      /* Display text */
#define MIXCLR              0xC052      /* Disable 4 lines of text */
#define MIXSET              0xC053      /* Enable 4 lines of text */
#define LOWSCR              0xC054      /* Page 1 */
#define HISCR               0xC055      /* Page 2 */
#define LORES               0xC056      /* Lores graphics */
#define HIRES               0xC057      /* Hires graphics */

#define PAGE1MEM            0x2000      /* HGR Page 1 memory address */
#define PAGE2MEM            0x4000      /* HGR Page 2 memory address */
#define PAGE_SIZE           0x2000      /* Byte size of an HGR memory Page */

#define SCREEN_HEIGHT_GFX   192         /* Pixels vertical */
#define COLOR_WIDTH         140         /* Pixels horizontal (2 bpp) */
#define SCREEN_HEIGHT       (SCREEN_HEIGHT_GFX/8)

extern uchar __fastcall__ asm_DetectMachine();
extern void  __fastcall__ asm_ClearHiRes();
extern void  __fastcall__ asm_ShowTile(uchar row, uchar col, const char* even, const char* odd);
extern void  __fastcall__ asm_ShowFont(uchar row, uchar col, uchar style, const char* character);
extern void  __fastcall__ asm_InvertSquare(uchar row, uchar col);
extern void  __fastcall__ asm_DecompSplash();

// String to hold dynamic display strings (sprintf dest)
char string[21];
plat_TimeType current_time = 0, current_ticks = 0, time_scale = 800;

// Help text specific to the Apple 2
char* helpStrings[] =
{
    "The object of the",
    "game is to get",
    "exactly five game ",
    "pieces in a row on",
    "a vertical line, a",
    "horizontal line, or",
    "a diagonal line.",
    "",
    "Use WASD or cursor",
    "keys to navigate",
    "menus or move the",
    "selector on the",
    "game board.",
    "",
    "ENTER or SPACE to",
    "place a game piece.",
    "",
    "U or R for Undo or",
    "Redo.",
    "",
    "ESC to quit.",
};
const uchar numHelpStrings = sizeof(helpStrings) / sizeof(helpStrings[0]);


/*
 * Graphics mode, page 1
 */
void plat_Init()
{
    POKE(TXTCLR, 0);                    // Display Graphics
    POKE(LOWSCR, 0);                    // Set Page 1 "On"
    POKE(HIRES, 0);                     // 280x192 Monochrome or 140x192 color
    POKE(MIXCLR, 0);                    // Disable 4 lines of text

    // Detect the IIgs and IIc+ and make the delay longer
    // Does not detect any accelerators
    if(asm_DetectMachine() & 1)
        plat_TimeScale(time_scale * 3);
}


/*
 * Back to text mode
 */
void plat_Shutdown()
{
    POKE(TXTSET, 0);
}

/*
 * See if platform can quit the app back to some OS
 */
uchar plat_CanQuit()
{
    return 1;
}

/*
 * Platform independent way to get a platform specific random number seed
 */
void plat_SeedRandom()
{
}

/*
 * Platform independent way to get a platform specific random number
 */
uchar plat_Random(uchar max)
{
    return rand() % max;
}

/*
 * Uses an asm routine to clear the screen
 */
void plat_ClearScreen()
{
    asm_ClearHiRes();
}

/*
 * Draws a null terminated string at y (0..192) and X (a column number) in one of the 
 * 4 styles.  Does not clip at all (so 0..183 for the 8 pixel high font)
 */
void plat_ShowText(uint rawY, uchar colX, uchar style, char* text)
{
    char c;
    uchar index = 0;
    while((c = text[index++]))
    {
        if(c >= 96)
            c -= 56;
        else if(c >= 65)
            c -= 50;
        else if(c >= 44)
            c -= 43;
        else
            c -= 32;
        asm_ShowFont(rawY, colX++, style, fontData[c]);
    }
}

/*
 * Decompress the LZSA title screen image to the screen, and wait for a key or timeout
 * if timeout return -1, ESC return ESC else return 0
 */
uchar plat_ShowSplash()
{
    int key;
    asm_DecompSplash();
    key = plat_ReadKeys(15);
    plat_ClearScreen();
    return key == -1 ? 1 : key == INPUT_ESCAPE ? key : 0;
}

/*
 * Show the prepared help text for the apple 2
 */
void plat_ShowHelp()
{
    uchar i, y = 16;

    plat_ClearScreen();
    plat_ShowText(0,7,3,"Gomoku");
    for(i = 0; i < numHelpStrings; i++, y += 8)
    {
        plat_ShowText(y, 0, i < 7 ? 0 : 1, helpStrings[i]);
    }
    plat_ShowText(183, 0, 2, "Any key to continue ");
    plat_ReadKeys(60);
    plat_ClearScreen();
}

/*
 * Show and process a menu until a non ItemChoices item is selected, or ESC is pressed, or
 * the menu is idle for timeout (seconds)
 */
uchar plat_Menu(Menu* m, uchar timeout)
{
    uchar i, y, sel = 0, j = 0;
    uchar spacingY = SCREEN_HEIGHT_GFX / (m->menuItems->count + 4);
    uchar yTop = (SCREEN_HEIGHT_GFX - (spacingY * m->menuItems->count)) / 2 ;
    plat_TimeType now = plat_TimeGet();

    plat_ShowText(4,(20-strlen(m->Title))/2, 0, m->Title);

    y = yTop;
    for(i =0; i < m->menuItems->count; i++)
    {
        plat_ShowText(y, 1, sel == i ? 3 : 0, m->menuItems->MenuItems[i]);
        if(j < m->numChoices)
        {
            if(m->itemChoices[j].item == i)
            {
                plat_ShowText(y, 10, 1, m->itemChoices[j].select->MenuItems[m->activeChoice[j]]);
                j++;
            }
        }
        y += spacingY;
    }

    while(1)
    {
        int keyMask = plat_ReadKeys(timeout);
        // Timeout returns 255
        if(keyMask == -1)
            return 255;
        y = yTop + spacingY * sel;
        switch(keyMask)
        {
            case INPUT_UP:
            case INPUT_LEFT:
                plat_ShowText(y, 1, 0, m->menuItems->MenuItems[sel]);
                if(--sel > m->menuItems->count)
                    sel = m->menuItems->count - 1;
                y = yTop + spacingY * sel;
                plat_ShowText(y, 1, 3, m->menuItems->MenuItems[sel]);
                break;

            case INPUT_DOWN:
            case INPUT_RIGHT:
                plat_ShowText(y, 1, 0, m->menuItems->MenuItems[sel]);
                if(++sel >= m->menuItems->count)
                    sel = 0;
                y = yTop + spacingY * sel;
                plat_ShowText(y, 1, 3, m->menuItems->MenuItems[sel]);                    
                break;

            case INPUT_SELECT:
                j = 0;
                // See if selected item is an ItemChoices item, so processed internally
                for(i=0; i < m->numChoices; i++)
                {
                    if(sel == m->itemChoices[i].item)
                    {
                        j = 1;
                        if(++m->activeChoice[i] >= m->itemChoices[i].select->count)
                            m->activeChoice[i] = 0;
                        plat_ShowText(y, 10, 1, m->itemChoices[i].select->MenuItems[m->activeChoice[i]]);
                        break;
                    }
                }
                // Non ItemChoices item selected, return the item index
                if(!j)
                {
                    plat_ClearScreen();
                    return sel;
                }
                break;

            // esc returns 254
            case INPUT_ESCAPE:
                return 254;

        }
    }
}

/*
 * Draw the board with labels
 */
void plat_ShowBoard()
{
    asm_ShowTile(0, 1, tileData[EVEN_BLANK], tileData[ODD_CORNER_UL]);
    asm_ShowTile(0, 2+BOARD_X, tileData[EVEN_LINE_HORIZONTAL_DIV], tileData[ODD_LINE_DOWN_TOP_HALF]);

    for(move_y = 0; move_y < BOARD_Y; move_y++)
    {
        sprintf(string,"%c",'a'+move_y);
        plat_ShowText((1+move_y)*TILE_HEIGHT, 0, 0, string);
        asm_ShowTile(0, 2+move_y, tileData[EVEN_LINE_HORIZONTAL_DIV], tileData[ODD_CORNER_UL]);
        asm_ShowTile(1+move_y, 1, tileData[EVEN_BLANK], tileData[ODD_LEFT_DOWN_HORIZONTAL_DIV]);
        asm_ShowTile(1+move_y, 2+BOARD_Y, tileData[EVEN_LINE_HORIZONTAL_DIV], tileData[ODD_LINE_DOWN]);
        asm_ShowTile(BOARD_Y+1, 2+move_y, tileData[EVEN_LINE_HORIZONTAL_DIV], tileData[ODD_CORNER_LL]);        
        for(move_x=0; move_x < BOARD_X ; move_x++)
        {
            if(board[move_y][move_x])
            {
                plat_ShowMove();
            }
            else
            {
                asm_ShowTile(1+move_y, 2+move_x, tileData[EVEN_LINE_HORIZONTAL_DIV], tileData[ODD_LEFT_DOWN_HORIZONTAL_DIV]);
            }
        }
    }
    asm_ShowTile(1+BOARD_Y, 1, tileData[EVEN_BLANK], tileData[ODD_CORNER_LL]);
    asm_ShowTile(1+BOARD_Y, 2+BOARD_X, tileData[EVEN_LINE_HORIZONTAL_DIV], tileData[ODD_LINE_DOWN_BOTTOM_HALF]);
    plat_ShowText(192-8, 2, 0, "ABCDEFGHIJKLMNO");
}

/*
 * Draw a tile on the board based on the piece, or lack of piece, in the position on the board
 * uses move_y and move_x as the draw positions
 */
void plat_ShowMove()
{
    uchar piece = board[move_y][move_x];
    if(piece)
    {
        if(1 == piece)
            asm_ShowTile(1+move_y, 2+move_x, tileData[EVEN_BLACK], tileData[ODD_BLACK]);
        else
            asm_ShowTile(1+move_y, 2+move_x, tileData[EVEN_WHITE], tileData[ODD_WHITE]);
    }
    else
    {
        asm_ShowTile(1+move_y, 2+move_x, tileData[EVEN_LINE_HORIZONTAL_DIV], tileData[ODD_LEFT_DOWN_HORIZONTAL_DIV]);
    }
}

/*
 * Use asm routine to mask, unmask the MSB of the drawn tile, thus highlighting it
 */
void plat_SelectCursor(uchar y, uchar x, uchar )
{
    asm_InvertSquare(1+y, 2+x);
}

/*
 * Redraw the visible log up to SP
 */
void plat_LogMove()
{
    #define TITLE_ROWS 2
    uchar color, start, skip, row = TITLE_ROWS;

    if(sp >= (SCREEN_HEIGHT - TITLE_ROWS - 1))
    {
        start = sp - (SCREEN_HEIGHT - TITLE_ROWS - 1);
        skip = 0;
    }
    else
    {
        start = 0;
        skip = (SCREEN_HEIGHT - TITLE_ROWS - 1) - sp;
    }

    while(skip--)
    {
        plat_ShowText(row*8, 18, 0, "  ");
        row++;
    }

    if(start & 1)
        color = 1;
    else
        color = 0;

    while(start < sp)
    {
        uchar move = stack[start];
        sprintf(string, "%c%c", 'a'+(move >> 4), 'A'+(move & 0x0f));
        color ^= 1;
        plat_ShowText(row*8, 18, color, string);
        row++;
        start++;
    }
}

/*
 * Show the winner and wait for a key/timeout
 */
void plat_Winner(uchar winner)
{
    if(winner)
    {
        sprintf(string, "Player %d wins in %3d", winner, sp);
        plat_ShowText(4, 0, 4-winner, string);
    }
    else
    {
        plat_ShowText(4, 0, 3, "    It is a draw    ");
    }
}

/*
 * current_time is a simulated seconds counter.  Set that to 0
 */
void plat_TimeReset()
{
    current_time = 0;
}

/*
 * return the simulated seconds to the caller
 */
plat_TimeType plat_TimeGet()
{
    return current_time;
}

/*
 * once the ticks exceed the scale, add a simulated second and reset the ticks
 */
void plat_TimeTick()
{
    if(++current_ticks > time_scale)
    {
        current_time++;
        current_ticks = 0;
    }
}

/*
 * Set the scale - can be used to calibrate for a simulated second
 */
void plat_TimeScale(plat_TimeType aScale)
{
    time_scale = aScale;
}

/*
 * if current simulated seconds is more than the a snapshot time plus some elapsed time
 * return 1, else 0
 */
uchar plat_TimePassed(plat_TimeType aTime, uchar elapsed)
{
    if(current_time > aTime + elapsed)
        return 1;
    return 0;
}

/*
 * Used to test if chetc will block or not
 */
uchar plat_CheckKey()
{
    return kbhit();
}

/*
 * Get a key or time out (return -1) and translate that key into a key mask bit setting
 */
int plat_ReadKeys(uchar timeout)
{
    plat_TimeType now = plat_TimeGet();
    int keyMask = 0;
    uchar key = 0;

    while(!kbhit())
    {
        plat_TimeTick();
        if(timeout && plat_TimePassed(now, timeout))
            return -1;
    }

    key = cgetc();
    switch(key)
    {
        case 'A':
        case 'a':
        case CH_CURS_LEFT:
            keyMask |= INPUT_LEFT;
        break;

        case 'D':
        case 'd':
        case CH_CURS_RIGHT:
            keyMask |= INPUT_RIGHT;
        break;

        case 'W':
        case 'w':       // Reasonably located on the ][ keyboard
        case 0x0B:      // No CH_CURS_UP in apple2.h, only in apple2enh.h
            keyMask |= INPUT_UP;
        break;

        case 'S':
        case 's':       // Reasonably located on the ][ keyboard
        case 0x0A:      // No CH_CURS_DOWN in apple2.h, only in apple2enh.h
            keyMask |= INPUT_DOWN;
        break;

        case CH_ESC:
            keyMask |= INPUT_ESCAPE;
        break;

        case ' ':
        case CH_ENTER:
            keyMask |= INPUT_SELECT;
        break;

        case 'R':
        case 'r':
            keyMask |= INPUT_REDO;
        break;

        case 'U':
        case 'u':
            keyMask |= INPUT_UNDO;
        break;
    }

    return keyMask;
}