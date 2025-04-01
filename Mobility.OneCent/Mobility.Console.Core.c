/*
 * PROJECT:   Mobility
 * FILE:      Mobility.Console.Core.c
 * PURPOSE:   Implementation for Mobility Console Core
 *
 * LICENSE:   The MIT License
 *
 * MAINTAINER: MouriNaruto (Kenji.Mouri@outlook.com)
 */

#include "Mobility.Console.Core.h"

#include "Mobility.BitmapFont.LaffStd.h"

EXTERN_C MO_UINT32 MOAPI MoConsoleCoreGetBackgroundColor(
    _In_opt_ PMO_CONSOLE_SCREEN_BUFFER ConsoleScreenBuffer)
{
    if (!ConsoleScreenBuffer)
    {
        return MO_CONSOLE_DEFAULT_BACKGROUND_COLOR;
    }

    return ConsoleScreenBuffer->ColorLookupTable[0][0];
}

EXTERN_C MO_UINT32 MOAPI MoConsoleCoreGetForegroundColor(
    _In_opt_ PMO_CONSOLE_SCREEN_BUFFER ConsoleScreenBuffer)
{
    if (!ConsoleScreenBuffer)
    {
        return MO_CONSOLE_DEFAULT_FOREGROUND_COLOR;
    }

    return ConsoleScreenBuffer->ColorLookupTable[8][0];
}

EXTERN_C VOID MOAPI MoConsoleCoreUpdateColorSettings(
    _Out_ PMO_CONSOLE_SCREEN_BUFFER ConsoleScreenBuffer,
    _In_ MO_UINT32 BackgroundColor,
    _In_ MO_UINT32 ForegroundColor)
{
    if (!ConsoleScreenBuffer)
    {
        return;
    }

    for (MO_UINT8 Index = 0; Index < MO_CONSOLE_COLORLUT_ITEMS; ++Index)
    {
        ConsoleScreenBuffer->ColorLookupTable[Index][0] =
            (Index & 0x08) ? ForegroundColor : BackgroundColor;
        ConsoleScreenBuffer->ColorLookupTable[Index][1] =
            (Index & 0x04) ? ForegroundColor : BackgroundColor;
        ConsoleScreenBuffer->ColorLookupTable[Index][2] =
            (Index & 0x02) ? ForegroundColor : BackgroundColor;
        ConsoleScreenBuffer->ColorLookupTable[Index][3] =
            (Index & 0x01) ? ForegroundColor : BackgroundColor;
    }
}

EXTERN_C VOID MOAPI MoConsoleCoreInitializeScreenBuffer(
    _Out_ PMO_CONSOLE_SCREEN_BUFFER ConsoleScreenBuffer,
    _In_ MO_UINT32 MaximumColumn,
    _In_ MO_UINT32 MaximumRow,
    _In_ MO_UINT32 BackgroundColor,
    _In_ MO_UINT32 ForegroundColor,
    _In_ PMO_WIDE_CHAR CharacterBuffer)
{
    if (!ConsoleScreenBuffer)
    {
        return;
    }

    ConsoleScreenBuffer->ScreenBufferSize.X = (MO_UINT16)MaximumColumn;
    ConsoleScreenBuffer->ScreenBufferSize.Y = (MO_UINT16)MaximumRow;
    ConsoleScreenBuffer->CursorPosition.X = 0;
    ConsoleScreenBuffer->CursorPosition.Y = 0;
    MoConsoleCoreUpdateColorSettings(
        ConsoleScreenBuffer,
        BackgroundColor,
        ForegroundColor);
    ConsoleScreenBuffer->CharacterBuffer = CharacterBuffer;
}

/**
 * @brief Gets the background color from the console color lookup table.
 * @param ColorLookupTable The console color lookup table you want to query, the
 *                         caller must ensure the memory address of the lookup
 *                         table is valid.
 * @return The background color from the console color lookup table.
 */
EXTERN_C MO_UINT32 MOAPI MoConsoleCoreInternalGetBackgroundColor(
    _In_ MO_CONSOLE_COLORLUT ColorLookupTable)
{
    return ColorLookupTable[0][0];
}

/**
 * @brief Gets the foreground color from the console color lookup table.
 * @param ColorLookupTable The console color lookup table you want to query, the
 *                         caller must ensure the memory address of the lookup
 *                         table is valid.
 * @return The foreground color from the console color lookup table.
 */
EXTERN_C MO_UINT32 MOAPI MoConsoleCoreInternalGetForegroundColor(
    _In_ MO_CONSOLE_COLORLUT ColorLookupTable)
{
    return ColorLookupTable[8][0];
}

