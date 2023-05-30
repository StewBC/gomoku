/*---------------------------------------------------------------------------*\
  Gomoku By Stefam Wessels, 2023
  This is free and unencumbered software released into the public domain.
\*---------------------------------------------------------------------------*/

#include <string.h>
#include "globals.h"
#include "plat.h"

uchar board[BOARD_Y][BOARD_X];      // The visible board
uchar scoreBoard[BOARD_Y][BOARD_X]; // The AI scoring board
uchar playerType[2];                // 0 = Human, 1 = easy, 2 = med, 3 = hard AI
uchar moveStack[STACK_SIZE];        // Undo / Redo Stack for board
uchar scoreStack[STACK_SIZE];       // Undo / Redo stack for scoreBoard
uchar sp;                           // index into moveStack, increases as moves are made
uchar spMax;                        // max index in moveStack (top for redo)
schar player;                       // 0 or 1 take turns
schar move_y;                       // Where a piece is placed, undo/redo happens, Y (row)
schar move_x;                       // X (column)
schar winner;                       // 1 when five-in-a-row.
uchar exitApp;                      // 1 for quit back to OS
uchar quitGame;                     // 1 for quit the current board
uchar demoMode;                     // 1 when splash does an ai/ai game
uchar uy;                           // user cursor y
uchar ux;                           // user cursor x
uchar AI_LOCATION[3];               // Move for 3 skill levels
uchar AI_LOCATION_SCORE[3];         // Score at the slill level move locations

// The number of pieces in a row index into this for a score
const uchar lineScore[] =  { 0,1,3,7,23,0,0,0,0,0,0,0,0,0,0,0 };
// If a line ends in a blank, that's worth some extra score
const uchar blankScore[] = { 0,1,2,8,32,0,0,0,0,0,0,0,0,0,0,0 };

// These are for the GetLineLength function (or helpers to track) so it doesn't take parameters
uchar lineLength, piece, nextPiece, y, y1, dy, x, x1, dx;

// Main Menu
MenuItems mainItems = { 5, "Play", "Player1", "Player2", "Help", "Quit" };
MenuItems skills = { 4, "Human    ", "AI Easy  ", "AI Medium", "AI Hard  " };
ItemChoices mainItemsOptions[] = { {1, &skills}, {2, &skills} };
Menu mainMenu = { "Gomoku", &mainItems, 2, mainItemsOptions, {3,0} };

/*
 * Show the main menu and returns only when user has decided to play(1) or quit(0),
 * or the menu times out(0) back to the Splash screen
 */
uchar MainMenu()
{
    uchar item;

    while (1)
    {
        plat_ClearScreen();

        // displaying and processing the menu is platform specific
        if (254 <= (item = plat_Menu(&mainMenu, 45)))
            return 0;

        if (3 == item)
            plat_ShowHelp();

        // Option 4 is Quit
        if (4 == item)
        {
            exitApp = 1;
            return 0;
        }

        // Set the playerType to whatever is selected in the Menu
        playerType[0] = mainMenu.activeChoice[0];
        playerType[1] = mainMenu.activeChoice[1];

        // !item means option 1 which is play
        if (!item)
            break;
    }

    // return 1 to play
    return 1;
}

/*
 * Select a move from each of the highest, second and third highest scoring tiles
 */
void TestForAISCore(uchar score)
{
    // If score is >= the highest, replace or consider replacing with this move
    // and do the same for the second and third hihest scores
    // If a score is higher, the selected scores get moved down a notch
    if (score >= AI_LOCATION_SCORE[0])
    {
        if(score == AI_LOCATION_SCORE[0] && plat_Random(100) >= 50)
            return;
        AI_LOCATION_SCORE[2] = AI_LOCATION_SCORE[1];
        AI_LOCATION_SCORE[1] = AI_LOCATION_SCORE[0];
        AI_LOCATION_SCORE[0] = score;
        AI_LOCATION[2] = AI_LOCATION[1];
        AI_LOCATION[1] = AI_LOCATION[0];
        AI_LOCATION[0] = (y << 4) | x;
    }
    else if (score >= AI_LOCATION_SCORE[1])
    {
        if (score == AI_LOCATION_SCORE[1] && plat_Random(100) >= 50)
            return;
        AI_LOCATION_SCORE[2] = AI_LOCATION_SCORE[1];
        AI_LOCATION_SCORE[1] = score;
        AI_LOCATION[2] = AI_LOCATION[1];
        AI_LOCATION[1] = (y << 4) | x;
    }
    else if (score >= AI_LOCATION_SCORE[2])
    {
        if (score == AI_LOCATION_SCORE[2] && plat_Random(100) >= 50)
            return;
        AI_LOCATION_SCORE[2] = score;
        AI_LOCATION[2] = (y << 4) | x;
    }
}

