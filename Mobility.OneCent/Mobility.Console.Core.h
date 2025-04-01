/*
 * PROJECT:   Mobility
 * FILE:      Mobility.Console.Core.h
 * PURPOSE:   Definition for Mobility Console Core
 *
 * LICENSE:   The MIT License
 *
 * MAINTAINER: MouriNaruto (Kenji.Mouri@outlook.com)
 */

#ifndef MOBILITY_CONSOLE_CORE
#define MOBILITY_CONSOLE_CORE

#define MILE_MOBILITY_ENABLE_MINIMUM_SAL
#include <Mile.Mobility.Portable.Types.h>

#define MO_CONSOLE_COLORLUT_BITS 4
#define MO_CONSOLE_COLORLUT_ITEMS 16

typedef MO_UINT32 MO_CONSOLE_COLORLUT_ITEM[MO_CONSOLE_COLORLUT_BITS];

/**
 * @brief For improving the console drawing performance, Mobility Console
 *        introduces the console color lookup table for the current color
 *        settings with trying to get the balance between the memory usage
 *        and the performance.
 * @remark Here is the bitmap should be drawn on the screen which bit 0 means
 *         the background color and bit 1 means the foreground color:
 *         | 0 => 0 0 0 0 | 1 => 0 0 0 1 | 2 => 0 0 1 0 | 3 => 0 0 1 1 |
 *         | 4 => 0 1 0 0 | 5 => 0 1 0 1 | 6 => 0 1 1 0 | 7 => 0 1 1 1 |
 *         | 8 => 1 0 0 0 | 9 => 1 0 0 1 | A => 1 0 1 0 | B => 1 0 1 1 |
 *         | C => 1 1 0 0 | D => 1 1 0 1 | E => 1 1 1 0 | F => 1 1 1 1 |
 *         So, here are some quick internal only tricks for the lookup table:
 *         - Get index [0][0] for the background color.
 *         - Get index [8][0] for the foreground color.
 *         But you need to regenerate the lookup table if you want to modify the
 *         color settings.
 */
typedef MO_CONSOLE_COLORLUT_ITEM MO_CONSOLE_COLORLUT[MO_CONSOLE_COLORLUT_ITEMS];

/**
 * @brief Defines the coordinates of the specific character cell in the specific
 *        console screen buffer. The origin of the coordinate system (0,0) is at
 *        the top, left cell of the buffer.
 */
typedef struct _MO_CONSOLE_COORDINATE
{
    MO_UINT16 X;
    MO_UINT16 Y;
} MO_CONSOLE_COORDINATE, *PMO_CONSOLE_COORDINATE;

#define MO_CONSOLE_MAKE_COORDINATE(X, Y) \
    ((MO_CONSOLE_COORDINATE){ X, Y })

/**
 * @brief Contains the information of the console screen buffer.
 */
typedef struct _MO_CONSOLE_SCREEN_BUFFER
{
    MO_CONSOLE_COORDINATE ScreenSize;
    MO_CONSOLE_COORDINATE CursorPosition;
    MO_CONSOLE_COLORLUT ColorLookupTable;
    PMO_WIDE_CHAR Buffer;
} MO_CONSOLE_SCREEN_BUFFER, *PMO_CONSOLE_SCREEN_BUFFER;

#endif // !MOBILITY_CONSOLE_CORE
