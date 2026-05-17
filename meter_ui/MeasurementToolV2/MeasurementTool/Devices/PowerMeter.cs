using System;
using System.Collections.Generic;
using System.Linq;
using System.Numerics;
using System.Text;
using Avalonia.Controls;
using Avalonia.Layout;
using MeasurementTool.Devices.Controls;

namespace MeasurementTool.Devices;

public class PowerMeter<T, T2>: GenericDevice where T: INumber<T> where T2: INumber<T2>
{
    private readonly int _channels, _voltageCurrentResolution, _powerResolution;
    private readonly int _voltageNumbersBeforePoint, _currentNumbersBeforePoint;
    private readonly MeterReturnValueType _voltageValueType, _currentValueType;
    private readonly string _name;
    private readonly byte _channel;
    private readonly int _measurementPeriodMs;

    private List<PowerMeterChannel<T, T2>> _channelsUi;

    public PowerMeter(DeviceManager dm, byte[] config, byte channel, int sizeofT, int sizeofT2) : base(dm, channel)
    {
        if (config.Length < 8)
            throw new ArgumentException($"Power meter channel {channel}: Invalid configuration length {config.Length}");
        _voltageCurrentResolution = sizeofT;
        _powerResolution = sizeofT2;
        _channelsUi = [];
        _channels = config[0];
        _voltageValueType = (MeterReturnValueType)config[1];
        _voltageNumbersBeforePoint = config[2];
        _currentValueType = (MeterReturnValueType)config[3];
        _currentNumbersBeforePoint = config[4];
        _measurementPeriodMs = BitConverter.ToInt16(config, 5);
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

    private List<PowerMeterChannel<T, T2>> CreateChannelsUi()
    {
        return Enumerable.Range(0, _channels).Select(n => 
            new PowerMeterChannel<T, T2>(_voltageValueType, _voltageNumbersBeforePoint, _currentValueType,
                _currentNumbersBeforePoint, _measurementPeriodMs))
            .ToList();
    }

    internal override string GetName()
    {
        return _name;
    }
    
    internal override void ParseResponse(byte[] data)
    {
        var channelDataLength = data.Length / _channels;
        var resolution = _voltageCurrentResolution * 2;
        if ((data.Length % _channels) != 0 || (channelDataLength % resolution) != 0 || _channelsUi.Count != _channels)
        {
            Dm.LogError(Channel, "Invalid response length");
            return;
        }

        var idx = 0;
        var channelDataCount = channelDataLength / resolution;
        channelDataLength /= 2;
        for (var i = 0; i < _channels; i++)
        {
            var voltageValues = new T[channelDataCount];
            var currentValues = new T[channelDataCount];
            Buffer.BlockCopy(data, idx, voltageValues, 0, channelDataLength);
            idx += channelDataLength;
            Buffer.BlockCopy(data, idx, currentValues, 0, channelDataLength);
            idx += channelDataLength;
            _channelsUi[i].AddValues(voltageValues, currentValues); 
        }
    }
}
