/*
 * PROJECT:    Mobility
 * FILE:       Mobility.Console.Core.c
 * PURPOSE:    Implementation for Mobility Console Core
 *
 * LICENSE:    The MIT License
 *
 * MAINTAINER: MouriNaruto (Kenji.Mouri@outlook.com)
 */

#include "Mobility.Console.Core.h"

#include "Mobility.BitmapFont.LaffStd.h"

MO_EXTERN_C MO_UINT32 MOAPI MoConsoleCoreGetBackgroundColor(
    _Mo_In_Opt_ PMO_CONSOLE_SCREEN_BUFFER ConsoleScreenBuffer)
{
    if (!ConsoleScreenBuffer)
    {
        return MO_CONSOLE_DEFAULT_BACKGROUND_COLOR;
    }

    return ConsoleScreenBuffer->ColorLookupTable[0][0];
}

MO_EXTERN_C MO_UINT32 MOAPI MoConsoleCoreGetForegroundColor(
    _Mo_In_Opt_ PMO_CONSOLE_SCREEN_BUFFER ConsoleScreenBuffer)
{
    if (!ConsoleScreenBuffer)
    {
        return MO_CONSOLE_DEFAULT_FOREGROUND_COLOR;
    }

    return ConsoleScreenBuffer->ColorLookupTable[8][0];
}

MO_EXTERN_C MO_VOID MOAPI MoConsoleCoreUpdateColorSettings(
    _Mo_Out_ PMO_CONSOLE_SCREEN_BUFFER ConsoleScreenBuffer,
    _Mo_In_ MO_UINT32 BackgroundColor,
    _Mo_In_ MO_UINT32 ForegroundColor)
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

MO_EXTERN_C MO_VOID MOAPI MoConsoleCoreInitializeScreenBuffer(
    _Mo_Out_ PMO_CONSOLE_SCREEN_BUFFER ConsoleScreenBuffer,
    _Mo_In_ MO_GRAPHICS_SIZE ScreenBufferSize,
    _Mo_In_ MO_UINT32 BackgroundColor,
    _Mo_In_ MO_UINT32 ForegroundColor,
    _Mo_In_ PMO_WIDE_CHAR CharacterBuffer)
{
    if (!ConsoleScreenBuffer ||
        ScreenBufferSize.Width <= 0 ||
        ScreenBufferSize.Height <= 0)
    {
        return;
    }

    ConsoleScreenBuffer->ScreenBufferSize = ScreenBufferSize;
    ConsoleScreenBuffer->CursorPosition.X = 0;
    ConsoleScreenBuffer->CursorPosition.Y = 0;
    MoConsoleCoreUpdateColorSettings(
        ConsoleScreenBuffer,
        BackgroundColor,
        ForegroundColor);
    ConsoleScreenBuffer->CharacterBuffer = CharacterBuffer;
    ConsoleScreenBuffer->CharacterBufferMaximumSize =
        (MO_UINT32)ConsoleScreenBuffer->ScreenBufferSize.Width *
        (MO_UINT32)ConsoleScreenBuffer->ScreenBufferSize.Height;
    ConsoleScreenBuffer->CharacterBufferStartOffset = 0;
}

