/*
 * PROJECT:    Mobility
 * FILE:       Mobility.Graphics.Core.h
 * PURPOSE:    Definition for Mobility Graphics Core
 *
 * LICENSE:    The MIT License
 *
 * MAINTAINER: MouriNaruto (Kenji.Mouri@outlook.com)
 */

#ifndef MOBILITY_GRAPHICS_CORE
#define MOBILITY_GRAPHICS_CORE

#include <Mile.Mobility.Portable.Types.h>

/**
 * @brief Describes a signed pixel position relative to a graphics surface.
 *        Negative coordinates may represent positions outside the surface.
 */
typedef struct _MO_GRAPHICS_POINT
{
    /**
     * @brief The horizontal position in pixels.
     */
    MO_INT16 X;
    /**
     * @brief The vertical position in pixels.
     */
    MO_INT16 Y;
} MO_GRAPHICS_POINT, *PMO_GRAPHICS_POINT;

/**
 * @brief Describes a width and height in pixels. Negative dimensions are
 *        invalid. Zero-sized regions are interpreted by the operation using
 *        this size. Positive dimensions range from 1 to 32767.
 */
typedef struct _MO_GRAPHICS_SIZE
{
    /**
     * @brief The width in pixels.
     */
    MO_INT16 Width;
    /**
     * @brief The height in pixels.
     */
    MO_INT16 Height;
} MO_GRAPHICS_SIZE, *PMO_GRAPHICS_SIZE;

/**
 * @brief Describes a fixed-cell monochrome bitmap font. Glyph rows are stored
 *        from top to bottom, using (Size.Width + 7) / 8 bytes per row
 *        without extra padding. The most significant bit represents the
 *        leftmost pixel. Unused bits in the last byte are ignored.
 */
typedef struct _MO_GRAPHICS_BITMAP_FONT
{
    /**
     * @brief The size of each glyph cell in pixels. Both dimensions must be
     *        positive.
     */
    MO_GRAPHICS_SIZE Size;
    /**
     * @brief Queries the glyph bitmap for a character. This pointer must not
     *        be nullptr.
     * @param Character The character to query.
     * @return A pointer to static read-only glyph data, or nullptr if the
     *         character is not supported.
     */
    MO_CONST MO_UINT8* (MOAPI* Query)(
        _Mo_In_ MO_WIDE_CHAR Character);
} MO_GRAPHICS_BITMAP_FONT, *PMO_GRAPHICS_BITMAP_FONT;

/**
 * @brief Describes a top-down framebuffer in the current address space. Pixel
 *        format and access requirements are defined by the operation using this
 *        framebuffer. The caller validates the backing span and address
 *        calculations.
 */
typedef struct _MO_GRAPHICS_FRAMEBUFFER
{
    /**
     * @brief The base address of the frame buffer. It must not be nullptr
     *        for a valid framebuffer.
     */
    MO_POINTER Base;
    /**
     * @brief The visible size in pixels. Both dimensions must be positive.
     */
    MO_GRAPHICS_SIZE Size;
    /**
     * @brief The byte offset between the starts of adjacent rows. It must
     *        cover at least the bytes occupied by one visible row.
     */
    MO_UINT32 Pitch;
} MO_GRAPHICS_FRAMEBUFFER, *PMO_GRAPHICS_FRAMEBUFFER;

#endif // !MOBILITY_GRAPHICS_CORE
