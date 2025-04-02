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

EXTERN_C VOID MOAPI MoConsoleCoreDrawCharacter(
    _Out_ PMO_DISPLAY_BGRA32_FRAMEBUFFER DisplayFrameBuffer,
    _In_ PMO_CONSOLE_SCREEN_BUFFER ConsoleScreenBuffer,
    _In_ MO_CONSOLE_COORDINATE DestinationCoordinate,
    _In_ MO_WIDE_CHAR Character)
{
    if (!DisplayFrameBuffer || !ConsoleScreenBuffer)
    {
        return;
    }

    if (DestinationCoordinate.X >= ConsoleScreenBuffer->ScreenBufferSize.X ||
        DestinationCoordinate.Y >= ConsoleScreenBuffer->ScreenBufferSize.Y)
    {
        return;
    }

    MO_UINT8 FontWidth = MoBitmapFontLaffStdGetWidth();
    MO_UINT8 FontHeight = MoBitmapFontLaffStdGetHeight();

    MO_CONSOLE_COORDINATE MaximumScreenBufferSize;
    MaximumScreenBufferSize.X = (MO_UINT16)(
        DisplayFrameBuffer->HorizontalResolution / FontWidth);
    MaximumScreenBufferSize.Y = (MO_UINT16)(
        DisplayFrameBuffer->VerticalResolution / FontHeight);

    if (DestinationCoordinate.X >= MaximumScreenBufferSize.X ||
        DestinationCoordinate.Y >= MaximumScreenBufferSize.Y)
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
        MO_UINTN PixelStartOffset = ScreenY + GlyphY;
        PixelStartOffset *= DisplayFrameBuffer->HorizontalResolution;
        PixelStartOffset += ScreenX;
        MO_UINT8 GlyphDataLow = GlyphData[GlyphY] & 0x0F;
        MO_UINT8 GlyphDataHigh = (GlyphData[GlyphY] & 0xF0) >> 4;
        DisplayFrameBuffer->FrameBufferBase[PixelStartOffset + 0] =
            ConsoleScreenBuffer->ColorLookupTable[GlyphDataHigh][0];
        DisplayFrameBuffer->FrameBufferBase[PixelStartOffset + 1] =
            ConsoleScreenBuffer->ColorLookupTable[GlyphDataHigh][1];
        DisplayFrameBuffer->FrameBufferBase[PixelStartOffset + 2] =
            ConsoleScreenBuffer->ColorLookupTable[GlyphDataHigh][2];
        DisplayFrameBuffer->FrameBufferBase[PixelStartOffset + 3] =
            ConsoleScreenBuffer->ColorLookupTable[GlyphDataHigh][3];
        DisplayFrameBuffer->FrameBufferBase[PixelStartOffset + 4] =
            ConsoleScreenBuffer->ColorLookupTable[GlyphDataLow][0];
        DisplayFrameBuffer->FrameBufferBase[PixelStartOffset + 5] =
            ConsoleScreenBuffer->ColorLookupTable[GlyphDataLow][1];
        DisplayFrameBuffer->FrameBufferBase[PixelStartOffset + 6] =
            ConsoleScreenBuffer->ColorLookupTable[GlyphDataLow][2];
        DisplayFrameBuffer->FrameBufferBase[PixelStartOffset + 7] =
            ConsoleScreenBuffer->ColorLookupTable[GlyphDataLow][3];
    }
}
