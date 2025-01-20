using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Avalonia.Controls;
using Avalonia.Layout;
using MeasurementTool.Devices.Controls;

namespace MeasurementTool.Devices;

public class Meter: GenericDevice
{
    private readonly int _channels, _resolution, _numbersBeforePoint;
    private readonly MeterReturnValueType _valueType;
    private readonly MeterMeasureType _measureType;
    private readonly string _name;

    private List<MeterChannel> _channelsUi;
    
    public Meter(DeviceManager dm, byte[] config, byte channel) : base(dm, channel)
    {
        if (config.Length < 6)
            throw new ArgumentException($"Voltmeter channel {channel}: Invalid configuration length {config.Length}");
        _channelsUi = [];
        _channels = config[0];
        _resolution = config[1];
        if (_resolution != 2 && _resolution != 4 && _resolution != 8)
            throw new ArgumentException($"Voltmeter channel {channel}: Invalid resolution {_resolution}");
        _measureType = (MeterMeasureType)config[2];
        _valueType = (MeterReturnValueType)config[3];
        _numbersBeforePoint = config[4];
        _name = Encoding.UTF8.GetString(config[5..]);
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

    private List<MeterChannel> CreateChannelsUi()
    {
        return Enumerable.Range(0, _channels).Select(n => new MeterChannel()).ToList();
    }
    
    internal override string GetName()
    {
        return _name;
    }

    internal override void ParseResponse(byte[] data)
    {
        if (data.Length != _channels * _resolution || _channelsUi.Count != _channels)
        {
            Dm.LogError(Channel, "Invalid response length");
            return;
        }

        var idx = 0;
        for (var i = 0; i < _channels; i++)
        {
            long value;
            if (_resolution == 2)
            {
                value = BitConverter.ToInt16(data, idx);
                idx += 2;
            }
            else if (_resolution == 4)
            {
                value = BitConverter.ToInt32(data, idx);
                idx += 4;
            }
            else
            {
                value = BitConverter.ToInt64(data, idx);
                idx += 8;
            }
            _channelsUi[i].Value = value;
        }
    }
}

internal enum MeterMeasureType
{
    Voltage,
    Current,
    Resistance,
    Capacitance,
    Inductance,
    Temperature,
    Frequency
}

internal enum MeterReturnValueType
{
    Ones,
    Millis,
    Micros,
    Nanos,
    Picos
}