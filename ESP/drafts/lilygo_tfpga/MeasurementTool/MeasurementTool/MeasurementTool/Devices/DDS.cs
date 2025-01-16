using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading;
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
    private bool _getCapabilitiesFailed;
    
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
        _getCapabilitiesFailed = false;
    }

    private void ProcessGetCapabilitiesResponse(byte[]? response)
    {
        if (response is not { Length: 11 })
        {
            _getCapabilitiesFailed = true;
            return;
        }
        using var stream = new MemoryStream(response);
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
        MaxAmplitude = ToDb(ddsConfig.MaxVoutMv);
        MinAmplitude = MaxAmplitude - ddsConfig.MaxAttenuatorValue;
        _ddsConfig = ddsConfig;
    }

    internal override void Init()
    {
        Dm.QueueCommand(Channel, [(byte)DdsCommands.GetCapabilities], ProcessGetCapabilitiesResponse); // get capabilities
    }

    private static int ToDb(int mv)
    {
        return (int)(10 * Math.Log10((double)(mv * mv) / 50000)); // 50 Ohm
    }

    internal override Control? CreateUi()
    {
        while (_ddsConfig.Type == DdsType.Unknown)
        {
            if (_getCapabilitiesFailed)
                return null;
            Thread.Sleep(100);
        }
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
    GetCapabilities = 'c',
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
