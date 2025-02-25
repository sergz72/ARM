using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using Avalonia.Controls;
using Avalonia.Layout;
using MeasurementTool.Devices.Controls;

namespace MeasurementTool.Devices;

public sealed class Dds: GenericDevice
{
    private static readonly Dictionary<DdsType, DdsInfo> DdsInfos = new()
    {
        { DdsType.Ad9833, new DdsInfo(mclk => mclk / 2, mclk => 1, 1,
            [1,2], DdsModes.Sine|DdsModes.Square|DdsModes.Triangle, 28)},
        { DdsType.Ad9850, new DdsInfo(mclk => mclk / 2, mclk => 1, 1,
            [1], DdsModes.Sine, 32)},
        { DdsType.Ad9851, new DdsInfo(mclk => mclk / 2, mclk => 1, 1,
            [1], DdsModes.Sine, 32)},
        { DdsType.Adf4351, new DdsInfo(mclk => 4400000000, mclk => 35000000, 1,
            [1], DdsModes.Sine, 0)},
        { DdsType.Si5351, new DdsInfo(mclk => 225000000, mclk => 439454, 3,
            [1,2,4,8,16,32,64,128], DdsModes.Square, 0)}
    };

    public readonly long MinFrequency;
    public readonly long MaxFrequency;
    public readonly int MinAmplitude;
    public readonly int MaxAmplitude;
    public readonly DdsModes SupportedNodes;
    public readonly int[] Dividers;
    private readonly int _channels;
    private readonly int _accumulatorBits;
    private readonly bool _sendCodes;
    private readonly DdsConfig _ddsConfig;

    private List<DDSChannel> _channelsUi;
    private LevelMeter? _levelMeter;
    private long _sweepF1;
    private int _sweepStep;
    
    public Dds(DeviceManager dm, byte[] config, byte channel) : base(dm, channel)
    {
        if (config.Length != 15)
            throw new ArgumentException($"DDS channel {channel}: Invalid config length: {config.Length}");
        _channelsUi = [];
        using var stream = new MemoryStream(config);
        using var reader = new BinaryReader(stream);
        var type = (DdsType)reader.ReadInt16();
        var meterType = (MeterType)reader.ReadInt16();
        var mClk = reader.ReadInt64();
        var maxMv = reader.ReadInt16();
        var maxAttenuator = reader.ReadByte();
        var ddsConfig = new DdsConfig(type, meterType, mClk, maxMv, maxAttenuator);
        if (DdsInfos.TryGetValue(ddsConfig.Type, out var info))
        {
            _channels = info.Channels;
            MinFrequency = info.MinFrequencyFn(mClk);
            MaxFrequency = info.MaxFrequencyFn(mClk);
            SupportedNodes = info.SupportedModes;
            Dividers = info.Dividers;
            _accumulatorBits = info.AccumulatorBits;
            _sendCodes = _accumulatorBits > 0;
        }
        else
            throw new ArgumentException($"DDS channel {channel}: Unknown DDS type: {type}");
        MaxAmplitude = ToDb(ddsConfig.MaxVoutMv);
        MinAmplitude = MaxAmplitude - ddsConfig.MaxAttenuatorValue;
        _ddsConfig = ddsConfig;
    }

    private static int ToDb(int mv)
    {
        return (int)(10 * Math.Log10((double)(mv * mv) / 50000)); // 50 Ohm
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
        if (_ddsConfig.MeterType != MeterType.None)
        {
            _levelMeter = new LevelMeter(_ddsConfig.MeterType){Height = 200, HorizontalAlignment = HorizontalAlignment.Stretch};
            panel.Children.Add(_levelMeter);
        }
        return panel;
    }

    private List<DDSChannel> CreateChannelsUi()
    {
        return Enumerable.Range(0, _channels).Select(n => new DDSChannel(this, n)).ToList();
    }

    internal override string GetName() => _ddsConfig.Type.ToString();

