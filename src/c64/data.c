/*---------------------------------------------------------------------------*\
  Gomoku By Stefam Wessels, 2023
  This is free and unencumbered software released into the public domain.
\*---------------------------------------------------------------------------*/

#include "data.h"

/*
 * The pieces that make up the board (edges, lines pebbles, etc)
 */
const char tileData[NUM_TILES][TILE_HEIGHT] = 
{
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},   // EVEN_BLANK
    {0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xC0, 0xC0, 0xC0, 0xC0},   // ODD_CORNER_UL
    {0xC0, 0xC0, 0xC0, 0xC0, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00},   // ODD_CORNER_LL
    {0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0},   // ODD_LINE_DOWN
    {0x00, 0x00, 0x00, 0x00, 0x00, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0},   // ODD_LINE_DOWN_TOP_HALF
    {0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x00},   // ODD_LINE_DOWN_BOTTOM_HALF
    {0xC0, 0xC0, 0xC0, 0xC0, 0xFF, 0xFF, 0xC0, 0xC0, 0xC0, 0xC0},   // ODD_LEFT_DOWN_HORIZONTAL_DIV
    {0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00},   // EVEN_LINE_HORIZONTAL_DIV
    {0x00, 0x03, 0x0F, 0x0F, 0xFF, 0xFF, 0x0F, 0x0F, 0x03, 0x00},   // EVEN_BLACK
    {0xC0, 0xC0, 0xF0, 0xF0, 0xFF, 0xFF, 0xF0, 0xF0, 0xC0, 0xC0},   // ODD_BLACK
    {0x00, 0x01, 0x05, 0x05, 0xD5, 0xD5, 0x05, 0x05, 0x01, 0x00},   // EVEN_WHITE
    {0xC0, 0x40, 0x50, 0x50, 0x57, 0x57, 0x50, 0x50, 0x40, 0xC0},   // ODD_WHITE
    {0xAA, 0xAB, 0xAF, 0xAF, 0xFF, 0xFF, 0xAF, 0xAF, 0xAB, 0xAA},   // SEL_EVEN_BLACK
    {0xAA, 0xEA, 0xFA, 0xFA, 0xFF, 0xFF, 0xFA, 0xFA, 0xEA, 0xAA},   // SEL_ODD_BLACK
    {0xAA, 0xA9, 0xA5, 0xA5, 0xD5, 0xD5, 0xA5, 0xA5, 0xA9, 0xAA},   // SEL_EVEN_WHITE
    {0xEA, 0x6A, 0x5A, 0x5A, 0x57, 0x57, 0x5A, 0x5A, 0x6A, 0xEA},   // SEL_ODD_WHITE
    {0xAA, 0xAA, 0xAA, 0xAA, 0xFF, 0xFF, 0xAA, 0xAA, 0xAA, 0xAA},   // SEL_EVEN_BLANK
    {0xEA, 0xEA, 0xEA, 0xEA, 0xFF, 0xFF, 0xEA, 0xEA, 0xEA, 0xEA}    // SEL_ODD_BLANK
};

/*
 * 2 bytes per row, 8 rows per character
 */
