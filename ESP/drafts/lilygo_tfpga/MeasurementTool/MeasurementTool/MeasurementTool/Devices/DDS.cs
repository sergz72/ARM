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
        { DdsType.Ad9833, new DdsInfo(mclk => mclk / 2, mclk => 1, 1, 28,
            [1,2], DdsModes.Sine|DdsModes.Square|DdsModes.Triangle)},
        { DdsType.Ad9850, new DdsInfo(mclk => mclk / 2, mclk => 1, 1, 32,
            [1], DdsModes.Sine)},
        { DdsType.Adf4351, new DdsInfo(mclk => 4400000000, mclk => 35000000, 1, 0,
            [1,2,4,8,16,32,64], DdsModes.Sine)},
        { DdsType.Si5351, new DdsInfo(mclk => 160000000, mclk => 8000, 3, 0,
            [1,2,4,8,16,32,64,128], DdsModes.Square)}
    };
 
    public long MinFrequency { get; private set; }
    public long MaxFrequency { get; private set; }
    public int MinAmplitude { get; private set; }
    public int MaxAmplitude { get; private set; }
    public DdsModes SupportedNodes { get; private set; }
    public int[] Dividers { get; private set; }

    private DdsConfig _ddsConfig;
    private List<DDSChannel> _channelsUi;
    private LevelMeter? _levelMeter;
    private int _channels;
    
    public Dds(DeviceManager dm, byte channel) : base(dm, channel)
    {
        _ddsConfig = new DdsConfig(DdsType.Unknown, 0, 0, 0, 0, 0);
        _channelsUi = [];
        _channels = 0;
        MinFrequency = 0;
        MaxFrequency = 0;
        MinAmplitude = 0;
        MaxAmplitude = 0;
        SupportedNodes = DdsModes.Sine;
        Dividers = [1];
    }

    internal override void Init()
    {
        var response = Dm.SendCommand(Channel, [(byte)DdsCommands.GetCapabilities]); // get capabilities
        if (response.Length != 11)
            return;
        using var stream = new MemoryStream(response);
        using var reader = new BinaryReader(stream);
        var type = (DdsType)reader.ReadInt16();
        var minDb = reader.ReadSByte();
        var maxDb = reader.ReadSByte();
        var mClk = reader.ReadInt32();
        var maxMv = reader.ReadInt16();
        var maxAttenuator = reader.ReadByte();
        _ddsConfig = new DdsConfig(type, minDb, maxDb, mClk, maxMv, maxAttenuator);
        if (DdsInfos.TryGetValue(_ddsConfig.Type, out var info))
        {
            _channels = info.Channels;
            MinFrequency = info.MinFrequencyFn(mClk);
            MaxFrequency = info.MaxFrequencyFn(mClk);
            SupportedNodes = info.SupportedModes;
            Dividers = info.Dividers;
        }

        MaxAmplitude = ToDb(_ddsConfig.MaxVoutMv);
        MinAmplitude = MaxAmplitude - _ddsConfig.MaxAttenuatorValue;
    }

    private static int ToDb(int mv)
    {
        return (int)(10 * Math.Log10((double)(mv * mv) / 50000)); // 50 Ohm
    }

    internal override Control CreateUi()
    {
        var panel = new StackPanel
        {
            Orientation = Orientation.Vertical,
        };
        _channelsUi = CreateChannelsUi();
        panel.Children.AddRange(_channelsUi);
        if (_ddsConfig.LevelMeterMaxDb != _ddsConfig.LevelMeterMinDb)
        {
            _levelMeter = new LevelMeter();
            panel.Children.Add(_levelMeter);
        }
        return panel;
    }

    private List<DDSChannel> CreateChannelsUi()
    {
        return Enumerable.Range(0, _channels).Select(n => new DDSChannel(this, n)).ToList();
    }

    internal override string GetName() => _ddsConfig.Type.ToString();
}

internal abstract class DdsHandler
{
    internal abstract byte[] BuildDdsData(long frequency, int divider);
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
    Ad9850,
    Adf4351,
    Si5351
}

internal enum DdsCommands
{
    GetCapabilities = 'c',
    SetDdsData = 's',
    SetMode = 'm',
    SetAttenuator = 'a',
    EnableOutput = 'e',
    SetSweep = 'w'
}

internal record DdsInfo(
    Func<int, long> MaxFrequencyFn,
    Func<int, long> MinFrequencyFn,
    int Channels,
    int AccumulatorBits,
    int[] Dividers,
    DdsModes SupportedModes,
    Func<DDSHandler> BuildDdsHandler);

internal record DdsConfig(
    DdsType Type,
    int LevelMeterMinDb,
    int LevelMeterMaxDb,
    int Mclk,
    int MaxVoutMv,
    int MaxAttenuatorValue);