    public void SetMode(int channel, DdsModes mode)
    {
        var outMode = mode switch
        {
            DdsModes.Triangle => (byte)1,
            DdsModes.Square => (byte)2,
            _ => (byte)0
        };
        var command = new[] { (byte)DdsCommands.SetMode, (byte)channel, (byte)outMode };
        Dm.QueueCommand(Channel, command, CheckError);
    }

    public void OutputEnable(int channel, bool enable)
    {
        var command = new[] { (byte)DdsCommands.EnableOutput, (byte)channel, enable ? (byte)1 : (byte)0 };
        Dm.QueueCommand(Channel, command, CheckError);
    }

    public void SetAmplitude(int channel, int amplitude)
    {
        var attenuator = MaxAmplitude - amplitude;
        var command = new[] { (byte)DdsCommands.SetAttenuator, (byte)channel, (byte)attenuator };
        Dm.QueueCommand(Channel, command, CheckError);
    }

    private long CalculateFrequencyCode(long value)
    {
        var v = (value << _accumulatorBits) / (_ddsConfig.Mclk * 10);
        return v;
    }
    
    public void SetFrequency(int channel, long value, int divider)
    {
        using var ms = new MemoryStream();
        using var bw = new BinaryWriter(ms);
        bw.Write(_sendCodes ? (byte)DdsCommands.SetFrequencyCode : (byte)DdsCommands.SetFrequency);
        bw.Write((byte)channel);
        bw.Write(_sendCodes ? CalculateFrequencyCode(value) : value);
        bw.Write((short)divider);
        Dm.QueueCommand(Channel, ms.ToArray(), CheckError);
    }

    private int CalculateStep(long f2, int numPoints)
    {
        var code2 = CalculateFrequencyCode(f2);
        return (int)((code2 - _sweepF1) / numPoints);
    }
    
    public void SetSweep(int channel, long f1, int divider, int step, int numPoints)
    {
        var f2 = f1 + (long)step * (numPoints - 1);
        _levelMeter?.SetF(f1, f2);
        using var ms = new MemoryStream();
        using var bw = new BinaryWriter(ms);
        bw.Write(_sendCodes ? (byte)DdsCommands.SweepCodes : (byte)DdsCommands.Sweep);
        bw.Write((byte)channel);
        _sweepF1 = _sendCodes ? CalculateFrequencyCode(f1) : f1;
        bw.Write(_sweepF1);
        bw.Write((short)divider);
        _sweepStep = _sendCodes ? CalculateStep(f2, numPoints) : step;
        bw.Write(_sweepStep);
        bw.Write((short)numPoints);
        Dm.QueueCommand(Channel, ms.ToArray(), CheckError);
    }

    internal override void ParseResponse(byte[] data)
    {
        if (_levelMeter != null && data.Length > 0 && data.Length % 2 == 0)
        {
            var points = Enumerable.Range(0, data.Length / 2)
                .Select(p => BitConverter.ToInt16(data, p * 2))
                .ToArray();
            _levelMeter.Data = points;
        }
    }
}

[Flags]
public enum DdsModes
{
    Sine = 1,
    Triangle = 2,
    Square = 4
}

internal enum DdsType
{
    Unknown,
    Ad9833,
    Si5351,
    Ad9850,
    Ad9851,
    Adf4351
}

public enum MeterType
{
    None,
    Ad8307,
    Ad8310,
    Ad8313,
    Ad8317,
    Ad8318
}

internal enum DdsCommands
{
    SetFrequency = 'f',
    SetFrequencyCode = 'c',
    SetMode = 'm',
    SetAttenuator = 'a',
    EnableOutput = 'e',
    Sweep = 's',
    SweepCodes = 'd'
}

internal record DdsInfo(
    Func<long, long> MaxFrequencyFn,
    Func<long, long> MinFrequencyFn,
    int Channels,
    int[] Dividers,
    DdsModes SupportedModes,
    int AccumulatorBits);

internal record DdsConfig(
    DdsType Type,
    MeterType MeterType,
    long Mclk,
    int MaxVoutMv,
    int MaxAttenuatorValue);
