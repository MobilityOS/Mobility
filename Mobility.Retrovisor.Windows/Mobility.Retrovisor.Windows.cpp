/*
 * PROJECT:    Mobility.Retrovisor.Windows
 * FILE:       Mobility.Retrovisor.Windows.cpp
 * PURPOSE:    Implementation for Mobility.Retrovisor.Windows
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

#define MOBILITY_RETROVISOR_VERSION_UTF8_STRING \
    MILE_PROJECT_VERSION_UTF8_STRING " (Build " \
    MILE_PROJECT_MACRO_TO_UTF8_STRING(MILE_PROJECT_VERSION_BUILD) ")"

extern "C" int main_8086tiny(int argc, char** argv);

int main()
{
    std::printf(
        "Mobility Retrovisor for Windows (Development Use)"
        " " MOBILITY_RETROVISOR_VERSION_UTF8_STRING "\r\n"
        "(c) Kenji Mouri. All rights reserved.\r\n"
        "\r\n");

    std::vector<std::string> Arguments = Mile::SplitCommandLineString(
        Mile::ToString(CP_UTF8, ::GetCommandLineW()));

    std::vector<char*> ArgumentPointers;
    for (auto& Argument : Arguments)
    {
        ArgumentPointers.push_back(Argument.data());
    }
    ArgumentPointers.push_back(nullptr);

    return ::main_8086tiny(
        static_cast<int>(Arguments.size()),
        ArgumentPointers.data());
}
