namespace Mobility.Host.Terminal
{
    public class ProfileDefinition
    {
        public bool Enabled { get; set; } = false;
        public string Name { get; set; } = string.Empty;
        public string Type { get; set; } = string.Empty;
        public string Resource { get; set; } = string.Empty;
        public int Baud { get; set; } = 0;
    }
}
