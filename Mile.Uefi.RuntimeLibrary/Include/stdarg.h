/*
 * PROJECT:    Mouri Internal Library Essentials
 * FILE:       stdarg.h
 * PURPOSE:    The C Standard Library <stdarg.h> header.
 *
 * LICENSE:    The MIT License
 *
 * MAINTAINER: MouriNaruto (Kenji.Mouri@outlook.com)
 */

#ifndef MILE_UEFI_STDARG
#define MILE_UEFI_STDARG

#include <Uefi.h>

#define va_list VA_LIST

#define va_start VA_START
#define va_arg VA_ARG
#define va_end VA_END
#define va_copy VA_COPY

#endif // !MILE_UEFI_STDARG
