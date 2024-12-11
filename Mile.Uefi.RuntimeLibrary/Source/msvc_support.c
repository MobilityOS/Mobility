/*
 * PROJECT:   Mouri Internal Library Essentials
 * FILE:      msvc_support.cpp
 * PURPOSE:   Implementation for MSVC support.
 *
 * LICENSE:   The MIT License
 *
 * DEVELOPER: Mouri_Naruto (Mouri_Naruto AT Outlook.com)
 */

// This is required to keep VC++ happy if you use floating-point
int _fltused = 1;
int __sse2_available = 0;   ///< Used by ftol2_sse
