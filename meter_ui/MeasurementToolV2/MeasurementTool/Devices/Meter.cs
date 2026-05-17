using System;
using System.Collections.Generic;
using System.Globalization;
using System.Linq;
using System.Numerics;
using System.Text;
using Avalonia.Controls;
using Avalonia.Layout;
using MeasurementTool.Devices.Controls;

namespace MeasurementTool.Devices;

public class Meter<T>: GenericDevice where T: INumber<T>
{
    private readonly int _channels, _resolution, _numbersBeforePoint;
    private readonly MeterReturnValueType _valueType;
    private readonly MeterMeasureType _measureType;
    private readonly string _name;
    private readonly int _measurementPeriodMs;

    private List<MeterChannel<T>> _channelsUi;
    
    public Meter(DeviceManager dm, byte[] config, byte channel, int sizeofT) : base(dm, channel)
    {
        if (config.Length < 7)
            throw new ArgumentException($"Meter channel {channel}: Invalid configuration length {config.Length}");
        _resolution = sizeofT;
        _channelsUi = [];
        _channels = config[0];
        _measureType = (MeterMeasureType)config[1];
        _valueType = (MeterReturnValueType)config[2];
        _numbersBeforePoint = config[3];
        _measurementPeriodMs = BitConverter.ToInt16(config, 4);
        _name = Encoding.UTF8.GetString(config[6..]);
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

    private List<MeterChannel<T>> CreateChannelsUi()
    {
        return Enumerable.Range(0, _channels).Select(n => new MeterChannel<T>(_measureType, _valueType, _numbersBeforePoint, _measurementPeriodMs)).ToList();
    }
    
    internal override string GetName()
    {
        return _name;
    }
    
    internal override void ParseResponse(byte[] data)
    {
        var channelDataLength = data.Length / _channels;
        if ((data.Length % _channels) != 0 || (channelDataLength % _resolution) != 0 || _channelsUi.Count != _channels)
        {
            Dm.LogError(Channel, "Invalid response length");
            return;
        }

        var idx = 0;
        var channelDataCount = channelDataLength / _resolution;
        for (var i = 0; i < _channels; i++)
        {
            var values = new T[channelDataCount];
            Buffer.BlockCopy(data, idx, values, 0, channelDataLength);
            idx += channelDataLength;
            _channelsUi[i].AddValues(values); 
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
    Frequency,
    Power
}

internal enum MeterReturnValueType
{
    Ones,
    Millis,
    Micros,
    Nanos,
    Picos
}

internal static class MeterReturnValueTypeMethods<T> where T: INumber<T>
{
    private static readonly T Thousand = T.CreateChecked(1000);
    private static readonly char[] ValueTypes = ['.', 'm', 'u', 'n', 'p'];

    internal static string FormatY(MeterReturnValueType returnValueType, T number)
    {
        var sign = "";
        if (number < T.Zero)
        {
            sign = "-";
            number = -number;
        }
        var idx = (int)returnValueType;
        var values = new T[idx + 1];
        var endIdx = 0;
        while (idx >= 0 && number != T.Zero)
        {
            var v = number % Thousand;
            if (v != T.Zero && endIdx == 0)
                endIdx = idx;
            values[idx--] = v;
            number /= Thousand;
        }
        var sb = new StringBuilder();
        sb.Append(sign);
        var append0 = true;
        for (idx = 0; idx <= endIdx; idx++)
        {
            var value = values[idx];
            if (value == T.Zero)
                continue;
            append0 = false;
            sb.Append(value.ToString("D3", NumberFormatInfo.CurrentInfo));
            sb.Append(ValueTypes[idx]);
        }
        if (append0)
            sb.Append('0');
        return sb.ToString();
    }
} 