/*
 * Starts at board[y][x] (piece) and "walks" in the dy/dx direction counting pieces
 * matching the same color as piece.  Stops on blank, other color of leaving the board
 */
void GetLineLength()
{
    // At the edge of the board, y/x could already be off the board
    if (!(y < BOARD_Y && x < BOARD_X))
    {
        piece = 0;
        nextPiece = 3;  // dummy value
        return;
    }

    nextPiece = piece = board[y][x];
    // Only interested in counting pieces, not spaces
    if(piece)
    {
        do
        {
            y += dy;
            x += dx;
            lineLength++;

            // make sure to stay on the board
            if(y < BOARD_Y && x < BOARD_X)
            {
                nextPiece = board[y][x];
            }
            else
            {
                break;
            }
        // keep going while the pieces are the same color, ie making a line
        } while(piece == nextPiece);
    }
}

/*
 * Calculates a score for lines that go through board[move_y][move_x] and have
 * spaces at their other end (scores those spaces)
 */
void Calculate()
{
    uchar outLine, color;

    // Imagine playing X, where Y and V are the same color as X, and Z is a blank
    // in this config XYYZV with dx = 1 and dy = 0

    // step from X (X is at move_y/move_X)
    y = move_y + dy;
    x = move_x + dx;
    // get the length from X to Z
    lineLength = 0;
    GetLineLength();
    // If it ends not in a space it is a different color, or off the board,
    // so no space to update, but if it is a space, update it
    if (!nextPiece)
    {
        // Remember the length and spot (Z)
        outLine = lineLength;
        y1 = y;
        x1 = x;
        // The color of the neighbour - first piece in the line
        color = piece ? piece : player + 1;

        // Check if this spot where it ended splits a color line by stepping one more (to V)
        y += dy;
        x += dx;

        // If still on the board, and still the same color, get the score for that "row"
        // as it it was adjacent (so score V). Z is more valuable because of V, consider that
        if (y < BOARD_Y && x < BOARD_X && color == board[y][x])
        {
            GetLineLength();
        }

        // Fix this spot on the score board (Z is/was thus score as though V is, where Z is)
        scoreBoard[y1][x1] -= lineScore[lineLength] + (nextPiece ? 0 : blankScore[lineLength]);

        // Get the length of the V portion alone, and remember it
        lineLength -= outLine;
        outLine = lineLength;

        // Turn back to the piece that was played (X)
        dy = -dy;
        dx = -dx;
        // and step, from Z to the adjacent Y
        y = y1 + dy;
        x = x1 + dx;
        // Score the line back over X
        GetLineLength();
        // and that's the score at Z. If a tile on the line, past X is blank, add a blank score
        scoreBoard[y1][x1] += lineScore[lineLength] + (nextPiece ? 0 : blankScore[lineLength]);
        // Get the actual line length from Z to X, without the V length
        lineLength -= outLine;
    }
    else if(piece == player+1)
    {
        // If the line is the same color as the piece played, make sure this wasn't perhaps a winning move
        // so add the piece and check the line to the other side of the piece as well, and if
        // it's the same color, count that segment as well
        lineLength++;
        dy = -dy;
        dx = -dx;
        y = move_y + dy;
        x = move_x + dx;
        if(player + 1 == board[y][x])
            GetLineLength();
    }
    // If that is 5, this move was a winner
    if (5 == lineLength)
        winner = 1;
}

/*
 * Scores all the lines that start adjacent to the square being played
 */
void UpdateNeighbours()
{
    dy = -1; dx = -1; Calculate();
    dy = -1; dx =  0; Calculate();
    dy = -1; dx =  1; Calculate();
    dy =  0; dx =  1; Calculate();
    dy =  1; dx =  1; Calculate();
    dy =  1; dx =  0; Calculate();
    dy =  1; dx = -1; Calculate();
    dy =  0; dx = -1; Calculate();
}

/*
 * Places a piece in a blank at board[move_y][move_x] and updates the scores
 * for lines through that square
 */
void MakeMove()
{
    scoreStack[sp] = scoreBoard[move_y][move_x];
    board[move_y][move_x] = player + 1;
    scoreBoard[move_y][move_x] = 0;
    UpdateNeighbours();
    moveStack[sp++] = (move_y << 4) | move_x;
    if(spMax < sp)
        spMax = sp;
}

