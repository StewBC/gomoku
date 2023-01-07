/*---------------------------------------------------------------------------*\
  Gomoku By Stefam Wessels, 2023
  This is free and unencumbered software released into the public domain.
\*---------------------------------------------------------------------------*/

#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "../globals.h"
#include "../plat.h"

#define SCREEN_HEIGHT_GFX   192         /* Pixels vertical */
#define COLOR_WIDTH         140         /* Pixels horizontal (2 bpp) */
#define SCREEN_HEIGHT       31
#define SCREEN_WIDTH        80

const char show[3] = {' ','B','W'};
char string[40];
plat_TimeType current_time = 0, current_ticks = 0, time_scale = 800;
WORD seconds = 0;
extern uchar scoreBoard[BOARD_Y][BOARD_X];
const wchar_t* ansiClear = L"\x1b[2J\x1b[H";
const wchar_t* ansiColors[] = { L"\x1b[37m", L"\x1b[34m", L"\x1b[35m", L"\x1b[36m" };
enum ANSI_COLORS { ANSI_WHITE, ANSI_BLUE, ANSI_MAGENTA, ANSI_CYAN };

/*
 * Could set the console window size, but does nothing
 */
void plat_Init()
{
}

/*
 * Does nothing on Windows
 */
void plat_Shutdown()
{
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
    srand(time(NULL));
}

/*
 * Platform independent way to get a platform specific random number
 */
uchar plat_Random(uchar max)
{
    return rand() % max;
}

/*
 * Temporarily turns on ENABLE_VIRTUAL_TERMINAL_PROCESSING so that ansi control codes do something
 * then writes (issues) the parameter ansi control code and restores the old mode.
 */
void plat_ConsoleAnsi(const wchar_t* ansiString)
{
    HANDLE hStdOut;

    hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);

    // Fetch existing console mode so we correctly add a flag and not turn off others
    DWORD mode = 0;
    if (!GetConsoleMode(hStdOut, &mode))
    {
        return;
    }

    // Hold original mode to restore on exit to be cooperative with other command-line apps.
    const DWORD originalMode = mode;
    mode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;

    // Try to set the mode.
    if (!SetConsoleMode(hStdOut, mode))
    {
        return;
    }

    // Write the ansi
    DWORD written = 0;
    if (!WriteConsoleW(hStdOut, ansiString, (DWORD)wcslen(ansiString), &written, NULL))
    {
        // If we fail, try to restore the mode on the way out.
        SetConsoleMode(hStdOut, originalMode);
        return ;
    }

    // To also clear the scroll back, emit L"\x1b[3J" as well.
    // 2J only clears the visible window and 3J only clears the scroll back.

    // Restore the mode on the way out to be nice to other command-line applications.
    SetConsoleMode(hStdOut, originalMode);
}

/*
 * Uses an ASCII code string to clear the screen (Doesn't work with CMD but does with PoweShell)
 */
void plat_ClearScreen()
{
    plat_ConsoleAnsi(ansiClear);
}

/*
 * Draws a null terminated string at y which is a pixel value and X (a column number) in one of the 
 * 4 styles. 
 */
void plat_ShowText(uint rawY, uchar colX, uchar style, char* text)
{
    HANDLE h = GetStdHandle( STD_OUTPUT_HANDLE );
    COORD cursor;
    cursor.Y = rawY/8;
    cursor.X = colX;
    SetConsoleCursorPosition(h, cursor);
    plat_ConsoleAnsi(ansiColors[style]);
    printf("%s",text);
}

/*
 * Print the title screen, and wait for a key or timeout
 * if timeout return -1, ESC return ESC else return 0
 */
uchar plat_ShowSplash()
{
    int key;
    plat_TimeType start = plat_TimeGet();

    plat_ClearScreen();
    plat_ShowText( 0,18,3," _______                   _           ");
    plat_ShowText( 8,18,3,"(_______)                 | |          ");
    plat_ShowText(16,18,3," _   ___  ___  ____   ___ | |  _ _   _ ");
    plat_ShowText(24,18,3,"| | (_  |/ _ \\|    \\ / _ \\| |_/ ) | | |");
    plat_ShowText(32,18,3,"| |___) | |_| | | | | |_| |  _ (| |_| |");
    plat_ShowText(40,18,3," \\_____/ \\___/|_|_|_|\\___/|_| \\_)____/ ");
    plat_ShowText(56,18,3,"         By Stefan Wessels, 2023");
    
    while(1)
    {
        plat_TimeTick();
        if(plat_TimePassed(start, 15))
            return 1;

        if(plat_CheckKey())
        {
            plat_ClearScreen();
            key = plat_ReadKeys(0);
            if(key == INPUT_ESCAPE)
                return key;

            break;
        }
    }
    return 0;
}

/*
 * Slightly more complicated ShowHelp because the strings are platform independent so
 * format for a 80 col screen by finding wrap points in the strings
 * Since copies need to be made for MSVC, this could be done far more efficiently
 */
