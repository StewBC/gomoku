/*---------------------------------------------------------------------------*\
  Gomoku By Stefam Wessels, 2023
  This is free and unencumbered software released into the public domain.
\*---------------------------------------------------------------------------*/

#ifndef _DATA_H_
#define _DATA_H_

enum tileTypes_
{
    EVEN_BLANK,
    ODD_CORNER_UL,
    ODD_CORNER_LL,
    ODD_LINE_DOWN,
    ODD_LINE_DOWN_TOP_HALF,
    ODD_LINE_DOWN_BOTTOM_HALF,
    ODD_LEFT_DOWN_HORIZONTAL_DIV,
    EVEN_LINE_HORIZONTAL_DIV,
    EVEN_BLACK,
    ODD_BLACK,
    EVEN_WHITE,
    ODD_WHITE,
    NUM_TILES
} ;

#define TILE_HEIGHT 11
extern const char tileData[NUM_TILES][TILE_HEIGHT];
extern const char fontData[][16];

#endif //_DATA_H_