/*
 * Reverses the scores that would have been calculated by Calculate, returning the
 * board to a state, for the line (dy/dx) that ends with the piece being removed
 * Also adds the score to the new blank
 */
void UndoUpdateLine()
{
    uchar outLine, color;

    // Imagine undoing X, where Y and V are the same color as X, and Z is a blank
    // in this config YXYZV with dx = 1 and dy = 0

    // step from X towards Z (X is at move_y/move_X)
    y = move_y + dy;
    x = move_x + dx;
    // get the length from X to Z
    lineLength = 0;
    GetLineLength();

    // The score where X is removed doesn't get this score, because if there's a neighbour
    // across (ie leftmost Y in example), this score would be incorrect at this point
    // Use the scoreStack instead to avoid having to undo a more complicated mechanism

    // If it ends not in a space it is a different color, or off the board,
    // so end there (not the example scenario)
    if (nextPiece)
        return;

    // Remember the length and spot (Z)
    outLine = lineLength;
    y1 = y;
    x1 = x;
    // Color of neighbour also matters (1st piece in the row)
    color = piece ? piece : player + 1;

    // Check if this spot where it ended splits a color line by stepping one more (to V)
    y += dy;
    x += dx;

    // If still on the board, and still the same color, get the score for that "row"
    // as it it was adjacent (so score V)
    if (y < BOARD_Y && x < BOARD_X && color == board[y][x])
        GetLineLength();

    // Add the blank as well, if there is one
    scoreBoard[y1][x1] += lineScore[lineLength] + (nextPiece ? 0 : blankScore[lineLength]);

    // Start only with the length of the V side line
    lineLength -= outLine;

    // Turn back to the piece that was played (X)
    dy = -dy;
    dx = -dx;
    // and step, from Z to the adjacent Y
    y = y1 + dy;
    x = x1 + dx;
    // Score the line back through X to the other Y
    GetLineLength();
    // and that's the old score to remove at Z
    scoreBoard[y1][x1] -= lineScore[lineLength] + (nextPiece ? 0 : blankScore[lineLength]);
}

/*
 * Undo the lines through board[move_y][move_x] when that square is having its piece
 * removed
 */
void UndoUpdate()
{
    dy = -1; dx = -1; UndoUpdateLine();
    dy = -1; dx =  0; UndoUpdateLine();
    dy = -1; dx =  1; UndoUpdateLine();
    dy =  0; dx =  1; UndoUpdateLine();
    dy =  1; dx =  1; UndoUpdateLine();
    dy =  1; dx =  0; UndoUpdateLine();
    dy =  1; dx = -1; UndoUpdateLine();
    dy =  0; dx = -1; UndoUpdateLine();
}

/*
 * Retrieves the last move from the undo moveStack, and removes it from the board.  Switches
 * to the other player and redraws the log
 */
void UndoMove()
{
    if(sp > 1)
    {
        // Get the last (previous move)
        --sp;
        move_y = moveStack[sp] >> 4;
        move_x = moveStack[sp] & 0x0f;
        // Flip the player back to the player that played this move
        player ^= 1;
        // Update the scores with the piece gone
        // all lines going through this piece and also
        // the scores for this piece
        UndoUpdate();
        // Finally remove the piece from the board, show changes and switch to other player
        board[move_y][move_x] = 0;
        scoreBoard[move_y][move_x] = scoreStack[sp];
        plat_ShowMove();
        plat_LogMove();
    }
}

/*
 * Playes the next move in the undo/redo moveStack, redraws the log and switches to the other player
 */
void RedoMove()
{
    if(sp < spMax)
    {
        // extract the "current" move
        move_y = moveStack[sp] >> 4;
        move_x = moveStack[sp] & 0x0f;
        // Make the move (this advances sp to the "next" move)
        MakeMove();
        plat_ShowMove();
        plat_LogMove();
        player ^= 1;
    }
}

/*
 * The AI runs this to pick its next move.  Scans the whole board and selects 
 * one of the highest, second and third highest scores and then picks one of 
 * those at random, based on skill level.  That can make easy very, very easy
 * and even Medium can do some pretty silly things
 */
void Pick()
{
    uchar choice;
    AI_LOCATION_SCORE[0] = AI_LOCATION_SCORE[1] = AI_LOCATION_SCORE[2] = 0;
    for(y = 0; y < BOARD_Y; y++)
    {
        for(x = 0; x < BOARD_X ; x++)
        {
            uchar score = scoreBoard[y][x];
            if(score)
                TestForAISCore(score);
        }
    }
    choice = plat_Random(4) % (4 - playerType[player]);
    move_y = AI_LOCATION[choice] >> 4;
    move_x = AI_LOCATION[choice] & 0x0f;
}

