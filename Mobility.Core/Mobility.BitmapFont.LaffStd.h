/*
 * PROJECT:    Mobility
 * FILE:       Mobility.BitmapFont.LaffStd.h
 * PURPOSE:    Definition for LaffStd, the UEFI console default font
 *
 * LICENSE:    The MIT License
 *
 * MAINTAINER: MouriNaruto (Kenji.Mouri@outlook.com)
 */

#ifndef MOBILITY_BITMAPFONT_LAFFSTD
#define MOBILITY_BITMAPFONT_LAFFSTD

#include <Mile.Mobility.Portable.Types.h>

/**
 * @brief Gets the width of the LaffStd font.
 * @return The width of the LaffStd font.
 */
MO_EXTERN_C MO_UINT8 MOAPI MoBitmapFontLaffStdGetWidth();

/**
 * @brief Gets the height of the LaffStd font.
 * @return The height of the LaffStd font.
 */
MO_EXTERN_C MO_UINT8 MOAPI MoBitmapFontLaffStdGetHeight();

/**
 * @brief Queries the glyph data of the LaffStd font.
 * @param Character The character to query.
 * @return The glyph data of the LaffStd font. If the character is not found,
 *         the return value is nullptr.
 */
MO_EXTERN_C MO_CONST PMO_UINT8 MOAPI MoBitmapFontLaffStdQueryGlyph(
    _Mo_In_ MO_WIDE_CHAR Character);

#endif // !MOBILITY_BITMAPFONT_LAFFSTD
