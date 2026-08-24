using Microsoft.Win32.SafeHandles;
using System.Runtime.InteropServices;

namespace Mobility.Host.Terminal
{
    internal class NativeMethods
    {
        internal const int STD_INPUT_HANDLE = -10;
        internal const int STD_OUTPUT_HANDLE = -11;
        internal const int STD_ERROR_HANDLE = -12;

        [DllImport("kernel32.dll", EntryPoint = "GetStdHandle", SetLastError = true)]
        private static extern IntPtr _GetStdHandle(int nStdHandle);

        internal static SafeFileHandle GetStdHandle(int nStdHandle) =>
            new SafeFileHandle(_GetStdHandle(nStdHandle), false);

        // Input Mode Flags:

        internal const uint ENABLE_PROCESSED_INPUT = 0x0001;
        internal const uint ENABLE_LINE_INPUT = 0x0002;
        internal const uint ENABLE_ECHO_INPUT = 0x0004;
        internal const uint ENABLE_WINDOW_INPUT = 0x0008;
        internal const uint ENABLE_MOUSE_INPUT = 0x0010;
        internal const uint ENABLE_INSERT_MODE = 0x0020;
        internal const uint ENABLE_QUICK_EDIT_MODE = 0x0040;
        internal const uint ENABLE_EXTENDED_FLAGS = 0x0080;
        internal const uint ENABLE_AUTO_POSITION = 0x0100;
        internal const uint ENABLE_VIRTUAL_TERMINAL_INPUT = 0x0200;

        // Output Mode Flags:

        internal const uint ENABLE_PROCESSED_OUTPUT = 0x0001;
        internal const uint ENABLE_WRAP_AT_EOL_OUTPUT = 0x0002;
        internal const uint ENABLE_VIRTUAL_TERMINAL_PROCESSING = 0x0004;
        internal const uint DISABLE_NEWLINE_AUTO_RETURN = 0x0008;
        internal const uint ENABLE_LVB_GRID_WORLDWIDE = 0x0010;

        [DllImport("kernel32.dll", SetLastError = true)]
        internal static extern bool GetConsoleMode(
            SafeFileHandle hConsoleHandle,
            out uint lpMode);

        [DllImport("kernel32.dll", SetLastError = true)]
        internal static extern bool SetConsoleMode(
            SafeFileHandle hConsoleHandle,
            uint dwMode);
    }
}
