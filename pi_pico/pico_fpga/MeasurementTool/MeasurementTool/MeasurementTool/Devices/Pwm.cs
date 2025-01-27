using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using Avalonia.Controls;
using Avalonia.Layout;
using MeasurementTool.Devices.Controls;

namespace MeasurementTool.Devices;

public class Pwm: GenericDevice
{
    private List<PwmChannel> _channelsUi;
    private readonly int _channels;
    internal readonly bool DdsClock;
    internal readonly int MClk;
    internal readonly int Bits;
    
    public Pwm(DeviceManager dm, byte[] config, byte channel) : base(dm, channel)
    {
        if (config.Length < 6)
            throw new ArgumentException($"PWM channel {channel}: Invalid configuration length {config.Length}");
        _channelsUi = [];
        using var stream = new MemoryStream(config);
        using var reader = new BinaryReader(stream);
        MClk = reader.ReadInt32();
        _channels = reader.ReadByte();
        DdsClock = reader.ReadByte() != 0;
        Bits = reader.ReadByte();
    }

    internal override Control? CreateUi()
    {
        var panel = new StackPanel
        {
            Orientation = Orientation.Vertical,
            Spacing = 3
        };
        _channelsUi = CreateChannelsUi();
        panel.Children.AddRange(_channelsUi);
        return panel;
    }

    internal override string GetName()
    {
        return "PWM Generator";
    }
    
    private List<PwmChannel> CreateChannelsUi()
    {
        return Enumerable.Range(0, _channels).Select(n => new PwmChannel(this, n)).ToList();
    }

    public void OutputEnable(int channel, bool enable)
    {
        //todo
    }

    public void SetDuty(int channel, long value)
    {
        //todo
    }

    public void SetFrequency(int channel, long value)
    {
        //todo
    }
}