using Microsoft.Win32.SafeHandles;
using System.Diagnostics;
#if NETSTANDARD1_1_OR_GREATER
using System.Runtime.InteropServices;
#endif

namespace Mobility.Host.Terminal
{
    public class Helpers
    {
        public static bool IsWindows()
        {
#if NET462_OR_GREATER
            return true;
#elif NETSTANDARD1_1_OR_GREATER
            return RuntimeInformation.IsOSPlatform(OSPlatform.Windows);
#else
            return false;
#endif
        }

        public static string GetGitRepositoryRootPath()
        {
            try
            {
                Process process = new Process
                {
                    StartInfo = new ProcessStartInfo
                    {
                        CreateNoWindow = true,
                        UseShellExecute = false,
                        RedirectStandardOutput = true,
                        FileName = "git.exe",
                        Arguments = "rev-parse --show-toplevel",
                        WorkingDirectory = AppDomain.CurrentDomain.BaseDirectory
                    }
                };
                if (process.Start())
                {
                    string result = process.StandardOutput.ReadToEnd().Trim();
                    process.WaitForExit();
                    if (process.ExitCode == 0)
                    {
                        if (!string.IsNullOrEmpty(result))
                        {
                            return Path.GetFullPath(result);
                        }
                    }
                }
            }
            catch
            {
                // Git discovery is allowed to fail.
            }

            return string.Empty;
        }

        public static void EnableVirtualTerminalOutput()
        {
            if (!IsWindows())
            {
                return;
            }

            try
            {
                SafeFileHandle OutputHandle = NativeMethods.GetStdHandle(
                    NativeMethods.STD_OUTPUT_HANDLE);

                if (OutputHandle.IsInvalid)
                {
                    return;
                }

                uint PreviousMode = 0;

                if (!NativeMethods.GetConsoleMode(
                    OutputHandle,
                    out PreviousMode))
                {
                    return;
                }

                NativeMethods.SetConsoleMode(
                    OutputHandle,
                    PreviousMode |
                    NativeMethods.ENABLE_VIRTUAL_TERMINAL_PROCESSING);
            }
            catch (DllNotFoundException)
            {
            }
            catch (EntryPointNotFoundException)
            {
            }
        }
    }
}
