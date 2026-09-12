/*
 * PROJECT:    Mobility
 * FILE:       stdarg.h
 * PURPOSE:    Definition for the C Standard Library <stdarg.h> header
 *
 * LICENSE:    The MIT License
 *
 * MAINTAINER: MouriNaruto (Kenji.Mouri@outlook.com)
 */

#ifndef MOBILITY_RUNTIME_STDARG
#define MOBILITY_RUNTIME_STDARG

#include <Mile.Mobility.Portable.Types.h>

#define __STDC_VERSION_STDARG_H__ 202311L

typedef MO_VARIABLE_ARGUMENT_LIST va_list;

#define va_start(ArgumentList, Parameter) \
    MO_VARIABLE_ARGUMENT_START(ArgumentList, Parameter)

#define va_arg(ArgumentList, Type) \
    MO_VARIABLE_ARGUMENT_READ(ArgumentList, Type)

#define va_copy(Destination, Source) \
    MO_VARIABLE_ARGUMENT_COPY(Destination, Source)

#define va_end(ArgumentList) \
    MO_VARIABLE_ARGUMENT_END(ArgumentList)

#endif // !MOBILITY_RUNTIME_STDARG
