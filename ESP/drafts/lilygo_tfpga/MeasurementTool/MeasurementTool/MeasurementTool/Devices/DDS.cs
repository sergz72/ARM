using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using Avalonia.Controls;
using Avalonia.Layout;
using MeasurementTool.Devices.Controls;

namespace MeasurementTool.Devices;

public sealed class Dds: GenericDevice
{
    private static readonly Dictionary<DdsType, DdsInfo> DdsInfos = new()
    {
        { DdsType.Ad9833, new DdsInfo(mclk => mclk / 2, mclk => 1, 1,
            [1,2], DdsModes.Sine|DdsModes.Square|DdsModes.Triangle)},
        { DdsType.Ad9850, new DdsInfo(mclk => mclk / 2, mclk => 1, 1,
            [1], DdsModes.Sine)},
        { DdsType.Adf4351, new DdsInfo(mclk => 4400000000, mclk => 35000000, 1,
            [1,2,4,8,16,32,64], DdsModes.Sine)},
        { DdsType.Si5351, new DdsInfo(mclk => 160000000, mclk => 8000, 3,
            [1,2,4,8,16,32,64,128], DdsModes.Square)}
    };

    public readonly long MinFrequency;
    public readonly long MaxFrequency;
    public readonly int MinAmplitude;
    public readonly int MaxAmplitude;
    public readonly DdsModes SupportedNodes;
    public readonly int[] Dividers;
    private readonly int _channels;
    private readonly DdsConfig _ddsConfig;

    private List<DDSChannel> _channelsUi;
    private LevelMeter? _levelMeter;
    
    public Dds(DeviceManager dm, byte[] config, byte channel) : base(dm, channel)
    {
        if (config.Length != 11)
            throw new ArgumentException($"DDS channel {channel}: Invalid config length: {config.Length}");
        _channelsUi = [];
        using var stream = new MemoryStream(config);
        using var reader = new BinaryReader(stream);
        var type = (DdsType)reader.ReadInt16();
        var minDb = reader.ReadSByte();
        var maxDb = reader.ReadSByte();
        var mClk = reader.ReadInt32();
        var maxMv = reader.ReadInt16();
        var maxAttenuator = reader.ReadByte();
        var ddsConfig = new DdsConfig(type, minDb, maxDb, mClk, maxMv, maxAttenuator);
        if (DdsInfos.TryGetValue(ddsConfig.Type, out var info))
        {
            _channels = info.Channels;
            MinFrequency = info.MinFrequencyFn(mClk);
            MaxFrequency = info.MaxFrequencyFn(mClk);
            SupportedNodes = info.SupportedModes;
            Dividers = info.Dividers;
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

    private void CheckError(byte[]? response)
    {
        if (response == null)
            return;
        if (response.Length == 0)
            Dm.LogError(Channel, "Empty response");
        if (response[0] != (byte)'k')
            Dm.LogError(Channel, Encoding.UTF8.GetString(response));
    }
    public void SetMode(int channel, DdsModes mode)
    {
        var command = new[] { (byte)DdsCommands.SetMode, (byte)channel, (byte)mode };
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

    public void SetFrequency(int channel, long value, int divider)
    {
        using var ms = new MemoryStream();
        using var bw = new BinaryWriter(ms);
        bw.Write((byte)DdsCommands.SetFrequency);
        bw.Write((byte)channel);
        bw.Write(value);
        bw.Write((short)divider);
        Dm.QueueCommand(Channel, ms.ToArray(), CheckError);
    }

    public void SetSweep(int channel, long f1, long f2, int divider, int step)
    {
        using var ms = new MemoryStream();
        using var bw = new BinaryWriter(ms);
        bw.Write((byte)DdsCommands.Sweep);
        bw.Write((byte)channel);
        bw.Write(f1);
        bw.Write(f2);
        bw.Write((short)divider);
        bw.Write(step);
        Dm.QueueCommand(Channel, ms.ToArray(), CheckError);
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
    Ad9850,
    Adf4351,
    Si5351
}

internal enum DdsCommands
{
    SetFrequency = 'f',
    SetMode = 'm',
    SetAttenuator = 'a',
    EnableOutput = 'e',
    Sweep = 's'
}

internal record DdsInfo(
    Func<int, long> MaxFrequencyFn,
    Func<int, long> MinFrequencyFn,
    int Channels,
    int[] Dividers,
    DdsModes SupportedModes);

internal record DdsConfig(
    DdsType Type,
    int LevelMeterMinDb,
    int LevelMeterMaxDb,
    int Mclk,
    int MaxVoutMv,
    int MaxAttenuatorValue);
