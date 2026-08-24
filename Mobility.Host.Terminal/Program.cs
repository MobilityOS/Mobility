using System.IO.Pipes;
using System.IO.Ports;
using System.Text;
using System.Text.Json;

namespace Mobility.Host.Terminal
{
    internal class Program
    {
        private static readonly Encoding TerminalEncoding =
            new UTF8Encoding(encoderShouldEmitUTF8Identifier: false);

        private sealed class TerminalConnection : IDisposable
        {
            private readonly IDisposable _owner;
            private bool _disposed;

            public TerminalConnection(
                Stream stream,
                IDisposable owner)
            {
                Stream = stream;
                _owner = owner;
            }

            public Stream Stream { get; }

            public void Dispose()
            {
                if (_disposed)
                {
                    return;
                }

                _disposed = true;
                _owner.Dispose();
            }
        }

        private static ConfigurationDefinition LoadConfiguration()
        {
            string repositoryRoot = Helpers.GetGitRepositoryRootPath();

            if (string.IsNullOrEmpty(repositoryRoot))
            {
                throw new InvalidOperationException(
                    "Unable to locate the Git repository root.");
            }

            string path = Path.Combine(
                repositoryRoot,
                "Mobility.Host.Terminal",
                "Configuration.json");

            if (!File.Exists(path))
            {
                throw new FileNotFoundException(
                    $"Configuration file was not found: {path}",
                    path);
            }

            string json = File.ReadAllText(
                path,
                TerminalEncoding);

            ConfigurationDefinition? configuration =
                JsonSerializer.Deserialize(
                    json,
                    JsonContext.Default.Configuration);

            if (configuration is null)
            {
                throw new InvalidDataException(
                    "The configuration file is empty.");
            }

            return configuration;
        }

        private static ProfileDefinition? SelectProfile(
            IReadOnlyList<ProfileDefinition> profiles)
        {
            if (profiles.Count == 1)
            {
                Console.WriteLine(
                    "Automatically selected: {0}",
                    profiles[0].Name);

                return profiles[0];
            }

            while (true)
            {
                Console.WriteLine();
                Console.WriteLine("Select a terminal profile:");

                for (int index = 0; index < profiles.Count; index++)
                {
                    ProfileDefinition profile = profiles[index];

                    Console.WriteLine(
                        "  {0}. {1} [{2}: {3}]",
                        index + 1,
                        profile.Name,
                        profile.Type,
                        profile.Resource);
                }

                Console.Write("Enter a number or Q to exit: ");

                string? input = Console.ReadLine();

                if (input is null)
                {
                    return null;
                }

                input = input.Trim();

                if (string.Equals(
                    input,
                    "q",
                    StringComparison.OrdinalIgnoreCase))
                {
                    return null;
                }

                if (int.TryParse(input, out int selected) &&
                    selected >= 1 &&
                    selected <= profiles.Count)
                {
                    return profiles[selected - 1];
                }

                Console.WriteLine("Invalid selection.");
            }
        }

        private static TerminalConnection OpenSerialPort(
            ProfileDefinition profile)
        {
            if (string.IsNullOrWhiteSpace(profile.Resource))
            {
                throw new InvalidDataException(
                    "The serial port name is empty.");
            }

            if (profile.Baud <= 0)
            {
                throw new InvalidDataException(
                    "The serial port baud rate must be greater than zero.");
            }

            SerialPort serialPort = new SerialPort(
                profile.Resource,
                profile.Baud,
                Parity.None,
                8,
                StopBits.One)
            {
                Handshake = Handshake.None,
                DtrEnable = true,
                RtsEnable = true,
                ReadTimeout = SerialPort.InfiniteTimeout,
                WriteTimeout = SerialPort.InfiniteTimeout
            };

            try
            {
                serialPort.Open();

                return new TerminalConnection(
                    serialPort.BaseStream,
                    serialPort);
            }
            catch
            {
                serialPort.Dispose();
                throw;
            }
        }

        private static string NormalizePipeName(
            string resource)
        {
            const string localPipePrefix = @"\\.\pipe\";

            string value = resource.Trim();

            if (value.StartsWith(
                    localPipePrefix,
                    StringComparison.OrdinalIgnoreCase))
            {
                value = value.Substring(
                    localPipePrefix.Length);
            }

            if (value.Length == 0)
            {
                throw new InvalidDataException(
                    "The named pipe name is empty.");
            }

            return value;
        }

        private static TerminalConnection OpenNamedPipe(
            ProfileDefinition profile)
        {
            string pipeName =
                NormalizePipeName(profile.Resource);

            NamedPipeClientStream pipe = new NamedPipeClientStream(
                ".",
                pipeName,
                PipeDirection.InOut,
                PipeOptions.Asynchronous);

            try
            {
                // Fail immediately if no pipe server is available.
                pipe.Connect(0);

                return new TerminalConnection(
                    pipe,
                    pipe);
            }
            catch
            {
                pipe.Dispose();
                throw;
            }
        }

        private static TerminalConnection OpenConnection(
            ProfileDefinition profile)
        {
            if (string.Equals(
                profile.Type,
                "SerialPort",
                StringComparison.OrdinalIgnoreCase))
            {
                return OpenSerialPort(profile);
            }

            if (string.Equals(
                profile.Type,
                "NamedPipe",
                StringComparison.OrdinalIgnoreCase))
            {
                return OpenNamedPipe(profile);
            }

            throw new InvalidDataException(
                $"Unsupported connection type: {profile.Type}");
        }

