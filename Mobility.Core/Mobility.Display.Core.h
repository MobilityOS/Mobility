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
    /**
     * @brief The base address of the frame buffer.
     */
    PMO_UINT32 FrameBufferBase;
    /**
     * @brief The size of the frame buffer in pixels in the X dimension.
     */
    MO_UINT32 HorizontalResolution;
    /**
     * @brief The size of the frame buffer in pixels in the Y dimension.
     */
    MO_UINT32 VerticalResolution;
    /**
     * @brief The number of pixel elements per video memory line.
     */
    MO_UINT32 PixelsPerScanLine;
} MO_DISPLAY_BGRA32_FRAMEBUFFER, *PMO_DISPLAY_BGRA32_FRAMEBUFFER;

#define MO_DISPLAY_BGRA32_FRAMEBUFFER_GET_PIXEL_OFFSET(FrameBuffer, X, Y) ( \
    (((MO_UINTN)(Y)) * (FrameBuffer)->PixelsPerScanLine) + (X))
#define MO_DISPLAY_BGRA32_FRAMEBUFFER_GET_PIXEL_ADDRESS(FrameBuffer, X, Y) ( \
    (FrameBuffer)->FrameBufferBase \
    + MO_DISPLAY_BGRA32_FRAMEBUFFER_GET_PIXEL_OFFSET(FrameBuffer, X, Y))
#define MO_DISPLAY_BGRA32_FRAMEBUFFER_GET_PIXEL_COUNT(FrameBuffer) ( \
    ((MO_UINTN)(FrameBuffer)->HorizontalResolution) \
    * (FrameBuffer)->VerticalResolution)
#define MO_DISPLAY_BGRA32_FRAMEBUFFER_GET_SIZE_IN_BYTES(FrameBuffer) ( \
    ((MO_UINTN)(FrameBuffer)->PixelsPerScanLine) \
    * (FrameBuffer)->VerticalResolution \
    * sizeof(MO_UINT32))

/**
 * @brief Validates a BGRA32 frame buffer structure.
 * @param DisplayFrameBuffer The pointer to the BGRA32 frame buffer structure to
 *                           be validated.
 * @return If the BGRA32 frame buffer structure is valid, the return value is
 *         MO_TRUE. Otherwise, the return value is MO_FALSE.
 */
EXTERN_C MO_BOOL MOAPI MoDisplayFrameBufferValidate(
    _In_ PMO_DISPLAY_BGRA32_FRAMEBUFFER DisplayFrameBuffer);

#endif // !MOBILITY_DISPLAY_CORE
