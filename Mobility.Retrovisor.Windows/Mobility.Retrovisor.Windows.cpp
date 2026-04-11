/*
 * PROJECT:    Mobility.Retrovisor.Windows
 * FILE:       Mobility.Retrovisor.Windows.cpp
 * PURPOSE:    Implementation for Mobility.Retrovisor.Windows
 *
 * LICENSE:    The MIT License
 *
 * MAINTAINER: MouriNaruto (Kenji.Mouri@outlook.com)
 */

#include <clocale>
#include <cstdio>
#include <cwchar>

int main()
{
    // I can't use that because of the limitation in VC-LTL.
    // std::setlocale(LC_ALL, "zh_CN.UTF-8");

    std::setlocale(LC_ALL, "chs");

    std::wprintf(
        L"Mobility.Retrovisor.Windows\n"
        L"================================================================\n"
        L"Hello World!\n");

    return 0;
}