/**
 * @brief Updates the color settings in the console color lookup table.
 *
 * @param ColorLookupTable The console color lookup table you want to update,
 *                         the caller must ensure the memory address of the
 *                         lookup table is valid.
 * @param BackgroundColor The new background color.
 * @param ForegroundColor The new foreground color.
 */
EXTERN_C VOID MOAPI MoConsoleCoreInternalUpdateColorSettings(
    _Out_ MO_CONSOLE_COLORLUT ColorLookupTable,
    _In_ MO_UINT32 BackgroundColor,
    _In_ MO_UINT32 ForegroundColor)
{
    for (MO_UINT8 Index = 0; Index < MO_CONSOLE_COLORLUT_ITEMS; ++Index)
    {
        ColorLookupTable[Index][0] =
            (Index & 0x08) ? ForegroundColor : BackgroundColor;
        ColorLookupTable[Index][1] =
            (Index & 0x04) ? ForegroundColor : BackgroundColor;
        ColorLookupTable[Index][2] =
            (Index & 0x02) ? ForegroundColor : BackgroundColor;
        ColorLookupTable[Index][3] =
            (Index & 0x01) ? ForegroundColor : BackgroundColor;
    }
}

/**
 * @brief Draws a character to the frame buffer.
 *
 * @param FrameBuffer The frame buffer to draw.
 * @param HorizontalResolution The horizontal resolution of the frame buffer.
 * @param VerticalResolution The vertical resolution of the frame buffer.
 * @param DestinationColumn The destination character column to draw.
 * @param DestinationRow The destination character row to draw.
 * @param Character The character to draw.
 * @param ColorLookupTable The console color lookup table for color settings.
 */
EXTERN_C VOID MOAPI MoConsoleCoreDrawCharacter(
    _Out_ PMO_UINT32 FrameBuffer,
    _In_ MO_UINT32 HorizontalResolution,
    _In_ MO_UINT32 VerticalResolution,
    _In_ MO_CONSOLE_COORDINATE DestinationCoordinate,
    _In_ MO_WIDE_CHAR Character,
    _In_ MO_CONSOLE_COLORLUT ColorLookupTable)
{
    MO_UINT8 FontWidth = MoBitmapFontLaffStdGetWidth();
    MO_UINT8 FontHeight = MoBitmapFontLaffStdGetHeight();

    MO_CONSOLE_COORDINATE MaximumSize = MO_CONSOLE_MAKE_COORDINATE(
        (MO_UINT16)(HorizontalResolution / FontWidth),
        (MO_UINT16)(VerticalResolution / FontHeight));

    if (DestinationCoordinate.X >= MaximumSize.X ||
        DestinationCoordinate.Y >= MaximumSize.Y)
    {
        return;
    }

    // Query the glyph data if the character is not null, or query the space
    // character instead.
    PMO_UINT8 GlyphData = MoBitmapFontLaffStdQueryGlyph(
        Character ? Character : L'\x0020');
    if (!GlyphData)
    {
        // Query the full block character if the glyph data is not found.
        GlyphData = MoBitmapFontLaffStdQueryGlyph(L'\x2588');
        if (!GlyphData)
        {
            // Just return if the glyph data is still not found.
            return;
        }
    }

    MO_UINT32 ScreenX = DestinationCoordinate.X * FontWidth;
    MO_UINT32 ScreenY = DestinationCoordinate.Y * FontHeight;
    for (MO_UINT8 GlyphY = 0; GlyphY < FontHeight; ++GlyphY)
    {
        MO_UINTN Start = ((ScreenY + GlyphY) * HorizontalResolution) + ScreenX;
        MO_UINT8 Low = GlyphData[GlyphY] & 0x0F;
        MO_UINT8 High = (GlyphData[GlyphY] & 0xF0) >> 4;
        FrameBuffer[Start + 0] = ColorLookupTable[High][0];
        FrameBuffer[Start + 1] = ColorLookupTable[High][1];
        FrameBuffer[Start + 2] = ColorLookupTable[High][2];
        FrameBuffer[Start + 3] = ColorLookupTable[High][3];
        FrameBuffer[Start + 4] = ColorLookupTable[Low][0];
        FrameBuffer[Start + 5] = ColorLookupTable[Low][1];
        FrameBuffer[Start + 6] = ColorLookupTable[Low][2];
        FrameBuffer[Start + 7] = ColorLookupTable[Low][3];
    }
}
