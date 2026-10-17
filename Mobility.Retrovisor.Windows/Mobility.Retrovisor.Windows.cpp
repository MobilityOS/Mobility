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

#include <Mobility.Display.Core.h>

#include <mutex>

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

        if (MMSYSERR_NOERROR != ::waveOutReset(g_PcSpeakerWaveOut))
        {
            return MO_FALSE;
        }

        if (MMSYSERR_NOERROR != ::waveOutUnprepareHeader(
            g_PcSpeakerWaveOut,
            &g_PcSpeakerWaveHeader,
            sizeof(g_PcSpeakerWaveHeader)))
        {
            return MO_FALSE;
        }

        g_PcSpeakerWaveHeader = {};
        g_PcSpeakerWaveHeaderPrepared = MO_FALSE;

        return MO_TRUE;
    }
}

MO_EXTERN_C MO_VOID MOAPI MoRetroVPcSpeakerInitialize(MO_VOID)
{
    WAVEFORMATEX WaveFormat = {};

    WaveFormat.wFormatTag = WAVE_FORMAT_PCM;
    WaveFormat.nChannels = 1;
    WaveFormat.nSamplesPerSec = PcSpeakerSampleRate;
    WaveFormat.wBitsPerSample = 8;
    WaveFormat.nBlockAlign = 1;
    WaveFormat.nAvgBytesPerSec = PcSpeakerSampleRate;
    WaveFormat.cbSize = 0;

    if (MMSYSERR_NOERROR != ::waveOutOpen(
        &g_PcSpeakerWaveOut,
        WAVE_MAPPER,
        &WaveFormat,
        0,
        0,
        CALLBACK_NULL))
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

    MO_BOOL RequestedEnabled = Enabled && PitDivisor ? MO_TRUE : MO_FALSE;

    if (g_PcSpeakerEnabled == RequestedEnabled &&
        (!RequestedEnabled || g_PcSpeakerPitDivisor == PitDivisor))
    {
        return;
    }

    if (!::MoRetroVPcSpeakerStopPlayback())
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

    for (MO_UINT32 Index = 0; Index < PcSpeakerSampleCount; ++Index)
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

    if (MMSYSERR_NOERROR != ::waveOutPrepareHeader(
        g_PcSpeakerWaveOut,
        &g_PcSpeakerWaveHeader,
        sizeof(g_PcSpeakerWaveHeader)))
    {
        g_PcSpeakerAvailable = MO_FALSE;
        return;
    }

    g_PcSpeakerWaveHeaderPrepared = MO_TRUE;

    g_PcSpeakerWaveHeader.dwFlags |= WHDR_BEGINLOOP | WHDR_ENDLOOP;

    if (MMSYSERR_NOERROR != ::waveOutWrite(
        g_PcSpeakerWaveOut,
        &g_PcSpeakerWaveHeader,
        sizeof(g_PcSpeakerWaveHeader)))
    {
        if (MMSYSERR_NOERROR == ::waveOutUnprepareHeader(
            g_PcSpeakerWaveOut,
            &g_PcSpeakerWaveHeader,
            sizeof(g_PcSpeakerWaveHeader)))
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

MO_EXTERN_C MO_VOID MOAPI MoRetroVPcSpeakerUninitialize(MO_VOID)
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

namespace
{
    constexpr wchar_t DisplayWindowClassName[] =
        L"Mobility.Retrovisor.Windows.Display";

    HANDLE g_DisplayThread = nullptr;
    HANDLE g_DisplayReadyEvent = nullptr;
    HWND g_DisplayWindow = nullptr;

    std::vector<MO_UINT32> g_DisplayPixels;
    MO_DISPLAY_BGRA32_FRAMEBUFFER g_DisplayFrameBuffer = {};
    BITMAPINFO g_DisplayBitmapInformation = {};

    constexpr MO_UINT32 KeyboardQueueCapacity = MO_UINT32_C(256);

    std::mutex g_KeyboardQueueLock;

    MO_UINT16 g_KeyboardQueue[KeyboardQueueCapacity] = {};

    MO_UINT16 g_PressedKeys[256] = {};

    MO_UINT32 g_KeyboardQueueReadIndex = 0;
    MO_UINT32 g_KeyboardQueueWriteIndex = 0;

    MO_VOID MoRetroVDisplayClearKeyboardQueue()
    {
        std::lock_guard<std::mutex> LockGuard(g_KeyboardQueueLock);

        g_KeyboardQueueReadIndex = 0;
        g_KeyboardQueueWriteIndex = 0;

        for (MO_UINT32 Index = 0; Index < 256; ++Index)
        {
            g_PressedKeys[Index] = 0;
        }
    }

    MO_VOID MoRetroVDisplayEnqueueKey(
        _Mo_In_ MO_UINT16 KeyEvent)
    {
        std::lock_guard<std::mutex> LockGuard(g_KeyboardQueueLock);

        MO_UINT32 NextWriteIndex =
            (g_KeyboardQueueWriteIndex + 1) & (KeyboardQueueCapacity - 1);

        if (NextWriteIndex != g_KeyboardQueueReadIndex)
        {
            g_KeyboardQueue[g_KeyboardQueueWriteIndex] = KeyEvent;
            g_KeyboardQueueWriteIndex = NextWriteIndex;
        }
    }

    MO_UINT16 MoRetroVDisplayTranslateKey(
        _In_ WPARAM wParam,
        _In_ LPARAM lParam)
    {
        UINT VirtualKey = static_cast<UINT>(wParam);
        UINT ScanCode = static_cast<UINT>((lParam >> 16) & 0xFF);

        if (VirtualKey >= VK_NUMPAD0 && VirtualKey <= VK_NUMPAD9)
        {
            return static_cast<MO_UINT16>(256 + VirtualKey - VK_NUMPAD0);
        }

        if (VirtualKey >= VK_F1 && VirtualKey <= VK_F12)
        {
            return static_cast<MO_UINT16>(282 + VirtualKey - VK_F1);
        }

        switch (VirtualKey)
        {
        case VK_BACK:
            return 8;
        case VK_TAB:
            return 9;
        case VK_CLEAR:
            return 12;
        case VK_RETURN:
            return (lParam & (1 << 24)) ? 271 : 13;
        case VK_PAUSE:
            return 19;
        case VK_ESCAPE:
            return 27;
        case VK_SPACE:
            return 32;
        case VK_DELETE:
            return 127;
        case VK_DECIMAL:
            return 266;
        case VK_DIVIDE:
            return 267;
        case VK_MULTIPLY:
            return 268;
        case VK_SUBTRACT:
            return 269;
        case VK_ADD:
            return 270;
        case VK_UP:
            return 273;
        case VK_DOWN:
            return 274;
        case VK_RIGHT:
            return 275;
        case VK_LEFT:
            return 276;
        case VK_INSERT:
            return 277;
        case VK_HOME:
            return 278;
        case VK_END:
            return 279;
        case VK_PRIOR:
            return 280;
        case VK_NEXT:
            return 281;
        case VK_NUMLOCK:
            return 300;
        case VK_CAPITAL:
            return 301;
        case VK_SCROLL:
            return 302;
        case VK_RSHIFT:
            return 303;
        case VK_LSHIFT:
            return 304;
        case VK_SHIFT:
            return VK_RSHIFT == ::MapVirtualKeyW(ScanCode, MAPVK_VSC_TO_VK_EX)
                ? 303
                : 304;
        case VK_RCONTROL:
            return 305;
        case VK_LCONTROL:
            return 306;
        case VK_CONTROL:
            return (lParam & (1 << 24)) ? 305 : 306;
        case VK_RMENU:
            return 307;
        case VK_LMENU:
            return 308;
        case VK_MENU:
            return (lParam & (1 << 24)) ? 307 : 308;
        case VK_SNAPSHOT:
            return 316;
        case VK_CANCEL:
            return 318;
        case VK_APPS:
            return 319;
        default:
            break;
        }

        BYTE KeyboardState[256] = {};
        if (!::GetKeyboardState(KeyboardState))
        {
            return 0;
        }

        WCHAR Characters[4] = {};
        int CharacterCount = ::ToUnicodeEx(
            VirtualKey,
            ScanCode,
            KeyboardState,
            Characters,
            static_cast<int>(MO_ARRAY_SIZE(Characters)),
            0,
            ::GetKeyboardLayout(0));

        if (1 == CharacterCount && Characters[0] > 0 && Characters[0] <= 0x7F)
        {
            return static_cast<MO_UINT16>(Characters[0]);
        }

        return 0;
    }

    MO_VOID MoRetroVDisplayProcessKey(
        _In_ UINT uMsg,
        _In_ WPARAM wParam,
        _In_ LPARAM lParam)
    {
        MO_BOOL Released = MO_FALSE;
        if (WM_KEYUP == uMsg || WM_SYSKEYUP == uMsg)
        {
            Released = MO_TRUE;
        }

        MO_UINT32 VirtualKey = static_cast<MO_UINT32>(wParam) & 0xFF;

        MO_UINT16 KeyCode = 0;
        if (Released)
        {
            KeyCode = g_PressedKeys[VirtualKey];
            if (!KeyCode)
            {
                KeyCode = ::MoRetroVDisplayTranslateKey(wParam, lParam);
            }
            g_PressedKeys[VirtualKey] = 0;
        }
        else
        {
            KeyCode = ::MoRetroVDisplayTranslateKey(wParam, lParam);
            g_PressedKeys[VirtualKey] = KeyCode;
        }
        if (!KeyCode)
        {
            return;
        }

        MO_UINT16 KeyEvent = static_cast<MO_UINT16>(0x0400 | KeyCode);
        if (::GetKeyState(VK_MENU) & 0x8000)
        {
            KeyEvent |= 0x0800;
        }
        if (::GetKeyState(VK_SHIFT) & 0x8000)
        {
            KeyEvent |= 0x1000;
        }
        if (::GetKeyState(VK_CONTROL) & 0x8000)
        {
            KeyEvent |= 0x2000;
        }
        if (Released)
        {
            KeyEvent |= 0x4000;
        }

        ::MoRetroVDisplayEnqueueKey(KeyEvent);
    }

    LRESULT CALLBACK MoRetroVDisplayWindowProcedure(
        _In_ HWND hWnd,
        _In_ UINT uMsg,
        _In_ WPARAM wParam,
        _In_ LPARAM lParam)
    {
        switch (uMsg)
        {
        case WM_KEYDOWN:
        case WM_KEYUP:
        case WM_SYSKEYDOWN:
        case WM_SYSKEYUP:
            ::MoRetroVDisplayProcessKey(uMsg, wParam, lParam);
            return 0;
        case WM_CHAR:
        case WM_SYSCHAR:
            return 0;
        case WM_PAINT:
        {
            PAINTSTRUCT PaintStructure = {};
            HDC DeviceContext = ::BeginPaint(hWnd, &PaintStructure);

            ::StretchDIBits(
                DeviceContext,
                0,
                0,
                static_cast<int>(
                    g_DisplayFrameBuffer.HorizontalResolution),
                static_cast<int>(
                    g_DisplayFrameBuffer.VerticalResolution),
                0,
                0,
                static_cast<int>(
                    g_DisplayFrameBuffer.HorizontalResolution),
                static_cast<int>(
                    g_DisplayFrameBuffer.VerticalResolution),
                g_DisplayFrameBuffer.FrameBufferBase,
                &g_DisplayBitmapInformation,
                DIB_RGB_COLORS,
                SRCCOPY);

            ::EndPaint(hWnd, &PaintStructure);

            return 0;
        }
        case WM_ERASEBKGND:
            return 1;
        case WM_CLOSE:
            ::DestroyWindow(hWnd);
            return 0;
        case WM_DESTROY:
            ::PostQuitMessage(0);
            return 0;
        default:
            break;
        }

        return ::DefWindowProcW(hWnd, uMsg, wParam, lParam);
    }

    DWORD WINAPI MoRetroVDisplayThreadProcedure(
        _In_ LPVOID lpThreadParameter)
    {
        UNREFERENCED_PARAMETER(lpThreadParameter);

        HINSTANCE InstanceHandle = ::GetModuleHandleW(nullptr);

        WNDCLASSEXW WindowClass = {};
        WindowClass.cbSize = sizeof(WindowClass);
        WindowClass.style = CS_HREDRAW | CS_VREDRAW;
        WindowClass.lpfnWndProc = ::MoRetroVDisplayWindowProcedure;
        WindowClass.hInstance = InstanceHandle;
        WindowClass.hCursor = ::LoadCursorW(nullptr, IDC_ARROW);
        WindowClass.hbrBackground =
            reinterpret_cast<HBRUSH>(::GetStockObject(BLACK_BRUSH));
        WindowClass.lpszClassName = DisplayWindowClassName;

        ATOM WindowClassAtom = ::RegisterClassExW(&WindowClass);

        if (!WindowClassAtom)
        {
            ::SetEvent(g_DisplayReadyEvent);
            return 0;
        }

        DWORD WindowStyle = WS_OVERLAPPED | WS_CAPTION;

        RECT WindowRectangle = {};
        WindowRectangle.left = 0;
        WindowRectangle.top = 0;
        WindowRectangle.right = static_cast<LONG>(
            g_DisplayFrameBuffer.HorizontalResolution);
        WindowRectangle.bottom = static_cast<LONG>(
            g_DisplayFrameBuffer.VerticalResolution);

        ::AdjustWindowRect(&WindowRectangle, WindowStyle, FALSE);

        g_DisplayWindow = ::CreateWindowExW(
            0,
            DisplayWindowClassName,
            L"Mobility Retrovisor for Windows",
            WindowStyle,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            WindowRectangle.right - WindowRectangle.left,
            WindowRectangle.bottom - WindowRectangle.top,
            nullptr,
            nullptr,
            InstanceHandle,
            nullptr);

        if (!g_DisplayWindow)
        {
            ::SetEvent(g_DisplayReadyEvent);
            ::UnregisterClassW(DisplayWindowClassName, InstanceHandle);
            return 0;
        }

        ::ShowWindow(g_DisplayWindow, SW_SHOWNORMAL);
        ::UpdateWindow(g_DisplayWindow);

        ::SetEvent(g_DisplayReadyEvent);

        MSG Message = {};
        while (::GetMessageW(&Message, nullptr, 0, 0) > 0)
        {
            ::TranslateMessage(&Message);
            ::DispatchMessageW(&Message);
        }

        g_DisplayWindow = nullptr;

        ::UnregisterClassW(DisplayWindowClassName, InstanceHandle);

        return 0;
    }
}

MO_EXTERN_C MO_VOID MOAPI MoRetroVDisplayCreate(
    _Mo_In_ MO_UINT32 HorizontalResolution,
    _Mo_In_ MO_UINT32 VerticalResolution,
    _Mo_Out_ PMO_DISPLAY_BGRA32_FRAMEBUFFER FrameBuffer)
{
    ::MoRetroVDisplayClearKeyboardQueue();

    g_DisplayPixels.assign(
        static_cast<MO_UINTN>(HorizontalResolution) *
        VerticalResolution,
        MO_DISPLAY_BGRA32_MAKE(0, 0, 0, MO_UINT8_MAX));

    g_DisplayFrameBuffer.FrameBufferBase = g_DisplayPixels.data();
    g_DisplayFrameBuffer.HorizontalResolution = HorizontalResolution;
    g_DisplayFrameBuffer.VerticalResolution = VerticalResolution;
    g_DisplayFrameBuffer.PixelsPerScanLine = HorizontalResolution;

    g_DisplayBitmapInformation = {};
    g_DisplayBitmapInformation.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    g_DisplayBitmapInformation.bmiHeader.biWidth =
        static_cast<LONG>(HorizontalResolution);
    g_DisplayBitmapInformation.bmiHeader.biHeight =
        -static_cast<LONG>(VerticalResolution);
    g_DisplayBitmapInformation.bmiHeader.biPlanes = 1;
    g_DisplayBitmapInformation.bmiHeader.biBitCount = 32;
    g_DisplayBitmapInformation.bmiHeader.biCompression = BI_RGB;

    g_DisplayReadyEvent = ::CreateEventW(nullptr, FALSE, FALSE, nullptr);

    if (g_DisplayReadyEvent)
    {
        g_DisplayThread = ::MileCreateThread(
            nullptr,
            0,
            ::MoRetroVDisplayThreadProcedure,
            nullptr,
            0,
            nullptr);

        if (g_DisplayThread)
        {
            ::WaitForSingleObject(g_DisplayReadyEvent, INFINITE);
        }

        ::CloseHandle(g_DisplayReadyEvent);
        g_DisplayReadyEvent = nullptr;
    }

    *FrameBuffer = g_DisplayFrameBuffer;
}

MO_EXTERN_C MO_VOID MOAPI MoRetroVDisplayPresent(MO_VOID)
{
    HWND WindowHandle = g_DisplayWindow;
    if (WindowHandle)
    {
        ::InvalidateRect(WindowHandle, nullptr, FALSE);
    }
}

MO_EXTERN_C MO_BOOL MOAPI MoRetroVDisplayReadKey(
    _Mo_Out_ PMO_UINT16 KeyEvent)
{
    std::lock_guard<std::mutex> LockGuard(g_KeyboardQueueLock);

    if (g_KeyboardQueueReadIndex == g_KeyboardQueueWriteIndex)
    {
        return MO_FALSE;
    }

    *KeyEvent = g_KeyboardQueue[g_KeyboardQueueReadIndex];

    g_KeyboardQueueReadIndex =
        (g_KeyboardQueueReadIndex + 1) & (KeyboardQueueCapacity - 1);

    return MO_TRUE;
}

MO_EXTERN_C MO_VOID MOAPI MoRetroVDisplayDestroy(MO_VOID)
{
    if (g_DisplayThread)
    {
        HWND WindowHandle = g_DisplayWindow;
        if (WindowHandle)
        {
            ::PostMessageW(WindowHandle, WM_CLOSE, 0, 0);
        }

        ::WaitForSingleObject(g_DisplayThread, INFINITE);

        ::CloseHandle(g_DisplayThread);
        g_DisplayThread = nullptr;
    }

    g_DisplayWindow = nullptr;
    g_DisplayPixels.clear();
    g_DisplayFrameBuffer = {};
    g_DisplayBitmapInformation = {};

    ::MoRetroVDisplayClearKeyboardQueue();
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
