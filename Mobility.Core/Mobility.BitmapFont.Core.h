/*
 * PROJECT:    Mobility
 * FILE:       Mobility.BitmapFont.Core.h
 * PURPOSE:    Definition for Mobility Bitmap Font Core
 *
 * LICENSE:    The MIT License
 *
 * MAINTAINER: MouriNaruto (Kenji.Mouri@outlook.com)
 */

#ifndef MOBILITY_BITMAPFONT_CORE
#define MOBILITY_BITMAPFONT_CORE

#include <Mile.Mobility.Portable.Types.h>

/**
 * @brief Describes a fixed-cell monochrome bitmap font. Glyph rows are stored
 *        from top to bottom, using (Width + 7) / 8 bytes per row without extra
 *        padding. The most significant bit represents the leftmost pixel.
 *        Unused bits in the last byte are ignored.
 */
typedef struct _MO_BITMAP_FONT
{
    /**
     * @brief The width of each glyph cell in pixels. It must be nonzero.
     */
    MO_UINT16 Width;
    /**
     * @brief The height of each glyph cell in pixels. It must be nonzero.
     */
    MO_UINT16 Height;
    /**
     * @brief Queries the glyph bitmap for a character. This pointer must not
     *        be nullptr.
     * @param Character The character to query.
     * @return A pointer to static read-only glyph data, or nullptr if
     *         the character is not supported.
     */
    MO_CONST MO_UINT8* (MOAPI* Query)(
        _Mo_In_ MO_WIDE_CHAR Character);
} MO_BITMAP_FONT, *PMO_BITMAP_FONT;

#endif // !MOBILITY_BITMAPFONT_CORE
