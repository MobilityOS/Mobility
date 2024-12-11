/*
 * PROJECT:   Mouri Internal Library Essentials
 * FILE:      stdbool.h
 * PURPOSE:   The C Standard Library <stdbool.h> header.
 *
 * LICENSE:   The MIT License
 *
 * DEVELOPER: Mouri_Naruto (Mouri_Naruto AT Outlook.com)
 */

#ifndef MILE_UEFI_STDBOOL
#define MILE_UEFI_STDBOOL

#define __bool_true_false_are_defined	1

#ifndef __cplusplus

#define bool	_Bool
#define false	0
#define true	1

#endif // __cplusplus

#endif // !MILE_UEFI_STDBOOL