        private static byte[] GetInputBytes(
            ConsoleKeyInfo key)
        {
            if (key.Key == ConsoleKey.Enter)
            {
                return TerminalEncoding.GetBytes("\r\n");
            }

            if (key.KeyChar == '\0')
            {
                return new byte[0];
            }

            return TerminalEncoding.GetBytes(key.KeyChar.ToString());
        }

        private static async Task PumpOutputAsync(
            Stream stream)
        {
            byte[] inputBuffer = new byte[256];

            char[] outputBuffer = new char[
                TerminalEncoding.GetMaxCharCount(
                    inputBuffer.Length)];

            Decoder decoder = TerminalEncoding.GetDecoder();

            while (true)
            {
                int bytesRead = await stream.ReadAsync(
                    inputBuffer,
                    0,
                    inputBuffer.Length);

                if (bytesRead == 0)
                {
                    int finalCharacterCount = decoder.GetChars(
                        Array.Empty<byte>(),
                        0,
                        0,
                        outputBuffer,
                        0,
                        true);

                    if (finalCharacterCount != 0)
                    {
                        Console.Write(
                            outputBuffer,
                            0,
                            finalCharacterCount);

                        Console.Out.Flush();
                    }

                    return;
                }

                int characterCount = decoder.GetChars(
                    inputBuffer,
                    0,
                    bytesRead,
                    outputBuffer,
                    0,
                    false);

                if (characterCount != 0)
                {
                    Console.Write(
                        outputBuffer,
                        0,
                        characterCount);

                    Console.Out.Flush();
                }
            }
        }

        private static async Task IgnoreTaskFailureAsync(
            Task task)
        {
            try
            {
                await task;
            }
            catch
            {
                // The stream was closed intentionally.
            }
        }

        private static async Task<bool> RunTerminalAsync(
            TerminalConnection connection)
        {
            Stream stream = connection.Stream;

            Task outputTask = PumpOutputAsync(stream);

            try
            {
                while (!outputTask.IsCompleted)
                {
                    if (!Console.KeyAvailable)
                    {
                        await Task.Delay(10);
                        continue;
                    }

                    ConsoleKeyInfo key = Console.ReadKey(true);

                    if (key.KeyChar == '\x1D')
                    {
                        connection.Dispose();

                        await IgnoreTaskFailureAsync(outputTask);

                        return false;
                    }

                    byte[] data = GetInputBytes(key);

                    if (data.Length == 0)
                    {
                        continue;
                    }

                    await stream.WriteAsync(
                        data,
                        0,
                        data.Length);

                    await stream.FlushAsync();
                }

                await outputTask;
                return true;
            }
            catch
            {
                connection.Dispose();
                await IgnoreTaskFailureAsync(outputTask);
                throw;
            }
        }

        private static async Task<int> RunAsync()
        {
            Console.InputEncoding = TerminalEncoding;
            Console.OutputEncoding = TerminalEncoding;

            Helpers.EnableVirtualTerminalOutput();

            bool originalTreatControlCAsInput =
                Console.TreatControlCAsInput;

            Console.TreatControlCAsInput = false;

            try
            {
                ConfigurationDefinition configuration = LoadConfiguration();

                List<ProfileDefinition> profiles = configuration.Profiles
                    .Where(profile => profile.Enabled)
                    .ToList();

                if (profiles.Count == 0)
                {
                    Console.Error.WriteLine(
                        "No enabled terminal profiles were found.");

                    return 1;
                }

                while (true)
                {
                    Console.TreatControlCAsInput = false;

                    ProfileDefinition? profile = SelectProfile(profiles);

                    if (profile is null)
                    {
                        return 0;
                    }

                    try
                    {
                        using (TerminalConnection connection =
                               OpenConnection(profile))
                        {
                            Console.WriteLine();

                            Console.WriteLine("Connected: {0}", profile.Name);

                            Console.WriteLine("Press Ctrl+] to exit.");

                            Console.WriteLine();
                            Console.Out.Flush();

                            bool disconnected;

                            Console.TreatControlCAsInput = true;

                            try
                            {
                                disconnected = await RunTerminalAsync(
                                    connection);
                            }
                            finally
                            {
                                Console.TreatControlCAsInput = false;
                            }

                            if (!disconnected)
                            {
                                return 0;
                            }
                        }

                        Console.WriteLine();

                        Console.WriteLine(
                            "Connection closed. Select another profile.");
                    }
                    catch (Exception exception)
                    {
                        Console.TreatControlCAsInput = false;

                        Console.Error.WriteLine();

                        Console.Error.WriteLine(
                            "Connection failed ({0}): {1}",
                            exception.GetType().Name,
                            exception.Message);
                    }
                }
            }
            catch (Exception exception)
            {
                Console.Error.WriteLine(
                    "Startup failed ({0}): {1}",
                    exception.GetType().Name,
                    exception.Message);

                return 1;
            }
            finally
            {
                Console.TreatControlCAsInput =
                    originalTreatControlCAsInput;
            }
        }

        static void Main(string[] args)
        {
            int ExitCode = 0;

            try
            {
                ExitCode = RunAsync().GetAwaiter().GetResult();
            }
            catch (Exception ex)
            {
                ExitCode = 1;
                Console.Error.WriteLine(
                    "Fatal error ({0}): {1}",
                    ex.GetType().Name,
                    ex.Message);
            }

            Environment.ExitCode = ExitCode;
        }
    }
}
