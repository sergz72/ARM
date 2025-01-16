using Avalonia.Controls;

namespace MeasurementTool.Devices;

public class PowerMeter: GenericDevice
{
    private int _channels;

    public PowerMeter(DeviceManager dm, byte channel) : base(dm, channel)
    {
        _channels = 0;
    }

    internal override Control? CreateUi()
    {
        throw new System.NotImplementedException();
    }

    internal override string GetName()
    {
        throw new System.NotImplementedException();
    }
}

internal enum PowerMeterCommands
{
    GetCapabilities = 'c'
}