void plat_ShowHelp()
{
    uchar i, s, b, e, c, y = 16;

    // MSVC will give a write error when touching globally declared char *[] values, so make copies
    char** nonconsthelp = (char**)malloc(sizeof(char*) * numHelpStrings);
    for (i = 0; i < numHelpStrings; i++)
        nonconsthelp[i] = strdup(helpStrings[i]);

    plat_ClearScreen();
    plat_ShowText(0,7,3,"Gomoku");
    for(i = 0; i < numHelpStrings; i++)
    {
        s = b = e = 0; // start, break and end
        do
        {
            c = nonconsthelp[i][e];
            // search for a possible line break
            while(c && c != ' ')
            {
                c = nonconsthelp[i][++e];
            }

            // if not eol and line fits, mark as possible break
            if(c && e - s <= SCREEN_WIDTH)
            {
                b = e++;
            }
            else
            {
                // eol or line doesn't fit.  if eol and fits, put "break" at eol, else create break
                if(!c && e - s <= SCREEN_WIDTH)
                    b = e;
                else
                    nonconsthelp[i][b] = '\0';
                // show this portion of the help
                plat_ShowText(y, 0, 0, &nonconsthelp[i][s]);
                y += 8;
                // if not eol, restore the space
                if(c)
                {
                    nonconsthelp[i][b] = ' ';
                }
                else if(b != e)
                {
                    // if eol and eol past last break, show the remainder of the text
                    plat_ShowText(y, 0, 0, &nonconsthelp[i][b+1]);
                    y += 8;
                }
                // start and end of new line right after the last break
                s = e = ++b;
            }
            // Repeat while not eol
        } while(c);
    }
    plat_ShowText(183, 0, 1, "Any key to continue");
    plat_ReadKeys(60);

    for (i = 0; i < numHelpStrings; i++)
        free(nonconsthelp[i]);
    free(nonconsthelp);

    plat_ClearScreen();
}

/*
 * Show and process a menu until a non ItemChoices item is selected, or ESC is pressed, or
 * the menu is idle for timeout (seconds)
 */
uchar plat_Menu(Menu* m, uchar timeout)
{
    uchar i, y, sel = 0, j = 0;
    uchar spacingY = (SCREEN_HEIGHT / (m->menuItems->count + 4)) ;
    uchar yTop = ((SCREEN_HEIGHT - (spacingY * m->menuItems->count)) / 2);
    plat_TimeType now = plat_TimeGet();

    plat_ShowText(4,(20-strlen(m->Title))/2, 0, m->Title);

    y = yTop;
    for(i =0; i < m->menuItems->count; i++)
    {
        plat_ShowText(y*8, 1, sel == i ? 3 : 0, m->menuItems->MenuItems[i]);
        if(j < m->numChoices)
        {
            if(m->itemChoices[j].item == i)
            {
                plat_ShowText(y*8, 10, 1, m->itemChoices[j].select->MenuItems[m->activeChoice[j]]);
                j++;
            }
        }
        y += spacingY;
    }

    plat_ShowText(yTop*8, 1+strlen(m->menuItems->MenuItems[0]), 1, "");
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
                plat_ShowText(y*8, 1, 3, m->menuItems->MenuItems[sel]);
                break;

            case INPUT_DOWN:
            case INPUT_RIGHT:
                plat_ShowText(y*8, 1, 0, m->menuItems->MenuItems[sel]);
                if(++sel >= m->menuItems->count)
                    sel = 0;
                y = yTop + spacingY * sel;
                plat_ShowText(y*8, 1, 3, m->menuItems->MenuItems[sel]);                    
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
                        plat_ShowText(y*8, 10, 1, m->itemChoices[i].select->MenuItems[m->activeChoice[i]]);
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
    uchar x, y;
    HANDLE h = GetStdHandle( STD_OUTPUT_HANDLE );
    COORD cursor;
    cursor.X = 0;
    cursor.Y = 0;
    SetConsoleCursorPosition(h, cursor);

    plat_ConsoleAnsi(ansiColors[ANSI_WHITE]);
    printf("    ");
    for(x=0; x < BOARD_X; x++)
        printf("%c ", 'A'+x);

    printf("    ");
    for(x=0; x < BOARD_X; x++)
        printf("%c  ", 'A'+x);

    plat_ConsoleAnsi(ansiColors[ANSI_MAGENTA]);
    printf("\n   |");
    for(x=0; x < BOARD_X-1; x++)
        printf("-+");

    printf("-|   |");
    for(x=0; x < BOARD_X-1; x++)
        printf("--+");
    printf("--|\n");

    for(y=0; y < BOARD_Y; y++)
    {
        plat_ConsoleAnsi(ansiColors[ANSI_WHITE]);
        printf(" %c ",'a'+y);
        plat_ConsoleAnsi(ansiColors[ANSI_MAGENTA]);
        printf("|");
        for(x=0; x < BOARD_X; x++)
        {
            if(board[y][x] == 2)
                plat_ConsoleAnsi(ansiColors[ANSI_WHITE]);
            else
                plat_ConsoleAnsi(ansiColors[ANSI_BLUE]);
            printf("%c", show[board[y][x]]);
            plat_ConsoleAnsi(ansiColors[ANSI_MAGENTA]);
            printf("|");
        }
        printf("   |");
        for(x=0; x < BOARD_X; x++)
        {
            plat_ConsoleAnsi(ansiColors[ANSI_WHITE]);
            printf("%2X", scoreBoard[y][x]);
            plat_ConsoleAnsi(ansiColors[ANSI_MAGENTA]);
            printf("|");
        }
        printf("\n   |");
        for(x=0; x < BOARD_X-1; x++)
            printf("-+");
        printf("-|   |");
        for(x=0; x < BOARD_X-1; x++)
            printf("--+");
        printf("--|\n");
    }
}

