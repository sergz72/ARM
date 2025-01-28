using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using Avalonia.Controls;
using Avalonia.Layout;
using MeasurementTool.Devices.Controls;

namespace MeasurementTool.Devices;

public sealed class Pwm: GenericDevice
{
    private List<PwmChannel> _channelsUi;
    private readonly int _channels;
    private readonly bool _ddsClock;
    private readonly int _bits;

    internal readonly int MClk;
    
    public Pwm(DeviceManager dm, byte[] config, byte channel) : base(dm, channel)
    {
        if (config.Length < 6)
            throw new ArgumentException($"PWM channel {channel}: Invalid configuration length {config.Length}");
        _channelsUi = [];
        using var stream = new MemoryStream(config);
        using var reader = new BinaryReader(stream);
        MClk = reader.ReadInt32();
        _channels = reader.ReadByte();
        _ddsClock = reader.ReadByte() != 0;
        _bits = reader.ReadByte();
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
        var command = new[] { (byte)PwmCommands.EnableOutput, (byte)channel, enable ? (byte)1 : (byte)0 };
        Dm.QueueCommand(Channel, command, CheckError);
    }

    public (long, long) SetFrequencyAndDuty(int channel, long frequency, long duty)
    {
        using var ms = new MemoryStream();
        using var bw = new BinaryWriter(ms);
        bw.Write((byte)PwmCommands.SetFrequency);
        bw.Write((byte)channel);
        var c = CalculateClock(frequency, duty);
        bw.Write(c);
        var (fc, f) = CalculateFrequencyCode(c, (int)frequency);
        bw.Write(fc);
        var (dc, d) = CalculateDutyCode(f, (int)duty);
        bw.Write(dc);
        Dm.QueueCommand(Channel, ms.ToArray(), CheckError);
        return (f, d);
    }

    private int CalculateClock(long frequency, long duty)
    {
        //todo
        return MClk;
    }

    private (int, int) CalculateDutyCode(int f, int duty)
    {
        var code = (long)f * duty / 1000;
        var d = code * 1000 / f;
        return ((int)code, (int)d);
    }

    private (int, int) CalculateFrequencyCode(int clock, int frequency)
    {
        var code = MClk / frequency;
        var f = code == 0 ? 0 : MClk / code;
        return (code, f);
    }
}

internal enum PwmCommands
{
    SetFrequency = 'f',
    EnableOutput = 'e'
}
