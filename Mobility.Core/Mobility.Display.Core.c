/*
 * PROJECT:    Mobility
 * FILE:       Mobility.Display.Core.c
 * PURPOSE:    Implementation for Mobility Display Core
 *
 * LICENSE:    The MIT License
 *
 * MAINTAINER: MouriNaruto (Kenji.Mouri@outlook.com)
 */

#include "Mobility.Display.Core.h"

EXTERN_C MO_BOOL MOAPI MoDisplayFrameBufferValidate(
    _In_ PMO_DISPLAY_BGRA32_FRAMEBUFFER DisplayFrameBuffer)
{
    if (!DisplayFrameBuffer || !DisplayFrameBuffer->FrameBufferBase)
    {
        // Null pointer is invalid.
        return MO_FALSE;
    }

    // Note: HorizontalResolution and VerticalResolution are MO_UINT32,
    if (!DisplayFrameBuffer->HorizontalResolution ||
        !DisplayFrameBuffer->VerticalResolution)
    {
        // Zero or negative resolution is invalid.
        return MO_FALSE;
    }


    if (DisplayFrameBuffer->PixelsPerScanLine
        < DisplayFrameBuffer->HorizontalResolution)
    {
        // PixelsPerScanLine must be greater than or equal to
        // HorizontalResolution.
        return MO_FALSE;
    }

    return MO_TRUE;
}