/*
 * In windows, just redraw the whole board every time
 */
void plat_ShowMove()
{
    plat_ShowBoard();
}

/*
 * Sets the cursor to row (y) / col (x) and ignores the on parameter
 */
void plat_SelectCursor(uchar y, uchar x, uchar on)
{
    HANDLE h = GetStdHandle( STD_OUTPUT_HANDLE );
    COORD cursor;
    cursor.Y = (y + 1) * 2;
    cursor.X = (x + 2) * 2;
    SetConsoleCursorPosition(h, cursor);
}

/*
 * Redraw the visible log up to SP
 */
void plat_LogMove()
{
    #define TITLE_ROWS 2
    #define LOG_X   84
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
        plat_ShowText(row*8, LOG_X, 0, "   ");
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
        plat_ShowText(row*8, LOG_X, color, string);
        row++;
        start++;
    }
}

/*
 * Show the winner and wait for a key/timeout
 */
void plat_Winner(uchar winner)
{
    uchar* blank = "                      ";

    if(winner)
    {
        sprintf(string, " Player %d wins in %3d ", winner, sp);
        plat_ShowText(14*8, 28, 1-(winner-1), blank);
        plat_ShowText(15*8, 28, 1-(winner-1), string);
        plat_ShowText(16*8, 28, 1-(winner-1), blank);
    }
    else
    {
        plat_ShowText(14*8, 28, 1-(winner-1), blank);
        plat_ShowText(15*8, 28, ANSI_CYAN, "     It is a draw     ");
        plat_ShowText(16*8, 28, 1-(winner-1), blank);
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
    SYSTEMTIME syst;
    GetSystemTime(&syst);
    if(syst.wSecond != seconds)
    {
        current_time++;
        seconds = syst.wSecond;
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
    INPUT_RECORD irec;
    DWORD n = 1;
    HANDLE h = GetStdHandle(STD_INPUT_HANDLE);

    if (h == NULL)
        return 0; // console not found

    while(n)
    {
        PeekConsoleInput(h, &irec, 1, &n);
        if(n)
        {
            if(irec.EventType == KEY_EVENT && irec.Event.KeyEvent.bKeyDown)
                return 1;
            else
                ReadConsoleInput(h, &irec, 1, &n);
        }
    }
    
    return 0;
}

/*
 * Windows Console get a key, with timeout ability
 */
int getconchar(KEY_EVENT_RECORD* krec, uchar timeout)
{
    DWORD n;
    INPUT_RECORD irec;
    HANDLE h = GetStdHandle(STD_INPUT_HANDLE);
    plat_TimeType start = plat_TimeGet();

    if (h == NULL)
        return 0; // console not found

    while(1)
    {
        plat_TimeTick();
        if(timeout && plat_TimePassed(start, timeout))
            return -1;
        if(!timeout || plat_CheckKey())
        {
            ReadConsoleInput(h, &irec, 1, &n);
            if(irec.EventType == KEY_EVENT && irec.Event.KeyEvent.bKeyDown)
            {
                *krec= irec.Event.KeyEvent;
                return 1;
            }
        }
    }
    return 0;
}

/*
 * Get a key or time out (return -1) and translate that key into a key mask bit setting
 */
int plat_ReadKeys(uchar timeout)
{
    int keyMask = 0;
    KEY_EVENT_RECORD key;

    if(-1 == getconchar(&key, timeout))
        return -1;
    switch(key.wVirtualKeyCode)
    {
        case 'O':
        case 'o':       // Reasonably located on the ][ keyboard
        case 37:
            keyMask |= INPUT_LEFT;
        break;

        case 'P':
        case 'p':       // Reasonably located on the ][ keyboard
        case 39:
            keyMask |= INPUT_RIGHT;
        break;

        case 'Q':
        case 'q':
        case 38:
            keyMask |= INPUT_UP;
        break;

        case 'A':
        case 'a':
        case 40:
            keyMask |= INPUT_DOWN;
        break;

        case 27:
            keyMask |= INPUT_ESCAPE;
        break;

        case 13:
        case ' ':
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
