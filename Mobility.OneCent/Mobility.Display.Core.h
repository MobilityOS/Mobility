/*
 * PROJECT:   Mobility
 * FILE:      Mobility.Display.Core.h
 * PURPOSE:   Definition for Mobility Display Core
 *
 * LICENSE:   The MIT License
 *
 * MAINTAINER: MouriNaruto (Kenji.Mouri@outlook.com)
 */

#ifndef MOBILITY_DISPLAY_CORE
#define MOBILITY_DISPLAY_CORE

#define MILE_MOBILITY_ENABLE_MINIMUM_SAL
#include <Mile.Mobility.Portable.Types.h>

/**
 * @brief Contains the information of the BGRA32 frame buffer.
 */
typedef struct _MO_DISPLAY_BGRA32_FRAMEBUFFER
{
    PMO_UINT32 FrameBufferBase;
    MO_UINT32 HorizontalResolution;
    MO_UINT32 VerticalResolution;
} MO_DISPLAY_BGRA32_FRAMEBUFFER, *PMO_DISPLAY_BGRA32_FRAMEBUFFER;

#endif // !MOBILITY_DISPLAY_CORE
