/*
 * PROJECT:    Mobility
 * FILE:       stdalign.h
 * PURPOSE:    Definition for the C Standard Library <stdalign.h> header
 *
 * LICENSE:    The MIT License
 *
 * MAINTAINER: MouriNaruto (Kenji.Mouri@outlook.com)
 */

#ifndef MOBILITY_RUNTIME_STDALIGN
#define MOBILITY_RUNTIME_STDALIGN

#include <Mile.Mobility.Portable.Types.h>

#if !defined(__cplusplus) && \
    (!defined(__STDC_VERSION__) || (__STDC_VERSION__ < 202311L))
#define alignas MO_DECLSPEC_ALIGN
#if defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 201112L)
#define alignof _Alignof
#elif defined(_MSC_VER)
#define alignof __alignof
#elif defined(__GNUC__) || defined(__clang__)
#define alignof __alignof__
#endif
#endif

#define __alignas_is_defined 1
#define __alignof_is_defined 1

#endif // !MOBILITY_RUNTIME_STDALIGN
