/*
 * PROJECT:    Mobility
 * FILE:       Mobility.Display.Core.h
 * PURPOSE:    Definition for Mobility Display Core
 *
 * LICENSE:    The MIT License
 *
 * MAINTAINER: MouriNaruto (Kenji.Mouri@outlook.com)
 */

#ifndef MOBILITY_DISPLAY_CORE
#define MOBILITY_DISPLAY_CORE

#define MILE_MOBILITY_ENABLE_MINIMUM_SAL
#include <Mile.Mobility.Portable.Types.h>

#define MO_DISPLAY_BGRA32_MAKE(R, G, B, A) ((MO_UINT32)( \
    (((MO_UINT32)((MO_UINT8)(R))) << 16) | \
    (((MO_UINT32)((MO_UINT8)(G))) << 8) | \
    (((MO_UINT32)((MO_UINT8)(B))) << 0) | \
    (((MO_UINT32)((MO_UINT8)(A))) << 24)))

#define MO_DISPLAY_BGRA32_GET_RED(BGRA32) ((MO_UINT8)((BGRA32) >> 16))
#define MO_DISPLAY_BGRA32_GET_GREEN(BGRA32) ((MO_UINT8)((BGRA32) >> 8))
#define MO_DISPLAY_BGRA32_GET_BLUE(BGRA32) ((MO_UINT8)((BGRA32) >> 0))
#define MO_DISPLAY_BGRA32_GET_ALPHA(BGRA32) ((MO_UINT8)((BGRA32) >> 24))

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
