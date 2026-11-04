/*
 * PROJECT:    Mobility
 * FILE:       Mobility.Unicode.Core.c
 * PURPOSE:    Implementation for Mobility Unicode Core
 *
 * LICENSE:    The MIT License
 *
 * MAINTAINER: MouriNaruto (Kenji.Mouri@outlook.com)
 */

#include "Mobility.Unicode.Core.h"

MO_EXTERN_C MO_INT32 MOAPI MoUnicodeCoreParseUcs2FromUtf8(
    _Mo_Out_Opt_ PMO_WIDE_CHAR Destination,
    _Mo_In_ PMO_CHAR Source,
    _Mo_In_ MO_UINTN SourceLength)
{
    if (!Source || !SourceLength)
    {
        return 0;
    }

    MO_UINT8 FirstByte = (MO_UINT8)Source[0];
    MO_UINT32 Character;
    MO_INT32 Length;
    if (FirstByte < 0x80)
    {
        Character = FirstByte;
        Length = 1;
    }
    else if (FirstByte >= 0xC2 && FirstByte <= 0xDF)
    {
        if (SourceLength < 2)
        {
            return -1;
        }
        MO_UINT8 SecondByte = (MO_UINT8)Source[1];
        if ((SecondByte & 0xC0) != 0x80)
        {
            return -1;
        }
        Character = ((MO_UINT32)(FirstByte & 0x1F) << 6) |
            (SecondByte & 0x3F);
        Length = 2;
    }
    else if (FirstByte >= 0xE0 && FirstByte <= 0xEF)
    {
        if (SourceLength < 3)
        {
            return -1;
        }
        MO_UINT8 SecondByte = (MO_UINT8)Source[1];
        MO_UINT8 ThirdByte = (MO_UINT8)Source[2];
        if ((SecondByte & 0xC0) != 0x80 ||
            (ThirdByte & 0xC0) != 0x80 ||
            (FirstByte == 0xE0 && SecondByte < 0xA0) ||
            (FirstByte == 0xED && SecondByte >= 0xA0))
        {
            return -1;
        }
        Character = ((MO_UINT32)(FirstByte & 0x0F) << 12) |
            ((SecondByte & 0x3F) << 6) | (ThirdByte & 0x3F);
        Length = 3;
    }
    else
    {
        return -1;
    }

    if (Destination)
    {
        *Destination = (MO_WIDE_CHAR)Character;
    }
    return Character == MO_UNICODE_NULL ? 0 : Length;
}

MO_EXTERN_C MO_INT32 MOAPI MoUnicodeCoreParseUtf8FromUcs2(
    _Mo_Out_Opt_ PMO_CHAR Destination,
    _Mo_In_ MO_WIDE_CHAR Source,
    _Mo_In_ MO_UINTN DestinationLength)
{
    MO_UINT32 Character = (MO_UINT32)Source;
    if (Character > 0xFFFF ||
        (Character >= 0xD800 && Character <= 0xDFFF))
    {
        return -1;
    }

    MO_INT32 Length;
    if (Character < 0x80)
    {
        Length = 1;
    }
    else if (Character < 0x800)
    {
        Length = 2;
    }
    else
    {
        Length = 3;
    }

    if (Destination)
    {
        if (DestinationLength < (MO_UINTN)Length)
        {
            return -1;
        }
        PMO_UINT8 Bytes = (PMO_UINT8)Destination;
        if (Length == 1)
        {
            Bytes[0] = (MO_UINT8)Character;
        }
        else if (Length == 2)
        {
            Bytes[0] = (MO_UINT8)(0xC0 | (Character >> 6));
            Bytes[1] = (MO_UINT8)(0x80 | (Character & 0x3F));
        }
        else
        {
            Bytes[0] = (MO_UINT8)(0xE0 | (Character >> 12));
            Bytes[1] = (MO_UINT8)(0x80 | ((Character >> 6) & 0x3F));
            Bytes[2] = (MO_UINT8)(0x80 | (Character & 0x3F));
        }
    }
    return Character == MO_UNICODE_NULL ? 0 : Length;
}
