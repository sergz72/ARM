using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using Avalonia.Controls;
using Avalonia.Layout;
using MeasurementTool.Devices.Controls;

namespace MeasurementTool.Devices;

public sealed class Pwm: GenericDevice
{
    private List<PwmChannel> _channelsUi;
    private readonly int _channels;
    private readonly bool _ddsClock;
    private readonly int _bits, _prescalerBits;
    private readonly long _maxFrequencyCode, _maxPrescaler;
    private readonly string _name;

    internal readonly int MClk;
    
    public Pwm(DeviceManager dm, byte[] config, byte channel) : base(dm, channel)
    {
        if (config.Length < 9)
            throw new ArgumentException($"PWM channel {channel}: Invalid configuration length {config.Length}");
        _channelsUi = [];
        using var stream = new MemoryStream(config);
        using var reader = new BinaryReader(stream);
        MClk = reader.ReadInt32();
        _channels = reader.ReadByte();
        _ddsClock = reader.ReadByte() != 0;
        _bits = reader.ReadByte();
        _prescalerBits = reader.ReadByte();
        _name = Encoding.UTF8.GetString(config[8..]);
        _maxFrequencyCode = (1 << _bits) - 1;
        _maxPrescaler = 1 << _prescalerBits;
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
        return _name;
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

    public FactPwmValues CalculateFactValues(long frequency, double duty)
    {
        var clock = CalculateClock(frequency, duty);
        var (fc, f, p) = CalculateFrequencyCodeAndPrescaler(clock, (int)frequency);
        var (dc, d) = CalculateDutyCode(fc, duty);
        return new FactPwmValues(clock, f, d, p, fc, dc);
    }
    
    public void SetFrequencyAndDuty(int channel, FactPwmValues values)
    {
        using var ms = new MemoryStream();
        using var bw = new BinaryWriter(ms);
        bw.Write((byte)PwmCommands.SetFrequency);
        bw.Write((byte)channel);
        bw.Write((uint)values.Clock);
        bw.Write((uint)values.FrequencyCode);
        bw.Write((uint)values.DutyCode);
        bw.Write((ushort)values.Prescaler);
        Dm.QueueCommand(Channel, ms.ToArray(), CheckError);
    }

    private long CalculateClock(long frequency, double duty)
    {
        //todo
        return MClk;
    }

    private static (long, double) CalculateDutyCode(long fc, double duty)
    {
        var code = (long)(fc * duty / 100);
        var d = (double)code * 100 / fc;
        return (code, d);
    }

    private (long, long, int) CalculateFrequencyCodeAndPrescaler(long clock, long frequency)
    {
        var code = clock / frequency;
        var prescaler = code / _maxFrequencyCode + 1;
        if (prescaler > _maxPrescaler)
            prescaler = _maxPrescaler;
        code = clock / (frequency * prescaler);
        if (code > _maxFrequencyCode)
            code = _maxFrequencyCode;
        var f = code == 0 ? 0 : clock / prescaler / code;
        return (code, f, (int)prescaler);
    }
}

internal enum PwmCommands
{
    SetFrequency = 'f',
    EnableOutput = 'e'
}

public record FactPwmValues(long Clock, long Frequency, double Duty, int Prescaler, long FrequencyCode, long DutyCode);