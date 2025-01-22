using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Avalonia.Controls;
using Avalonia.Layout;
using MeasurementTool.Devices.Controls;

namespace MeasurementTool.Devices;

public class PowerMeter: GenericDevice
{
    private readonly int _channels, _voltageResolution, _currentResolution;
    private readonly int _voltageNumbersBeforePoint, _currentNumbersBeforePoint;
    private readonly MeterReturnValueType _voltageValueType, _currentValueType;
    private readonly string _name;
    private readonly byte _channel;

    private List<PowerMeterChannel> _channelsUi;

    public PowerMeter(DeviceManager dm, byte[] config, byte channel) : base(dm, channel)
    {
        if (config.Length < 8)
            throw new ArgumentException($"Power meter channel {channel}: Invalid configuration length {config.Length}");
        _channelsUi = [];
        _channels = config[0];
        _voltageResolution = config[1];
        Meter.Validateresolution(_voltageResolution, channel);
        _voltageValueType = (MeterReturnValueType)config[2];
        _voltageNumbersBeforePoint = config[3];
        _currentResolution = config[4];
        Meter.Validateresolution(_currentResolution, channel);
        _currentValueType = (MeterReturnValueType)config[5];
        _currentNumbersBeforePoint = config[6];
        _name = Encoding.UTF8.GetString(config[7..]);
        _channel = channel;
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

    private List<PowerMeterChannel> CreateChannelsUi()
    {
        return Enumerable.Range(0, _channels).Select(n => 
            new PowerMeterChannel(_voltageValueType, _voltageNumbersBeforePoint, _currentValueType, _currentNumbersBeforePoint))
            .ToList();
    }

    internal override string GetName()
    {
        return _name;
    }
    
    internal override void ParseResponse(byte[] data)
    {
        if (data.Length != _channels * (_voltageResolution + _currentResolution) || _channelsUi.Count != _channels)
        {
            Dm.LogError(Channel, "Invalid response length");
            return;
        }

        var idx = 0;
        for (var i = 0; i < _channels; i++)
        {
            var voltage = Meter.GetValue(_voltageResolution, data, ref idx);
            var current = Meter.GetValue(_currentResolution, data, ref idx);
            _channelsUi[i].SetValues(voltage, current);
        }
    }
}