const char fontData[][16] =
{
    {
        0x00, 0x00, /*                  */
        0x00, 0x00, /*                  */
        0x00, 0x00, /*                  */
        0x00, 0x00, /*                  */
        0x00, 0x00, /*                  */
        0x00, 0x00, /*                  */
        0x00, 0x00, /*                  */
        0x00, 0x00, /*                  */
    },
    {
        0x00, 0x00, /*                  */
        0x00, 0x00, /*                  */
        0x00, 0x00, /*                  */
        0x00, 0x00, /*                  */
        0x00, 0x00, /*                  */
        0x03, 0xC0, /*       XXXX       */
        0x03, 0xC0, /*       XXXX       */
        0x0F, 0x00, /*     XXXX         */
    },
    {
        0x00, 0x00, /*                  */
        0x00, 0x00, /*                  */
        0x00, 0x00, /*                  */
        0x3F, 0xFC, /*   XXXXXXXXXXXX   */
        0x00, 0x00, /*                  */
        0x00, 0x00, /*                  */
        0x00, 0x00, /*                  */
        0x00, 0x00, /*                  */
    },
    {
        0x00, 0x00, /*                  */
        0x00, 0x00, /*                  */
        0x00, 0x00, /*                  */
        0x00, 0x00, /*                  */
        0x00, 0x00, /*                  */
        0x03, 0xC0, /*       XXXX       */
        0x03, 0xC0, /*       XXXX       */
        0x00, 0x00, /*                  */
    },
    {
        0x00, 0x00, /*                  */
        0x00, 0x0F, /*             XXXX */
        0x00, 0x3C, /*           XXXX   */
        0x00, 0xF0, /*         XXXX     */
        0x03, 0xC0, /*       XXXX       */
        0x0F, 0x00, /*     XXXX         */
        0x3C, 0x00, /*   XXXX           */
        0x00, 0x00, /*                  */
    },
    {
        0x03, 0xC0, /*       XXXX       */
        0x03, 0xC0, /*       XXXX       */
        0x0F, 0xC0, /*     XXXXXX       */
        0x03, 0xC0, /*       XXXX       */
        0x03, 0xC0, /*       XXXX       */
        0x03, 0xC0, /*       XXXX       */
        0x3F, 0xFC, /*   XXXXXXXXXXXX   */
        0x00, 0x00, /*                  */
    },
    {
        0x0F, 0xF0, /*     XXXXXXXX     */
        0x3C, 0x3C, /*   XXXX    XXXX   */
        0x00, 0x3C, /*           XXXX   */
        0x00, 0xF0, /*         XXXX     */
        0x0F, 0x00, /*     XXXX         */
        0x3C, 0x00, /*   XXXX           */
        0x3F, 0xFC, /*   XXXXXXXXXXXX   */
        0x00, 0x00, /*                  */
    },
    {
        0x0F, 0xF0, /*     XXXXXXXX     */
        0x3C, 0x3C, /*   XXXX    XXXX   */
        0x00, 0x3C, /*           XXXX   */
        0x03, 0xF0, /*       XXXXXX     */
        0x00, 0x3C, /*           XXXX   */
        0x3C, 0x3C, /*   XXXX    XXXX   */
        0x0F, 0xF0, /*     XXXXXXXX     */
        0x00, 0x00, /*                  */
    },
    {
        0x00, 0x00, /*                  */
        0x00, 0xFC, /*         XXXXXX   */
        0x03, 0xFC, /*       XXXXXXXX   */
        0x3C, 0x3C, /*   XXXX    XXXX   */
        0x3F, 0xFF, /*   XXXXXXXXXXXXXX */
        0x00, 0x3C, /*           XXXX   */
        0x00, 0x3C, /*           XXXX   */
        0x00, 0x00, /*                  */
    },
    {
        0x3F, 0xFC, /*   XXXXXXXXXXXX   */
        0x3C, 0x00, /*   XXXX           */
        0x3F, 0xF0, /*   XXXXXXXXXX     */
        0x00, 0x3C, /*           XXXX   */
        0x00, 0x3C, /*           XXXX   */
        0x3C, 0x3C, /*   XXXX    XXXX   */
        0x0F, 0xF0, /*     XXXXXXXX     */
        0x00, 0x00, /*                  */
    },
    {
        0x0F, 0xF0, /*     XXXXXXXX     */
        0x3C, 0x3C, /*   XXXX    XXXX   */
        0x3C, 0x00, /*   XXXX           */
        0x3F, 0xF0, /*   XXXXXXXXXX     */
        0x3C, 0x3C, /*   XXXX    XXXX   */
        0x3C, 0x3C, /*   XXXX    XXXX   */
        0x0F, 0xF0, /*     XXXXXXXX     */
        0x00, 0x00, /*                  */
    },
    {
        0x3F, 0xFC, /*   XXXXXXXXXXXX   */
        0x3C, 0x3C, /*   XXXX    XXXX   */
        0x00, 0xF0, /*         XXXX     */
        0x03, 0xC0, /*       XXXX       */
        0x03, 0xC0, /*       XXXX       */
        0x03, 0xC0, /*       XXXX       */
        0x03, 0xC0, /*       XXXX       */
        0x00, 0x00, /*                  */
    },
    {
        0x0F, 0xF0, /*     XXXXXXXX     */
        0x3C, 0x3C, /*   XXXX    XXXX   */
        0x3C, 0x3C, /*   XXXX    XXXX   */
        0x0F, 0xF0, /*     XXXXXXXX     */
        0x3C, 0x3C, /*   XXXX    XXXX   */
        0x3C, 0x3C, /*   XXXX    XXXX   */
        0x0F, 0xF0, /*     XXXXXXXX     */
        0x00, 0x00, /*                  */
    },
    {
        0x0F, 0xF0, /*     XXXXXXXX     */
        0x3C, 0x3C, /*   XXXX    XXXX   */
        0x3C, 0x3C, /*   XXXX    XXXX   */
        0x0F, 0xFC, /*     XXXXXXXXXX   */
        0x00, 0x3C, /*           XXXX   */
        0x3C, 0x3C, /*   XXXX    XXXX   */
        0x0F, 0xF0, /*     XXXXXXXX     */
        0x00, 0x00, /*                  */
    },
    {
        0x0F, 0xF0, /*     XXXXXXXX     */
        0x3C, 0x3C, /*   XXXX    XXXX   */
        0x3C, 0xFC, /*   XXXX  XXXXXX   */
        0x3F, 0x3C, /*   XXXXXX  XXXX   */
        0x3C, 0x3C, /*   XXXX    XXXX   */
        0x3C, 0x3C, /*   XXXX    XXXX   */
        0x0F, 0xF0, /*     XXXXXXXX     */
        0x00, 0x00, /*                  */
    },
    {
        0x00, 0x00, /*                  */
        0x00, 0x00, /*                  */
        0x0F, 0xF0, /*     XXXXXXXX     */
        0x00, 0x3C, /*           XXXX   */
        0x0F, 0xFC, /*     XXXXXXXXXX   */
        0x3C, 0x3C, /*   XXXX    XXXX   */
        0x0F, 0xFC, /*     XXXXXXXXXX   */
        0x00, 0x00, /*                  */
    },
    {
        0x00, 0x00, /*                  */
        0x3C, 0x00, /*   XXXX           */
        0x3C, 0x00, /*   XXXX           */
        0x3F, 0xF0, /*   XXXXXXXXXX     */
        0x3C, 0x3C, /*   XXXX    XXXX   */
        0x3C, 0x3C, /*   XXXX    XXXX   */
        0x3F, 0xF0, /*   XXXXXXXXXX     */
        0x00, 0x00, /*                  */
    },
    {
        0x00, 0x00, /*                  */
        0x00, 0x00, /*                  */
        0x0F, 0xF0, /*     XXXXXXXX     */
        0x3C, 0x00, /*   XXXX           */
        0x3C, 0x00, /*   XXXX           */
        0x3C, 0x00, /*   XXXX           */
        0x0F, 0xF0, /*     XXXXXXXX     */
        0x00, 0x00, /*                  */
    },
    {
        0x00, 0x00, /*                  */
        0x00, 0x3C, /*           XXXX   */
        0x00, 0x3C, /*           XXXX   */
        0x0F, 0xFC, /*     XXXXXXXXXX   */
        0x3C, 0x3C, /*   XXXX    XXXX   */
        0x3C, 0x3C, /*   XXXX    XXXX   */
        0x0F, 0xFC, /*     XXXXXXXXXX   */
        0x00, 0x00, /*                  */
    },
    {
        0x00, 0x00, /*                  */
        0x00, 0x00, /*                  */
        0x0F, 0xF0, /*     XXXXXXXX     */
        0x3C, 0x3C, /*   XXXX    XXXX   */
        0x3F, 0xFC, /*   XXXXXXXXXXXX   */
        0x3C, 0x00, /*   XXXX           */
        0x0F, 0xF0, /*     XXXXXXXX     */
        0x00, 0x00, /*                  */
    },
    {
        0x00, 0x00, /*                  */
        0x00, 0xFC, /*         XXXXXX   */
        0x03, 0xC0, /*       XXXX       */
        0x0F, 0xFC, /*     XXXXXXXXXX   */
        0x03, 0xC0, /*       XXXX       */
        0x03, 0xC0, /*       XXXX       */
        0x03, 0xC0, /*       XXXX       */
        0x00, 0x00, /*                  */
    },
    {
        0x00, 0x00, /*                  */
        0x00, 0x00, /*                  */
        0x0F, 0xFC, /*     XXXXXXXXXX   */
        0x3C, 0x3C, /*   XXXX    XXXX   */
        0x3C, 0x3C, /*   XXXX    XXXX   */
        0x0F, 0xFC, /*     XXXXXXXXXX   */
        0x00, 0x3C, /*           XXXX   */
        0x3F, 0xF0, /*   XXXXXXXXXX     */
    },
    {
        0x3C, 0x00, /*   XXXX           */
        0x3C, 0x00, /*   XXXX           */
        0x3F, 0xF0, /*   XXXXXXXXXX     */
        0x3C, 0x3C, /*   XXXX    XXXX   */
        0x3C, 0x3C, /*   XXXX    XXXX   */
        0x3C, 0x3C, /*   XXXX    XXXX   */
        0x3C, 0x3C, /*   XXXX    XXXX   */
        0x00, 0x00, /*                  */
    },
    {
        0x03, 0xC0, /*       XXXX       */
        0x00, 0x00, /*                  */
        0x0F, 0xC0, /*     XXXXXX       */
        0x03, 0xC0, /*       XXXX       */
        0x03, 0xC0, /*       XXXX       */
        0x03, 0xC0, /*       XXXX       */
        0x0F, 0xF0, /*     XXXXXXXX     */
        0x00, 0x00, /*                  */
    },
    {
        0x00, 0x00, /*                  */
        0x00, 0xF0, /*         XXXX     */
        0x00, 0x00, /*                  */
        0x00, 0xF0, /*         XXXX     */
        0x00, 0xF0, /*         XXXX     */
        0x00, 0xF0, /*         XXXX     */
        0x00, 0xF0, /*         XXXX     */
        0x0F, 0xC0, /*     XXXXXX       */
    },
    {
        0x3C, 0x00, /*   XXXX           */
        0x3C, 0x00, /*   XXXX           */
        0x3C, 0x00, /*   XXXX           */
        0x3C, 0xF0, /*   XXXX  XXXX     */
        0x3F, 0xC0, /*   XXXXXXXX       */
        0x3C, 0xF0, /*   XXXX  XXXX     */
        0x3C, 0x3C, /*   XXXX    XXXX   */
        0x00, 0x00, /*                  */
    },
    {
        0x0F, 0xC0, /*     XXXXXX       */
        0x03, 0xC0, /*       XXXX       */
        0x03, 0xC0, /*       XXXX       */
        0x03, 0xC0, /*       XXXX       */
        0x03, 0xC0, /*       XXXX       */
        0x03, 0xC0, /*       XXXX       */
        0x0F, 0xF0, /*     XXXXXXXX     */
        0x00, 0x00, /*                  */
    },
    {
        0x00, 0x00, /*                  */
        0x00, 0x00, /*                  */
        0x3C, 0x3C, /*   XXXX    XXXX   */
        0x3F, 0xFF, /*   XXXXXXXXXXXXXX */
        0x3F, 0xFF, /*   XXXXXXXXXXXXXX */
        0x3C, 0xCF, /*   XXXX  XX  XXXX */
        0x3C, 0x0F, /*   XXXX      XXXX */
        0x00, 0x00, /*                  */
    },
    {
        0x00, 0x00, /*                  */
        0x00, 0x00, /*                  */
        0x3F, 0xF0, /*   XXXXXXXXXX     */
        0x3C, 0x3C, /*   XXXX    XXXX   */
        0x3C, 0x3C, /*   XXXX    XXXX   */
        0x3C, 0x3C, /*   XXXX    XXXX   */
        0x3C, 0x3C, /*   XXXX    XXXX   */
        0x00, 0x00, /*                  */
    },
    {
        0x00, 0x00, /*                  */
        0x00, 0x00, /*                  */
        0x0F, 0xF0, /*     XXXXXXXX     */
        0x3C, 0x3C, /*   XXXX    XXXX   */
        0x3C, 0x3C, /*   XXXX    XXXX   */
        0x3C, 0x3C, /*   XXXX    XXXX   */
        0x0F, 0xF0, /*     XXXXXXXX     */
        0x00, 0x00, /*                  */
    },
    {
        0x00, 0x00, /*                  */
        0x00, 0x00, /*                  */
        0x3F, 0xF0, /*   XXXXXXXXXX     */
        0x3C, 0x3C, /*   XXXX    XXXX   */
        0x3C, 0x3C, /*   XXXX    XXXX   */
        0x3F, 0xF0, /*   XXXXXXXXXX     */
        0x3C, 0x00, /*   XXXX           */
        0x3C, 0x00, /*   XXXX           */
    },
    {
        0x00, 0x00, /*                  */
        0x00, 0x00, /*                  */
        0x0F, 0xFC, /*     XXXXXXXXXX   */
        0x3C, 0x3C, /*   XXXX    XXXX   */
        0x3C, 0x3C, /*   XXXX    XXXX   */
        0x0F, 0xFC, /*     XXXXXXXXXX   */
        0x00, 0x3C, /*           XXXX   */
        0x00, 0x3C, /*           XXXX   */
    },
    {
        0x00, 0x00, /*                  */
        0x00, 0x00, /*                  */
        0x3F, 0xF0, /*   XXXXXXXXXX     */
        0x3C, 0x3C, /*   XXXX    XXXX   */
        0x3C, 0x00, /*   XXXX           */
        0x3C, 0x00, /*   XXXX           */
        0x3C, 0x00, /*   XXXX           */
        0x00, 0x00, /*                  */
    },
    {
        0x00, 0x00, /*                  */
        0x00, 0x00, /*                  */
        0x0F, 0xFC, /*     XXXXXXXXXX   */
        0x3C, 0x00, /*   XXXX           */
        0x0F, 0xF0, /*     XXXXXXXX     */
        0x00, 0x3C, /*           XXXX   */
        0x3F, 0xF0, /*   XXXXXXXXXX     */
        0x00, 0x00, /*                  */
    },
    {
        0x00, 0x00, /*                  */
        0x03, 0xC0, /*       XXXX       */
        0x3F, 0xFC, /*   XXXXXXXXXXXX   */
        0x03, 0xC0, /*       XXXX       */
        0x03, 0xC0, /*       XXXX       */
        0x03, 0xC0, /*       XXXX       */
        0x00, 0xFC, /*         XXXXXX   */
        0x00, 0x00, /*                  */
    },
    {
        0x00, 0x00, /*                  */
        0x00, 0x00, /*                  */
        0x3C, 0x3C, /*   XXXX    XXXX   */
        0x3C, 0x3C, /*   XXXX    XXXX   */
        0x3C, 0x3C, /*   XXXX    XXXX   */
        0x3C, 0x3C, /*   XXXX    XXXX   */
        0x0F, 0xFC, /*     XXXXXXXXXX   */
        0x00, 0x00, /*                  */
    },
    {
        0x00, 0x00, /*                  */
        0x00, 0x00, /*                  */
        0x3C, 0x3C, /*   XXXX    XXXX   */
        0x3C, 0x3C, /*   XXXX    XXXX   */
        0x3C, 0x3C, /*   XXXX    XXXX   */
        0x0F, 0xF0, /*     XXXXXXXX     */
        0x03, 0xC0, /*       XXXX       */
        0x00, 0x00, /*                  */
    },
    {
        0x00, 0x00, /*                  */
        0x00, 0x00, /*                  */
        0x3C, 0x0F, /*   XXXX      XXXX */
        0x3C, 0xCF, /*   XXXX  XX  XXXX */
        0x3F, 0xFF, /*   XXXXXXXXXXXXXX */
        0x0F, 0xFC, /*     XXXXXXXXXX   */
        0x0F, 0x3C, /*     XXXX  XXXX   */
        0x00, 0x00, /*                  */
    },
    {
        0x00, 0x00, /*                  */
        0x00, 0x00, /*                  */
        0x3C, 0x3C, /*   XXXX    XXXX   */
        0x0F, 0xF0, /*     XXXXXXXX     */
        0x03, 0xC0, /*       XXXX       */
        0x0F, 0xF0, /*     XXXXXXXX     */
        0x3C, 0x3C, /*   XXXX    XXXX   */
        0x00, 0x00, /*                  */
    },
    {
        0x00, 0x00, /*                  */
        0x00, 0x00, /*                  */
        0x3C, 0x3C, /*   XXXX    XXXX   */
        0x3C, 0x3C, /*   XXXX    XXXX   */
        0x3C, 0x3C, /*   XXXX    XXXX   */
        0x0F, 0xFC, /*     XXXXXXXXXX   */
        0x00, 0xF0, /*         XXXX     */
        0x3F, 0xC0, /*   XXXXXXXX       */
    },
    {
        0x00, 0x00, /*                  */
        0x00, 0x00, /*                  */
        0x3F, 0xFC, /*   XXXXXXXXXXXX   */
        0x00, 0xF0, /*         XXXX     */
        0x03, 0xC0, /*       XXXX       */
        0x0F, 0x00, /*     XXXX         */
        0x3F, 0xFC, /*   XXXXXXXXXXXX   */
        0x00, 0x00, /*                  */
    },
    {
        0x03, 0xC0, /*       XXXX       */
        0x0F, 0xF0, /*     XXXXXXXX     */
        0x3C, 0x3C, /*   XXXX    XXXX   */
        0x3F, 0xFC, /*   XXXXXXXXXXXX   */
        0x3C, 0x3C, /*   XXXX    XXXX   */
        0x3C, 0x3C, /*   XXXX    XXXX   */
        0x3C, 0x3C, /*   XXXX    XXXX   */
        0x00, 0x00, /*                  */
    },
    {
        0x3F, 0xF0, /*   XXXXXXXXXX     */
        0x3C, 0x3C, /*   XXXX    XXXX   */
        0x3C, 0x3C, /*   XXXX    XXXX   */
        0x3F, 0xF0, /*   XXXXXXXXXX     */
        0x3C, 0x3C, /*   XXXX    XXXX   */
        0x3C, 0x3C, /*   XXXX    XXXX   */
        0x3F, 0xF0, /*   XXXXXXXXXX     */
        0x00, 0x00, /*                  */
    },
    {
        0x0F, 0xF0, /*     XXXXXXXX     */
        0x3C, 0x3C, /*   XXXX    XXXX   */
        0x3C, 0x00, /*   XXXX           */
        0x3C, 0x00, /*   XXXX           */
        0x3C, 0x00, /*   XXXX           */
        0x3C, 0x3C, /*   XXXX    XXXX   */
        0x0F, 0xF0, /*     XXXXXXXX     */
        0x00, 0x00, /*                  */
    },
    {
        0x3F, 0xC0, /*   XXXXXXXX       */
        0x3C, 0xF0, /*   XXXX  XXXX     */
        0x3C, 0x3C, /*   XXXX    XXXX   */
        0x3C, 0x3C, /*   XXXX    XXXX   */
        0x3C, 0x3C, /*   XXXX    XXXX   */
        0x3C, 0xF0, /*   XXXX  XXXX     */
        0x3F, 0xC0, /*   XXXXXXXX       */
        0x00, 0x00, /*                  */
    },
    {
        0x3F, 0xFC, /*   XXXXXXXXXXXX   */
        0x3C, 0x00, /*   XXXX           */
        0x3C, 0x00, /*   XXXX           */
        0x3F, 0xC0, /*   XXXXXXXX       */
        0x3C, 0x00, /*   XXXX           */
        0x3C, 0x00, /*   XXXX           */
        0x3F, 0xFC, /*   XXXXXXXXXXXX   */
        0x00, 0x00, /*                  */
    },
    {
        0x3F, 0xFC, /*   XXXXXXXXXXXX   */
        0x3C, 0x00, /*   XXXX           */
        0x3C, 0x00, /*   XXXX           */
        0x3F, 0xC0, /*   XXXXXXXX       */
        0x3C, 0x00, /*   XXXX           */
        0x3C, 0x00, /*   XXXX           */
        0x3C, 0x00, /*   XXXX           */
        0x00, 0x00, /*                  */
    },
    {
        0x0F, 0xF0, /*     XXXXXXXX     */
        0x3C, 0x3C, /*   XXXX    XXXX   */
        0x3C, 0x00, /*   XXXX           */
        0x3C, 0xFC, /*   XXXX  XXXXXX   */
        0x3C, 0x3C, /*   XXXX    XXXX   */
        0x3C, 0x3C, /*   XXXX    XXXX   */
        0x0F, 0xF0, /*     XXXXXXXX     */
        0x00, 0x00, /*                  */
    },
    {
        0x3C, 0x3C, /*   XXXX    XXXX   */
        0x3C, 0x3C, /*   XXXX    XXXX   */
        0x3C, 0x3C, /*   XXXX    XXXX   */
        0x3F, 0xFC, /*   XXXXXXXXXXXX   */
        0x3C, 0x3C, /*   XXXX    XXXX   */
        0x3C, 0x3C, /*   XXXX    XXXX   */
        0x3C, 0x3C, /*   XXXX    XXXX   */
        0x00, 0x00, /*                  */
    },
    {
        0x0F, 0xF0, /*     XXXXXXXX     */
        0x03, 0xC0, /*       XXXX       */
        0x03, 0xC0, /*       XXXX       */
        0x03, 0xC0, /*       XXXX       */
        0x03, 0xC0, /*       XXXX       */
        0x03, 0xC0, /*       XXXX       */
        0x0F, 0xF0, /*     XXXXXXXX     */
        0x00, 0x00, /*                  */
    },
    {
        0x03, 0xFC, /*       XXXXXXXX   */
        0x00, 0xF0, /*         XXXX     */
        0x00, 0xF0, /*         XXXX     */
        0x00, 0xF0, /*         XXXX     */
        0x00, 0xF0, /*         XXXX     */
        0x3C, 0xF0, /*   XXXX  XXXX     */
        0x0F, 0xC0, /*     XXXXXX       */
        0x00, 0x00, /*                  */
    },
    {
        0x3C, 0x3C, /*   XXXX    XXXX   */
        0x3C, 0xF0, /*   XXXX  XXXX     */
        0x3F, 0xC0, /*   XXXXXXXX       */
        0x3F, 0x00, /*   XXXXXX         */
        0x3F, 0xC0, /*   XXXXXXXX       */
        0x3C, 0xF0, /*   XXXX  XXXX     */
        0x3C, 0x3C, /*   XXXX    XXXX   */
        0x00, 0x00, /*                  */
    },
    {
        0x3C, 0x00, /*   XXXX           */
        0x3C, 0x00, /*   XXXX           */
        0x3C, 0x00, /*   XXXX           */
        0x3C, 0x00, /*   XXXX           */
        0x3C, 0x00, /*   XXXX           */
        0x3C, 0x00, /*   XXXX           */
        0x3F, 0xFC, /*   XXXXXXXXXXXX   */
        0x00, 0x00, /*                  */
    },
    {
        0x3C, 0x0F, /*   XXXX      XXXX */
        0x3F, 0x3F, /*   XXXXXX  XXXXXX */
        0x3F, 0xFF, /*   XXXXXXXXXXXXXX */
        0x3C, 0xCF, /*   XXXX  XX  XXXX */
        0x3C, 0x0F, /*   XXXX      XXXX */
        0x3C, 0x0F, /*   XXXX      XXXX */
        0x3C, 0x0F, /*   XXXX      XXXX */
        0x00, 0x00, /*                  */
    },
    {
        0x3C, 0x3C, /*   XXXX    XXXX   */
        0x3F, 0x3C, /*   XXXXXX  XXXX   */
        0x3F, 0xFC, /*   XXXXXXXXXXXX   */
        0x3F, 0xFC, /*   XXXXXXXXXXXX   */
        0x3C, 0xFC, /*   XXXX  XXXXXX   */
        0x3C, 0x3C, /*   XXXX    XXXX   */
        0x3C, 0x3C, /*   XXXX    XXXX   */
        0x00, 0x00, /*                  */
    },
    {
        0x0F, 0xF0, /*     XXXXXXXX     */
        0x3C, 0x3C, /*   XXXX    XXXX   */
        0x3C, 0x3C, /*   XXXX    XXXX   */
        0x3C, 0x3C, /*   XXXX    XXXX   */
        0x3C, 0x3C, /*   XXXX    XXXX   */
        0x3C, 0x3C, /*   XXXX    XXXX   */
        0x0F, 0xF0, /*     XXXXXXXX     */
        0x00, 0x00, /*                  */
    },
    {
        0x3F, 0xF0, /*   XXXXXXXXXX     */
        0x3C, 0x3C, /*   XXXX    XXXX   */
        0x3C, 0x3C, /*   XXXX    XXXX   */
        0x3F, 0xF0, /*   XXXXXXXXXX     */
        0x3C, 0x00, /*   XXXX           */
        0x3C, 0x00, /*   XXXX           */
        0x3C, 0x00, /*   XXXX           */
        0x00, 0x00, /*                  */
    },
    {
        0x0F, 0xF0, /*     XXXXXXXX     */
        0x3C, 0x3C, /*   XXXX    XXXX   */
        0x3C, 0x3C, /*   XXXX    XXXX   */
        0x3C, 0x3C, /*   XXXX    XXXX   */
        0x3C, 0x3C, /*   XXXX    XXXX   */
        0x0F, 0xF0, /*     XXXXXXXX     */
        0x00, 0xFC, /*         XXXXXX   */
        0x00, 0x00, /*                  */
    },
    {
        0x3F, 0xF0, /*   XXXXXXXXXX     */
        0x3C, 0x3C, /*   XXXX    XXXX   */
        0x3C, 0x3C, /*   XXXX    XXXX   */
        0x3F, 0xF0, /*   XXXXXXXXXX     */
        0x3F, 0xC0, /*   XXXXXXXX       */
        0x3C, 0xF0, /*   XXXX  XXXX     */
        0x3C, 0x3C, /*   XXXX    XXXX   */
        0x00, 0x00, /*                  */
    },
    {
        0x0F, 0xF0, /*     XXXXXXXX     */
        0x3C, 0x3C, /*   XXXX    XXXX   */
        0x3C, 0x00, /*   XXXX           */
        0x0F, 0xF0, /*     XXXXXXXX     */
        0x00, 0x3C, /*           XXXX   */
        0x3C, 0x3C, /*   XXXX    XXXX   */
        0x0F, 0xF0, /*     XXXXXXXX     */
        0x00, 0x00, /*                  */
    },
    {
        0x3F, 0xFC, /*   XXXXXXXXXXXX   */
        0x03, 0xC0, /*       XXXX       */
        0x03, 0xC0, /*       XXXX       */
        0x03, 0xC0, /*       XXXX       */
        0x03, 0xC0, /*       XXXX       */
        0x03, 0xC0, /*       XXXX       */
        0x03, 0xC0, /*       XXXX       */
        0x00, 0x00, /*                  */
    },
    {
        0x3C, 0x3C, /*   XXXX    XXXX   */
        0x3C, 0x3C, /*   XXXX    XXXX   */
        0x3C, 0x3C, /*   XXXX    XXXX   */
        0x3C, 0x3C, /*   XXXX    XXXX   */
        0x3C, 0x3C, /*   XXXX    XXXX   */
        0x3C, 0x3C, /*   XXXX    XXXX   */
        0x0F, 0xF0, /*     XXXXXXXX     */
        0x00, 0x00, /*                  */
    },
    {
        0x3C, 0x3C, /*   XXXX    XXXX   */
        0x3C, 0x3C, /*   XXXX    XXXX   */
        0x3C, 0x3C, /*   XXXX    XXXX   */
        0x3C, 0x3C, /*   XXXX    XXXX   */
        0x3C, 0x3C, /*   XXXX    XXXX   */
        0x0F, 0xF0, /*     XXXXXXXX     */
        0x03, 0xC0, /*       XXXX       */
        0x00, 0x00, /*                  */
    },
    {
        0x3C, 0x0F, /*   XXXX      XXXX */
        0x3C, 0x0F, /*   XXXX      XXXX */
        0x3C, 0x0F, /*   XXXX      XXXX */
        0x3C, 0xCF, /*   XXXX  XX  XXXX */
        0x3F, 0xFF, /*   XXXXXXXXXXXXXX */
        0x3F, 0x3F, /*   XXXXXX  XXXXXX */
        0x3C, 0x0F, /*   XXXX      XXXX */
        0x00, 0x00, /*                  */
    },
    {
        0x3C, 0x3C, /*   XXXX    XXXX   */
        0x3C, 0x3C, /*   XXXX    XXXX   */
        0x0F, 0xF0, /*     XXXXXXXX     */
        0x03, 0xC0, /*       XXXX       */
        0x0F, 0xF0, /*     XXXXXXXX     */
        0x3C, 0x3C, /*   XXXX    XXXX   */
        0x3C, 0x3C, /*   XXXX    XXXX   */
        0x00, 0x00, /*                  */
    },
    {
        0x3C, 0x3C, /*   XXXX    XXXX   */
        0x3C, 0x3C, /*   XXXX    XXXX   */
        0x3C, 0x3C, /*   XXXX    XXXX   */
        0x0F, 0xF0, /*     XXXXXXXX     */
        0x03, 0xC0, /*       XXXX       */
        0x03, 0xC0, /*       XXXX       */
        0x03, 0xC0, /*       XXXX       */
        0x00, 0x00, /*                  */
    },
    {
        0x3F, 0xFC, /*   XXXXXXXXXXXX   */
        0x00, 0x3C, /*           XXXX   */
        0x00, 0xF0, /*         XXXX     */
        0x03, 0xC0, /*       XXXX       */
        0x0F, 0x00, /*     XXXX         */
        0x3C, 0x00, /*   XXXX           */
        0x3F, 0xFC, /*   XXXXXXXXXXXX   */
        0x00, 0x00, /*                  */
    },
};
