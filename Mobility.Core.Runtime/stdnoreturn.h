/*
 * PROJECT:    Mobility
 * FILE:       stdnoreturn.h
 * PURPOSE:    Definition for the C Standard Library <stdnoreturn.h> header
 *
 * LICENSE:    The MIT License
 *
 * MAINTAINER: MouriNaruto (Kenji.Mouri@outlook.com)
 */

#ifndef MOBILITY_RUNTIME_STDNORETURN
#define MOBILITY_RUNTIME_STDNORETURN

#ifndef __cplusplus
#if defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 202311L)
#define noreturn [[noreturn]]
#elif defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 201112L)
#define noreturn _Noreturn
#elif defined(_MSC_VER)
#define noreturn __declspec(noreturn)
#elif defined(__GNUC__) || defined(__clang__)
#define noreturn __attribute__((noreturn))
#else
#define noreturn
#endif
#endif

#endif // !MOBILITY_RUNTIME_STDNORETURN