MO_EXTERN_C MO_VOID MOAPI MoConsoleCoreDrawCharacter(
    _Mo_Out_ PMO_GRAPHICS_FRAMEBUFFER DisplayFrameBuffer,
    _Mo_In_ PMO_CONSOLE_SCREEN_BUFFER ConsoleScreenBuffer,
    _Mo_In_ MO_GRAPHICS_POINT DestinationCoordinate,
    _Mo_In_ MO_WIDE_CHAR Character)
{
    if (!DisplayFrameBuffer ||
        !DisplayFrameBuffer->Base ||
        DisplayFrameBuffer->Size.Width <= 0 ||
        DisplayFrameBuffer->Size.Height <= 0 ||
        DisplayFrameBuffer->Pitch <
        (MO_UINT32)DisplayFrameBuffer->Size.Width * sizeof(MO_UINT32) ||
        !ConsoleScreenBuffer ||
        ConsoleScreenBuffer->ScreenBufferSize.Width <= 0 ||
        ConsoleScreenBuffer->ScreenBufferSize.Height <= 0)
    {
        return;
    }

    if (DestinationCoordinate.X < 0 ||
        DestinationCoordinate.Y < 0 ||
        DestinationCoordinate.X >= ConsoleScreenBuffer->ScreenBufferSize.Width ||
        DestinationCoordinate.Y >= ConsoleScreenBuffer->ScreenBufferSize.Height)
    {
        return;
    }

    MO_INT16 FontWidth = MoBitmapFontLaffStd.Size.Width;
    MO_INT16 FontHeight = MoBitmapFontLaffStd.Size.Height;

    MO_GRAPHICS_SIZE MaximumScreenBufferSize;
    MaximumScreenBufferSize.Width = DisplayFrameBuffer->Size.Width / FontWidth;
    MaximumScreenBufferSize.Height = DisplayFrameBuffer->Size.Height / FontHeight;

    if (DestinationCoordinate.X >= MaximumScreenBufferSize.Width ||
        DestinationCoordinate.Y >= MaximumScreenBufferSize.Height)
    {
        return;
    }

    // Query the glyph data if the character is not null, or query the space
    // character instead.
    MO_CONST MO_UINT8* GlyphData = MoBitmapFontLaffStd.Query(
        Character ? Character : MO_UNICODE_SPACE);
    if (!GlyphData)
    {
        // Query the full block character if the glyph data is not found.
        GlyphData = MoBitmapFontLaffStd.Query(MO_UNICODE_FULL_BLOCK);
        if (!GlyphData)
        {
            // Just return if the glyph data is still not found.
            return;
        }
    }

    // Use 'volatile MO_UINT8*' to ensure the compiler treats the data pointed
    // to as volatile, forcing actual memory writes to the framebuffer.
    // Using 'volatile PMO_UINT8' would only make the pointer variable itself
    // volatile.
    volatile MO_UINT8* FrameBufferBase = (volatile MO_UINT8*)(
        DisplayFrameBuffer->Base);

    MO_UINT32 ScreenX = DestinationCoordinate.X * FontWidth;
    MO_UINT32 ScreenY = DestinationCoordinate.Y * FontHeight;
    for (MO_UINT8 GlyphY = 0; GlyphY < FontHeight; ++GlyphY)
    {
        volatile MO_UINT32* ScanLine = (volatile MO_UINT32*)(
            FrameBufferBase +
            (MO_UINTN)(ScreenY + GlyphY) * DisplayFrameBuffer->Pitch) +
            ScreenX;
        MO_UINT8 GlyphDataLow = GlyphData[GlyphY] & 0x0F;
        MO_UINT8 GlyphDataHigh = (GlyphData[GlyphY] & 0xF0) >> 4;
        ScanLine[0] = ConsoleScreenBuffer->ColorLookupTable[GlyphDataHigh][0];
        ScanLine[1] = ConsoleScreenBuffer->ColorLookupTable[GlyphDataHigh][1];
        ScanLine[2] = ConsoleScreenBuffer->ColorLookupTable[GlyphDataHigh][2];
        ScanLine[3] = ConsoleScreenBuffer->ColorLookupTable[GlyphDataHigh][3];
        ScanLine[4] = ConsoleScreenBuffer->ColorLookupTable[GlyphDataLow][0];
        ScanLine[5] = ConsoleScreenBuffer->ColorLookupTable[GlyphDataLow][1];
        ScanLine[6] = ConsoleScreenBuffer->ColorLookupTable[GlyphDataLow][2];
        ScanLine[7] = ConsoleScreenBuffer->ColorLookupTable[GlyphDataLow][3];
    }
}

MO_EXTERN_C MO_VOID MOAPI MoConsoleCoreRefreshScreen(
    _Mo_Out_ PMO_GRAPHICS_FRAMEBUFFER DisplayFrameBuffer,
    _Mo_In_ PMO_CONSOLE_SCREEN_BUFFER ConsoleScreenBuffer)
{
    if (!DisplayFrameBuffer || !ConsoleScreenBuffer)
    {
        return;
    }

    MO_UINT32 MaximumSize = ConsoleScreenBuffer->CharacterBufferMaximumSize;
    MO_UINT32 StartOffset = ConsoleScreenBuffer->CharacterBufferStartOffset;

    if (!MaximumSize ||
        ConsoleScreenBuffer->ScreenBufferSize.Width <= 0 ||
        ConsoleScreenBuffer->ScreenBufferSize.Height <= 0)
    {
        return;
    }

    MO_UINT32 CurrentOffset = StartOffset;

    for (MO_INT16 Y = 0; Y < ConsoleScreenBuffer->ScreenBufferSize.Height; ++Y)
    {
        for (MO_INT16 X = 0; X < ConsoleScreenBuffer->ScreenBufferSize.Width; ++X)
        {
            MO_GRAPHICS_POINT CurrentCoordinate = { X, Y };

            if (CurrentOffset >= MaximumSize)
            {
                CurrentOffset %= MaximumSize;
            }

            MoConsoleCoreDrawCharacter(
                DisplayFrameBuffer,
                ConsoleScreenBuffer,
                CurrentCoordinate,
                ConsoleScreenBuffer->CharacterBuffer[CurrentOffset]);

            ++CurrentOffset;
        }
    }
}

