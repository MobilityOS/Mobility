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

#include <Mile.Helpers.CppBase.h>

#include <Mile.Project.Version.h>

#define MOBILITY_RETROVISOR_VERSION_UTF8_STRING \
    MILE_PROJECT_VERSION_UTF8_STRING " (Build " \
    MILE_PROJECT_MACRO_TO_UTF8_STRING(MILE_PROJECT_VERSION_BUILD) ")"

#include <mmsystem.h>
#pragma comment(lib, "Winmm.lib")

namespace
{
    constexpr MO_UINT32 PcSpeakerSampleRate = MO_UINT32_C(44100);
    constexpr MO_UINT32 PcSpeakerSampleCount = MO_UINT32_C(65536);

    HWAVEOUT g_PcSpeakerWaveOut = nullptr;
    WAVEHDR g_PcSpeakerWaveHeader = {};

    MO_UINT8 g_PcSpeakerSamples[PcSpeakerSampleCount] = {};

    MO_BOOL g_PcSpeakerAvailable = MO_FALSE;
    MO_BOOL g_PcSpeakerWaveHeaderPrepared = MO_FALSE;
    MO_BOOL g_PcSpeakerEnabled = MO_FALSE;

    MO_UINT16 g_PcSpeakerPitDivisor = 0;

    MO_BOOL MoRetroVPcSpeakerStopPlayback()
    {
        if (!g_PcSpeakerWaveHeaderPrepared)
        {
            return MO_TRUE;
        }

        if (::waveOutReset(g_PcSpeakerWaveOut) != MMSYSERR_NOERROR)
        {
            return MO_FALSE;
        }

        if (::waveOutUnprepareHeader(
            g_PcSpeakerWaveOut,
            &g_PcSpeakerWaveHeader,
            sizeof(g_PcSpeakerWaveHeader)) != MMSYSERR_NOERROR)
        {
            return MO_FALSE;
        }

        g_PcSpeakerWaveHeader = {};
        g_PcSpeakerWaveHeaderPrepared = MO_FALSE;

        return MO_TRUE;
    }
}

MO_EXTERN_C MO_VOID MOAPI MoRetroVPcSpeakerInitialize(
    MO_VOID)
{
    WAVEFORMATEX WaveFormat = {};

    WaveFormat.wFormatTag = WAVE_FORMAT_PCM;
    WaveFormat.nChannels = 1;
    WaveFormat.nSamplesPerSec = PcSpeakerSampleRate;
    WaveFormat.wBitsPerSample = 8;
    WaveFormat.nBlockAlign = 1;
    WaveFormat.nAvgBytesPerSec = PcSpeakerSampleRate;
    WaveFormat.cbSize = 0;

    if (::waveOutOpen(
        &g_PcSpeakerWaveOut,
        WAVE_MAPPER,
        &WaveFormat,
        0,
        0,
        CALLBACK_NULL) != MMSYSERR_NOERROR)
    {
        g_PcSpeakerWaveOut = nullptr;
        return;
    }

    g_PcSpeakerAvailable = MO_TRUE;
}

MO_EXTERN_C MO_VOID MOAPI MoRetroVPcSpeakerSetTone(
    _Mo_In_ MO_BOOL Enabled,
    _Mo_In_ MO_UINT16 PitDivisor)
{
    if (!g_PcSpeakerAvailable)
    {
        return;
    }

    MO_BOOL RequestedEnabled =
        Enabled && PitDivisor
        ? MO_TRUE
        : MO_FALSE;

    if (g_PcSpeakerEnabled == RequestedEnabled &&
        (!RequestedEnabled ||
            g_PcSpeakerPitDivisor == PitDivisor))
    {
        return;
    }

    if (!MoRetroVPcSpeakerStopPlayback())
    {
        g_PcSpeakerAvailable = MO_FALSE;
        return;
    }

    g_PcSpeakerEnabled = MO_FALSE;
    g_PcSpeakerPitDivisor = 0;

    if (!RequestedEnabled)
    {
        return;
    }

    for (MO_UINT32 Index = 0;
        Index < PcSpeakerSampleCount;
        ++Index)
    {
        g_PcSpeakerSamples[Index] =
            ((MO_UINT32_C(54) * Index / PitDivisor) & 1)
            ? MO_UINT8_MAX
            : 0;
    }

    g_PcSpeakerWaveHeader = {};
    g_PcSpeakerWaveHeader.lpData =
        reinterpret_cast<LPSTR>(g_PcSpeakerSamples);
    g_PcSpeakerWaveHeader.dwBufferLength =
        static_cast<DWORD>(sizeof(g_PcSpeakerSamples));
    g_PcSpeakerWaveHeader.dwLoops = MO_UINT32_MAX;

    if (::waveOutPrepareHeader(
        g_PcSpeakerWaveOut,
        &g_PcSpeakerWaveHeader,
        sizeof(g_PcSpeakerWaveHeader)) != MMSYSERR_NOERROR)
    {
        g_PcSpeakerAvailable = MO_FALSE;
        return;
    }

    g_PcSpeakerWaveHeaderPrepared = MO_TRUE;

    g_PcSpeakerWaveHeader.dwFlags |=
        WHDR_BEGINLOOP | WHDR_ENDLOOP;

    if (::waveOutWrite(
        g_PcSpeakerWaveOut,
        &g_PcSpeakerWaveHeader,
        sizeof(g_PcSpeakerWaveHeader)) != MMSYSERR_NOERROR)
    {
        if (::waveOutUnprepareHeader(
            g_PcSpeakerWaveOut,
            &g_PcSpeakerWaveHeader,
            sizeof(g_PcSpeakerWaveHeader)) == MMSYSERR_NOERROR)
        {
            g_PcSpeakerWaveHeader = {};
            g_PcSpeakerWaveHeaderPrepared = MO_FALSE;
        }

        g_PcSpeakerAvailable = MO_FALSE;
        return;
    }

    g_PcSpeakerEnabled = MO_TRUE;
    g_PcSpeakerPitDivisor = PitDivisor;
}

MO_EXTERN_C MO_VOID MOAPI MoRetroVPcSpeakerUninitialize(
    MO_VOID)
{
    if (!g_PcSpeakerWaveOut)
    {
        return;
    }

    ::waveOutReset(g_PcSpeakerWaveOut);

    if (g_PcSpeakerWaveHeaderPrepared)
    {
        ::waveOutUnprepareHeader(
            g_PcSpeakerWaveOut,
            &g_PcSpeakerWaveHeader,
            sizeof(g_PcSpeakerWaveHeader));
    }

    ::waveOutClose(g_PcSpeakerWaveOut);

    g_PcSpeakerWaveOut = nullptr;
    g_PcSpeakerWaveHeader = {};

    g_PcSpeakerAvailable = MO_FALSE;
    g_PcSpeakerWaveHeaderPrepared = MO_FALSE;
    g_PcSpeakerEnabled = MO_FALSE;
    g_PcSpeakerPitDivisor = 0;
}

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

    ::MoRetroVPcSpeakerInitialize();

    int Result = ::main_8086tiny(
        static_cast<int>(Arguments.size()),
        ArgumentPointers.data());

    ::MoRetroVPcSpeakerUninitialize();

    return Result;
}
