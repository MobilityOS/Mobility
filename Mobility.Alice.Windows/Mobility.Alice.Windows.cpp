/*
 * PROJECT:    Mobility.Alice.Windows
 * FILE:       Mobility.Alice.Windows.cpp
 * PURPOSE:    Implementation for Mobility.Alice.Windows
 *
 * LICENSE:    The MIT License
 *
 * MAINTAINER: MouriNaruto (Kenji.Mouri@outlook.com)
 */

#include <Mile.Mobility.Portable.Types.h>
#ifndef MILE_MOBILITY_ENABLE_MINIMUM_SAL
#include <sal.h>
#endif // !MILE_MOBILITY_ENABLE_MINIMUM_SAL

// Workaround for Windows SDK, which defines these types in #ifndef VOID block.
// That design is terrible, but needs to have a workaround.
#ifdef VOID
typedef char CHAR;
typedef short SHORT;
typedef long LONG;
#endif // VOID

#include <Mile.Helpers.CppBase.h>

#include <Mile.Project.Version.h>

#define MOBILITY_ALICE_VERSION_UTF8_STRING \
    MILE_PROJECT_VERSION_UTF8_STRING " (Build " \
    MILE_PROJECT_MACRO_TO_UTF8_STRING(MILE_PROJECT_VERSION_BUILD) ")"

#include <cstdio>

int main()
{
    std::printf(
        "Mobility Alice for Windows (Development Use)"
        " " MOBILITY_ALICE_VERSION_UTF8_STRING "\r\n"
        "(c) Kenji Mouri. All rights reserved.\r\n"
        "\r\n");

    std::printf("Under construction.\r\n");

    std::getchar();

    return 0;
}