MO_EXTERN_C MO_VOID MOAPI MoConsoleCoreWriteString(
    _Mo_Out_ PMO_CONSOLE_SCREEN_BUFFER ConsoleScreenBuffer,
    _Mo_In_ MO_CONSTANT_STRING String,
    _Mo_In_ MO_UINT32 StringLength)
{
    if (!ConsoleScreenBuffer ||
        ConsoleScreenBuffer->ScreenBufferSize.Width <= 0 ||
        ConsoleScreenBuffer->ScreenBufferSize.Height <= 0 ||
        !String ||
        !StringLength)
    {
        return;
    }

    const MO_INT16 TabSize = 4;

    MO_INT16 ScreenWidth = ConsoleScreenBuffer->ScreenBufferSize.Width;
    MO_INT16 ScreenHeight = ConsoleScreenBuffer->ScreenBufferSize.Height;

    MO_UINT32 MaximumSize = ConsoleScreenBuffer->CharacterBufferMaximumSize;
    MO_UINT32 StartOffset = ConsoleScreenBuffer->CharacterBufferStartOffset;

    MO_GRAPHICS_POINT FinalPosition = ConsoleScreenBuffer->CursorPosition;
    if (FinalPosition.X < 0 || FinalPosition.X >= ScreenWidth)
    {
        FinalPosition.X = 0;
    }
    if (FinalPosition.Y < 0 || FinalPosition.Y >= ScreenHeight)
    {
        FinalPosition.Y = 0;
    }

    for (MO_UINT32 i = 0; i < StringLength;)
    {
        MO_WIDE_CHAR CurrentCharacter = MO_UNICODE_NULL;
        MO_INT32 BytesParsed = MoUnicodeCoreParseUcs2FromUtf8(
            &CurrentCharacter,
            (PMO_CHAR)(String + i),
            StringLength - i);
        if (BytesParsed == 0)
        {
            break;
        }
        if (BytesParsed == -1)
        {
            ++i;
            continue;
        }
        i += BytesParsed;

        if ('\n' == CurrentCharacter)
        {
            FinalPosition.X = 0;
            ++FinalPosition.Y;
        }
        else if ('\r' == CurrentCharacter)
        {
            FinalPosition.X = 0;
        }
        else if ('\t' == CurrentCharacter)
        {
            MO_INT32 NewX = (FinalPosition.X + TabSize) & ~(TabSize - 1);
            if (NewX < ScreenWidth)
            {
                FinalPosition.X = (MO_INT16)NewX;
            }
            else
            {
                FinalPosition.X = 0;
                ++FinalPosition.Y;
            }
        }
        else if ('\b' == CurrentCharacter)
        {
            if (FinalPosition.X > 0)
            {
                --FinalPosition.X;

                // Clear the character at the current position.

                MO_UINT32 FinalOffset = StartOffset;
                FinalOffset += (FinalPosition.Y * ScreenWidth) + FinalPosition.X;
                FinalOffset %= MaximumSize;
                ConsoleScreenBuffer->CharacterBuffer[FinalOffset] = '\0';
            }
            else if (FinalPosition.Y > 0)
            {
                --FinalPosition.Y;
                FinalPosition.X = ScreenWidth - 1;

                // Clear the character at the current position.

                MO_UINT32 FinalOffset = StartOffset;
                FinalOffset += (FinalPosition.Y * ScreenWidth) + FinalPosition.X;
                FinalOffset %= MaximumSize;
                ConsoleScreenBuffer->CharacterBuffer[FinalOffset] = '\0';
            }
        }
        else
        {
            MO_UINT32 FinalOffset = StartOffset;
            FinalOffset += (FinalPosition.Y * ScreenWidth) + FinalPosition.X;
            FinalOffset %= MaximumSize;

            ConsoleScreenBuffer->CharacterBuffer[FinalOffset] = CurrentCharacter;
            ++FinalPosition.X;
            if (FinalPosition.X >= ScreenWidth)
            {
                FinalPosition.X = 0;
                ++FinalPosition.Y;
            }
        }

        if (FinalPosition.Y >= ScreenHeight)
        {
            MO_UINT32 ScrollRows = FinalPosition.Y - ScreenHeight + 1;

            // Scroll the screen buffer up via changing the character buffer
            // start offset.

            ConsoleScreenBuffer->CharacterBufferStartOffset +=
                ScrollRows * ScreenWidth;
            ConsoleScreenBuffer->CharacterBufferStartOffset %= MaximumSize;
            StartOffset = ConsoleScreenBuffer->CharacterBufferStartOffset;

            // Clear the last row of the screen buffer.
            for (MO_INT16 Column = 0; Column < ScreenWidth; ++Column)
            {
                MO_UINT32 ClearOffset = StartOffset;
                ClearOffset +=
                    (MO_UINT32)((ScreenHeight - 1) * ScreenWidth + Column);
                ClearOffset %= MaximumSize;
                ConsoleScreenBuffer->CharacterBuffer[ClearOffset] = '\0';
            }

            // Reset the cursor position to the last row.
            FinalPosition.Y = ScreenHeight - 1;
        }
    }

    // Update the cursor position.
    ConsoleScreenBuffer->CursorPosition = FinalPosition;
}
