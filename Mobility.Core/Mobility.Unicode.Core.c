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

/**
 * @brief Converts the first UTF-8 character to a Unicode scalar value.
 * @param Destination The scalar output. Missing or invalid input leaves this
 *                    output unchanged.
 * @param Source The input bytes, which are not modified. A null pointer
 *               represents missing input.
 * @param SourceLength The number of available bytes. Bytes following the
 *                     first character are not inspected.
 * @return The number of bytes parsed, from 1 to 4, including 1 for U+0000.
 *         Missing input returns 0. Malformed, truncated, overlong, surrogate
 *         or out-of-range input returns -1.
*/
static MO_INT32 Utf8ToCodePoint(
    _Mo_Out_ PMO_UINT32 Destination,
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
        Character = ((MO_UINT32)(FirstByte & 0x1F) << 6) | (SecondByte & 0x3F);
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
        Character = ((MO_UINT32)(FirstByte & 0x0F) << 12);
        Character |= ((SecondByte & 0x3F) << 6) | (ThirdByte & 0x3F);
        Length = 3;
    }
    else if (FirstByte >= 0xF0 && FirstByte <= 0xF4)
    {
        if (SourceLength < 4)
        {
            return -1;
        }
        MO_UINT8 SecondByte = (MO_UINT8)Source[1];
        MO_UINT8 ThirdByte = (MO_UINT8)Source[2];
        MO_UINT8 FourthByte = (MO_UINT8)Source[3];
        if ((SecondByte & 0xC0) != 0x80 ||
            (ThirdByte & 0xC0) != 0x80 ||
            (FourthByte & 0xC0) != 0x80 ||
            (FirstByte == 0xF0 && SecondByte < 0x90) ||
            (FirstByte == 0xF4 && SecondByte >= 0x90))
        {
            return -1;
        }
        Character = (MO_UINT32)(FirstByte & 0x07) << 18;
        Character |= (MO_UINT32)(SecondByte & 0x3F) << 12;
        Character |= ((ThirdByte & 0x3F) << 6) | (FourthByte & 0x3F);
        Length = 4;
    }
    else
    {
        return -1;
    }

    *Destination = Character;
    return Length;
}

/**
 * @brief Converts one Unicode scalar value to a UTF-8 byte sequence.
 * @param Destination Optional output buffer. If nullptr, only the conversion
 *                    length is returned and DestinationLength is ignored.
 * @param Source The Unicode scalar value. Surrogate code points and values
 *               above U+10FFFF are invalid.
 * @param DestinationLength The output capacity in bytes. U+0000 needs one byte.
 *                          No extra terminator is appended.
 * @return The number of bytes written, from 1 to 4, including 1 for U+0000.
 *         Invalid input or insufficient capacity returns -1 without modifying
 *         Destination.
*/
static MO_INT32 CodePointToUtf8(
    _Mo_Out_Opt_ PMO_CHAR Destination,
    _Mo_In_ MO_UINT32 Source,
    _Mo_In_ MO_UINTN DestinationLength)
{
    if (Source > 0x10FFFF || (Source >= 0xD800 && Source <= 0xDFFF))
    {
        return -1;
    }

    MO_INT32 Length;
    if (Source < 0x80)
    {
        Length = 1;
    }
    else if (Source < 0x800)
    {
        Length = 2;
    }
    else if (Source < 0x10000)
    {
        Length = 3;
    }
    else
    {
        Length = 4;
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
            Bytes[0] = (MO_UINT8)Source;
        }
        else if (Length == 2)
        {
            Bytes[0] = (MO_UINT8)(0xC0 | (Source >> 6));
            Bytes[1] = (MO_UINT8)(0x80 | (Source & 0x3F));
        }
        else if (Length == 3)
        {
            Bytes[0] = (MO_UINT8)(0xE0 | (Source >> 12));
            Bytes[1] = (MO_UINT8)(0x80 | ((Source >> 6) & 0x3F));
            Bytes[2] = (MO_UINT8)(0x80 | (Source & 0x3F));
        }
        else
        {
            Bytes[0] = (MO_UINT8)(0xF0 | (Source >> 18));
            Bytes[1] = (MO_UINT8)(0x80 | ((Source >> 12) & 0x3F));
            Bytes[2] = (MO_UINT8)(0x80 | ((Source >> 6) & 0x3F));
            Bytes[3] = (MO_UINT8)(0x80 | (Source & 0x3F));
        }
    }
    return Length;
}

MO_EXTERN_C MO_INT32 MOAPI MoUnicodeCoreParseUcs2FromUtf8(
    _Mo_Out_Opt_ PMO_WIDE_CHAR Destination,
    _Mo_In_ PMO_CHAR Source,
    _Mo_In_ MO_UINTN SourceLength)
{
    MO_UINT32 Character;
    MO_INT32 Length = Utf8ToCodePoint(&Character, Source, SourceLength);
    if (Length <= 0)
    {
        return Length;
    }
    if (Character > 0xFFFF)
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
    if (Character > 0xFFFF)
    {
        return -1;
    }
    MO_INT32 Length = CodePointToUtf8(
        Destination,
        Character,
        DestinationLength);
    if (Length < 0)
    {
        return Length;
    }
    return Character == MO_UNICODE_NULL ? 0 : Length;
}
