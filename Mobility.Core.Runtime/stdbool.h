/*
 * PROJECT:    Mobility
 * FILE:       stdbool.h
 * PURPOSE:    Definition for the C Standard Library <stdbool.h> header
 *
 * LICENSE:    The MIT License
 *
 * MAINTAINER: MouriNaruto (Kenji.Mouri@outlook.com)
 */

#ifndef MOBILITY_RUNTIME_STDBOOL
#define MOBILITY_RUNTIME_STDBOOL

#include <Mile.Mobility.Portable.Types.h>

#if !defined(__cplusplus) && \
    (!defined(__STDC_VERSION__) || (__STDC_VERSION__ < 202311L))
#define bool MO_BOOL
#define true MO_TRUE
#define false MO_FALSE
#endif

#define __bool_true_false_are_defined 1

#endif // !MOBILITY_RUNTIME_STDBOOL