/*
 * Get a move from the user.  User controls a cursor to pick a spot.  User can also 
 * quit, or undo/redo.  Calls MakeMove on the user selected spot and updates the log
 */
uchar GetMove(uchar AI)
{
    int keyMask, exitWhile = 0;

    if(AI)
    {
        Pick();
        if(playerType[1-player])
        {
            if(plat_CheckKey() && INPUT_ESCAPE == plat_ReadKeys(0))
                playerType[1-player] = 0;
        }
    }
    else
    {
        plat_SelectCursor(uy, ux, 1);
        while(!exitWhile)
        {
            keyMask = plat_ReadKeys(0);
            switch(keyMask)
            {
                case INPUT_UP:
                    plat_SelectCursor(uy, ux, 0);
                    if(--uy > BOARD_Y)
                        uy = BOARD_Y-1;
                    plat_SelectCursor(uy, ux, 1);
                    break;

                case INPUT_RIGHT:
                    plat_SelectCursor(uy, ux, 0);
                    if(++ux >= BOARD_X)
                        ux = 0;
                    plat_SelectCursor(uy, ux, 1);
                    break;

                case INPUT_DOWN:
                    plat_SelectCursor(uy, ux, 0);
                    if(++uy >= BOARD_Y)
                        uy = 0;
                    plat_SelectCursor(uy, ux, 1);
                    break;

                case INPUT_LEFT:
                    plat_SelectCursor(uy, ux, 0);
                    if(--ux > BOARD_X)
                        ux = BOARD_X-1;
                    plat_SelectCursor(uy, ux, 1);
                    break;

                case INPUT_SELECT:
                    if(!board[uy][ux])
                    {
                        spMax = sp;
                        exitWhile = 1;
                    }
                    break;

                case INPUT_UNDO:
                    plat_SelectCursor(uy, ux, 0);
                    UndoMove();
                    if (playerType[player])
                        UndoMove();
                    plat_SelectCursor(uy, ux, 1);
                    break;

                case INPUT_REDO:
                    plat_SelectCursor(uy, ux, 0);
                    RedoMove();
                    if (playerType[player])
                        RedoMove();
                    plat_SelectCursor(uy, ux, 1);
                    break;

                case INPUT_ESCAPE:
                    exitWhile = 1;
                    quitGame = 1;
                    break;
            }
        }
        plat_SelectCursor(uy, ux, 0);
        move_y = uy;
        move_x = ux;
    }
    MakeMove();
    plat_ShowMove();
    plat_LogMove();
    return 0;
}

/*
 * Show the splash and start a demo on a timeout. Goes to a menu on a key, from splash.
 * Menu quits back to splash (user or timeout) or goes to a game.  Splash can quit to OS
 */
void main()
{
    plat_Init();
    if (!plat_CanQuit())
        --mainItems.count;

    while(!exitApp)
    {
        uchar demoMode = plat_ShowSplash();
        if(demoMode == INPUT_ESCAPE && plat_CanQuit())
            break;

        while(1)
        {
            plat_SeedRandom();
            memset(board, 0, BOARD_Y*BOARD_X*sizeof(uchar));
            memset(scoreBoard, 0, BOARD_Y*BOARD_X*sizeof(uchar));
            memset(moveStack, 0, STACK_SIZE*sizeof(uchar));
            sp = spMax = player = quitGame = winner = 0;
            ux = move_x = (BOARD_X/2);
            uy = move_y = (BOARD_Y/2);
            MakeMove();

            if(!demoMode)
            {
                // MainMenu return 0 means timeout or ESC
                if(!MainMenu())
                    break;
            }
            else
            {
                // demoMode pits 2 random skilled AIs against one-another
                playerType[0] = plat_Random(3) + 1;
                playerType[1] = plat_Random(3) + 1;
            }

            plat_ShowBoard();
            plat_LogMove();
            do
            {
                // Stop if all spots filled or user quits (ESC)
                if(BOARD_X * BOARD_Y == sp || quitGame)
                    break;

                // Switch Players
                player ^= 1;
                // Get the move from this player
                GetMove(playerType[player]);
                // Go till a winner or the user quits
            } while(!quitGame && !winner);
            
            // If the user didn't quit then there's a winner or a draw
            if (!quitGame)
            {
                plat_Winner(sp == BOARD_X * BOARD_Y ? 0 : player + 1);
                plat_ReadKeys(demoMode ? 5 : 60);
            }

            // demo mode goes back to the splash screen            
            if(demoMode)
                break;
        }
    }
    plat_Shutdown();
}


