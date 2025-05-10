/*
 * PROJECT:   Mobility
 * FILE:      Mobility.Unicode.Core.c
 * PURPOSE:   Implementation for Mobility Unicode Core
 *
 * LICENSE:   The MIT License
 *
 * MAINTAINER: MouriNaruto (Kenji.Mouri@outlook.com)
 */

#include "Mobility.Unicode.Core.h"

EXTERN_C MO_INT32 MOAPI MoUnicodeCoreParseUcs2FromUtf8(
    _Out_opt_ PMO_WIDE_CHAR Destination,
    _In_ PMO_CHAR Source,
    _In_ MO_UINTN SourceLength)
{
    // Handle zero length source byte sequence.
    if (!Source || !SourceLength)
    {
        return 0;
    }

    // Handle NULL character.
    if (!*Source)
    {
        if (Destination)
        {
            *Destination = MO_UNICODE_NULL;
            return 0;
        }
    }

    // Handle Unicode Basic Multilingual Plane (UCS-2) characters.
    if (Source[0] < 0x80)
    {
        if (Destination)
        {
            *Destination = Source[0];
        }
        return 1;
    }
    else if ((Source[0] & 0xE0) == 0xC0)
    {
        if ((SourceLength < 2) ||
            (Source[1] & 0xC0) != 0x80)
        {
            return -1;
        }
        if (Destination)
        {
            *Destination = (Source[0] & 0x1F) << 6;
            *Destination |= Source[1] & 0x3F;
        }
        return 2;
    }
    else if ((Source[0] & 0xF0) == 0xE0)
    {
        if ((SourceLength < 3) ||
            (Source[1] & 0xC0) != 0x80 ||
            (Source[2] & 0xC0) != 0x80)
        {
            return -1;
        }
        if (Destination)
        {
            *Destination = (Source[0] & 0x0F) << 12;
            *Destination |= (Source[1] & 0x3F) << 6;
            *Destination |= Source[2] & 0x3F;
        }
        return 3;
    }

    return -1;
}
