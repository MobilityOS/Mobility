using System.Text.Json;
using System.Text.Json.Serialization;

namespace Mobility.Host.Terminal
{
    [JsonSourceGenerationOptions(
        PropertyNameCaseInsensitive = true,
        ReadCommentHandling = JsonCommentHandling.Skip,
        AllowTrailingCommas = true)]
    [JsonSerializable(
        typeof(ConfigurationDefinition),
        TypeInfoPropertyName = "Configuration")]
    internal partial class JsonContext : JsonSerializerContext
    {

    }
}
