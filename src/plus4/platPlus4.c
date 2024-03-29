/*---------------------------------------------------------------------------*\
  Gomoku By Stefam Wessels, 2023
  This is free and unencumbered software released into the public domain.
\*---------------------------------------------------------------------------*/

#include <plus4.h>
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <string.h>
#include "../plat.h"
#include "data.h"

// System locations
#define BITMAP_ADDRESS          (0x8000)
#define LUMINANCE_ADDERSS       (0x0800)
#define COLOR_ADDERSS           (LUMINANCE_ADDERSS + 0x0400)

#define SCREEN_WIDTH            40
#define SCREEN_HEIGHT           25

// assembly external function prototypes.  See .s files
extern void  __fastcall__ asm_ClearScreen();
extern void  __fastcall__ asm_ShowFont(uchar row, uchar col, uchar style, const char* character);
extern void  __fastcall__ asm_ShowTile(uchar row, uchar col, const char* even, const char* odd);
extern void  __fastcall__ asm_DecompSplash();

// String to hold dynamic display strings (sprintf dest)
char string[21];
// Back up a few Plus4 specific hardware values
static char t_bcol, t_bgco, t_colo, t_vscr, t_hscr, t_misc, t_vide;
plat_TimeType current_time = 0, current_ticks = 0, time_scale = 800;

// Help text specific to the C64
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
    "RUN-STOP to quit.",
};
const uchar numHelpStrings = sizeof(helpStrings) / sizeof(helpStrings[0]);

/*
 * Graphics mode
 */
void plat_Init()
{
    if(plat_CanQuit())
    {
        // Save these values so they can be restored
        t_bcol = TED.bordercolor;
        t_bgco = TED.bgcolor;
        t_colo = TED.color1;
        t_vscr = TED.vscroll;
        t_hscr = TED.hscroll;
        t_misc = TED.misc;
        t_vide = TED.video_addr;
    }

    // set the border and blank the screen for setup
    TED.bordercolor = BCOLOR_GREEN | CATTR_LUMA4;
    TED.bgcolor = BCOLOR_GREEN | CATTR_LUMA4;
    TED.color1 = BCOLOR_BLACK;
    TED.vscroll &= ~0b00010000;
    TED.hscroll |= 0b00010000;
    
    // Set the location of the bitmap
    TED.misc = (TED.misc & 0b11000011) | (BITMAP_ADDRESS >> 0x0A);
    // Set the location of the luminance and color
    TED.video_addr = (TED.video_addr & 0b00000111) | (LUMINANCE_ADDERSS >> 0x08);

    // Clear all pixels on the screen
    plat_ClearScreen();

    // Pixel masks:
    // GREEN  - %00 for bgcolor ($FF15)
    // WHITE  - %01 for the upper nibble of the color matrix with luminocity (lower nibble)
    // PURPLE - %10 for lower nibble of the color matrix with luminocity (upper nibble)
    // BLACK  - %11 for color1 ($FF16)

    // set the base screen colors
    memset((char*)LUMINANCE_ADDERSS, CATTR_LUMA3 | CATTR_LUMA7>>4, 0x400);
    memset((char*)COLOR_ADDERSS, BCOLOR_WHITE<<4 | BCOLOR_PURPLE, 0x400);

    // Un-blank the screen and turn on HiRes mode
    TED.vscroll |= 0b00110000;
}

/*
 * Back to text mode
 */
void plat_Shutdown()
{
    // restore Plus4 specific hardware values
    TED.bordercolor = t_bcol;
    TED.bgcolor     = t_bgco;
    TED.color1      = t_colo;
    TED.vscroll     = t_vscr;
    TED.hscroll     = t_hscr;
    TED.misc        = t_misc;
    TED.video_addr  = t_vide;
    clrscr();

    // Bank in the ROM
    TED.enable_rom = 1;

    // Switch back to uppercase
    __asm__("lda #142");
    __asm__("jsr $ffd2");
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
    // srand(CIA1.ta_lo);
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
    asm_ClearScreen();
}

