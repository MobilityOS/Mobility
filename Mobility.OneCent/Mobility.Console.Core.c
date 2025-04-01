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

/**
 * @brief Draws a character to the frame buffer.
 *
 * @param FrameBuffer The frame buffer to draw.
 * @param HorizontalResolution The horizontal resolution of the frame buffer.
 * @param VerticalResolution The vertical resolution of the frame buffer.
 * @param DestinationColumn The destination character column to draw.
 * @param DestinationRow The destination character row to draw.
 * @param Character The character to draw.
 * @param BackgroundColor The background color of the character.
 * @param ForegroundColor The foreground color of the character.
 */
EXTERN_C VOID MOAPI MoConsoleCoreDrawCharacter(
    _Out_ PMO_UINT32 FrameBuffer,
    _In_ MO_UINT32 HorizontalResolution,
    _In_ MO_UINT32 VerticalResolution,
    _In_ MO_UINT32 DestinationColumn,
    _In_ MO_UINT32 DestinationRow,
    _In_ MO_WIDE_CHAR Character,
    _In_ MO_UINT32 BackgroundColor,
    _In_ MO_UINT32 ForegroundColor)
{
    MO_UINT8 FontWidth = MoBitmapFontLaffStdGetWidth();
    MO_UINT8 FontHeight = MoBitmapFontLaffStdGetHeight();

    MO_UINT32 MaximumColumn = HorizontalResolution / FontWidth;
    MO_UINT32 MaximumRow = VerticalResolution / FontHeight;

    if (DestinationColumn >= MaximumColumn ||
        DestinationRow >= MaximumRow)
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

    MO_UINT32 ScreenX = DestinationColumn * FontWidth;
    MO_UINT32 ScreenY = DestinationRow * FontHeight;

    for (MO_UINT8 GlyphY = 0; GlyphY < FontHeight; ++GlyphY)
    {
        for (MO_UINT8 GlyphX = 0; GlyphX < FontWidth; ++GlyphX)
        {
            MO_UINT32 CurrentX = ScreenX + GlyphX;
            MO_UINT32 CurrentY = ScreenY + GlyphY;
            FrameBuffer[CurrentY * HorizontalResolution + CurrentX] =
                (GlyphData[GlyphY] & (1 << (FontWidth - GlyphX - 1)))
                ? ForegroundColor
                : BackgroundColor;
        }
    }
}