/*
 * Draws a null terminated string at y (0..200) and X (a column number) in one of the 
 * 3 styles.  Does not clip at all (so 0..192 for the 8 pixel high font)
 */
void plat_ShowText(uint rawY, uchar colX, uchar style, char* text)
{
    char c;
    uchar index = 0;
    while((c = text[index++]))
    {
        if(c >= 96)
            c -= 152;
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
 * Show the prepared help text for the c64
 */
void plat_ShowHelp()
{
    uchar i, y = 16;

    plat_ClearScreen();
    plat_ShowText(0,7,2,"Gomoku");
    for(i = 0; i < numHelpStrings; i++, y += 8)
    {
        plat_ShowText(y, 0, i < 7 ? 1 : 0, helpStrings[i]);
    }
    plat_ShowText(191, 0, 2, "Any key to continue ");
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
    uchar spacingY = SCREEN_HEIGHT / (m->menuItems->count + 2);
    uchar yTop = (SCREEN_HEIGHT - (spacingY * m->menuItems->count)) / 2 ;
    plat_TimeType now = plat_TimeGet();

    plat_ShowText(yTop-2,(20-strlen(m->Title))/2, 2, m->Title);

    y = yTop;
    for(i =0; i < m->menuItems->count; i++)
    {
        plat_ShowText(y*8, 1, sel == i ? 1 : 0, m->menuItems->MenuItems[i]);
        if(j < m->numChoices)
        {
            if(m->itemChoices[j].item == i)
            {
                plat_ShowText(y*8, 10, 0, m->itemChoices[j].select->MenuItems[m->activeChoice[j]]);
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
                plat_ShowText(y*8, 1, 0, m->menuItems->MenuItems[sel]);
                if(--sel > m->menuItems->count)
                    sel = m->menuItems->count - 1;
                y = yTop + spacingY * sel;
                plat_ShowText(y*8, 1, 1, m->menuItems->MenuItems[sel]);
                break;

            case INPUT_DOWN:
            case INPUT_RIGHT:
                plat_ShowText(y*8, 1, 0, m->menuItems->MenuItems[sel]);
                if(++sel >= m->menuItems->count)
                    sel = 0;
                y = yTop + spacingY * sel;
                plat_ShowText(y*8, 1, 1, m->menuItems->MenuItems[sel]);                    
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
                        plat_ShowText(y*8, 10, 0, m->itemChoices[i].select->MenuItems[m->activeChoice[i]]);
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
    // Draw the corners
    asm_ShowTile(0, 1, tileData[EVEN_BLANK], tileData[ODD_CORNER_UL]);
    asm_ShowTile(0, 2+BOARD_X, tileData[EVEN_LINE_HORIZONTAL_DIV], tileData[ODD_LINE_DOWN_TOP_HALF]);
    asm_ShowTile(1+BOARD_Y, 1        , tileData[EVEN_BLANK]              , tileData[ODD_CORNER_LL]);
    asm_ShowTile(1+BOARD_Y, 2+BOARD_X, tileData[EVEN_LINE_HORIZONTAL_DIV], tileData[ODD_LINE_DOWN_BOTTOM_HALF]);

    // Draw the border lines
    for(move_y = 0; move_y < BOARD_Y; move_y++) {
        asm_ShowTile(0        , 2+move_y , tileData[EVEN_LINE_HORIZONTAL_DIV], tileData[ODD_CORNER_UL]);
        asm_ShowTile(1+move_y , 1        , tileData[EVEN_BLANK]              , tileData[ODD_LEFT_DOWN_HORIZONTAL_DIV]);
        asm_ShowTile(1+move_y , 2+BOARD_Y, tileData[EVEN_LINE_HORIZONTAL_DIV], tileData[ODD_LINE_DOWN]);
        asm_ShowTile(BOARD_Y+1, 2+move_y , tileData[EVEN_LINE_HORIZONTAL_DIV], tileData[ODD_CORNER_LL]);
    }

    // Show the vertical label
    plat_ShowText(172, 2, 2, "ABCDEFGHIJKLMNO");

    // Show the horizontal label, and fill in the middle
    for(move_y = 0; move_y < BOARD_Y; move_y++)
    {
        sprintf(string,"%c",'a'+move_y);
        plat_ShowText((1+move_y)*TILE_HEIGHT, 0, 2, string);
        for(move_x=0; move_x < BOARD_X ; move_x++) {
            asm_ShowTile(1+move_y, 2+move_x, tileData[EVEN_LINE_HORIZONTAL_DIV], tileData[ODD_LEFT_DOWN_HORIZONTAL_DIV]);
        }
    }

    // Show the black piece
    move_x = (BOARD_X/2);
    move_y = (BOARD_Y/2);
    plat_ShowMove();
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
 * Use pre-canned colourd selection tiles to show selector on, and regular tiles to show off
 */
void plat_SelectCursor(uchar y, uchar x, uchar on)
{
    move_y = y;
    move_x = x;
    if(on)
    {
        uchar piece = board[move_y][move_x];
        if(piece)
        {
            if(1 == piece)
                asm_ShowTile(1+move_y, 2+move_x, tileData[SEL_EVEN_BLACK], tileData[SEL_ODD_BLACK]);
            else
                asm_ShowTile(1+move_y, 2+move_x, tileData[SEL_EVEN_WHITE], tileData[SEL_ODD_WHITE]);
        }
        else
        {
            asm_ShowTile(1+move_y, 2+move_x, tileData[SEL_EVEN_BLANK], tileData[SEL_ODD_BLANK]);
        }
    }
    else
    {
        plat_ShowMove();
    }
}

/*
 * Redraw the visible log up to SP
 */
void plat_LogMove()
{
    #define TITLE_ROWS 1
    #define FOOTER_ROWS 4
    uchar color, start, skip, row = TITLE_ROWS;

    if(sp >= (SCREEN_HEIGHT - FOOTER_ROWS - TITLE_ROWS - 1))
    {
        start = sp - (SCREEN_HEIGHT - FOOTER_ROWS - TITLE_ROWS - 1);
        skip = 0;
    }
    else
    {
        start = 0;
        skip = (SCREEN_HEIGHT - FOOTER_ROWS - TITLE_ROWS - 1) - sp;
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
        uchar move = moveStack[start];
        sprintf(string, "%c%c", 'a'+(move >> 4), 'A'+(move & 0x0f));
        color ^= 1;
        plat_ShowText(row*8, 18, 1-color, string);
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
        --winner;
        sprintf(string, "Player %d wins in %3d", winner, sp);
        plat_ShowText(192, 0, winner, string);
    }
    else
    {
        plat_ShowText(192, 0, 2, "    It is a draw    ");
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
    // Hack to get upper and lowercase letters to work
    if(key != 157 && key != 145) 
        key &= 0b01111111;
    switch(key)
    {
        case 87:        // w
        case 145:       // Up
            keyMask |= INPUT_UP;
        break;

        case 68:        //d
        case 29:        // Right
            keyMask |= INPUT_RIGHT;
        break;

        case 83:        // s
        case 17:        // Down
            keyMask |= INPUT_DOWN;
        break;

        case 65:        // a
        case 157:       // Left
            keyMask |= INPUT_LEFT;
        break;
        
        case 3:         // Run Stop
            keyMask |= INPUT_ESCAPE;
        break;

        case 32:        // Space
        case 13:        // Enter
            keyMask |= INPUT_SELECT;
        break;
        
        case 82:        // u
            keyMask |= INPUT_REDO;
        break;
        
        case 85:        // r
            keyMask |= INPUT_UNDO;
        break;
    }
    
    return keyMask;
}

