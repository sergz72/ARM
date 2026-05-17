using System.Linq;
using System.Numerics;
using Avalonia;
using Avalonia.Controls;

namespace MeasurementTool.Devices.Controls;

public class PowerMeterChannel<T, T2> : UserControl where T: INumber<T> where T2: INumber<T2>
{
    private readonly MeterChannel<T> _voltageChannel, _currentChannel;
    private readonly MeterChannel<T2> _powerChannel;
    
    public PowerMeterChannel(): this(MeterReturnValueType.Millis, 2, MeterReturnValueType.Millis, 1, 0)
    {
    }

    internal PowerMeterChannel(MeterReturnValueType voltageValueType, int voltageNumbersBeforePoint,
        MeterReturnValueType currentValueType, int currentNumbersBeforePoint, int measurementPeriodMs)
    {
        var valuePanel = new WrapPanel();
        Content = valuePanel;
        _voltageChannel = new MeterChannel<T>(MeterMeasureType.Voltage, voltageValueType, voltageNumbersBeforePoint,
            measurementPeriodMs) {Margin = new Thickness(3,3)};
        _currentChannel = new MeterChannel<T>(MeterMeasureType.Current, currentValueType, currentNumbersBeforePoint,
            measurementPeriodMs) {Margin = new Thickness(3,3)};
        _powerChannel = new MeterChannel<T2>(MeterMeasureType.Power, (int)voltageValueType + currentValueType,
            currentNumbersBeforePoint + voltageNumbersBeforePoint, measurementPeriodMs)
            {Margin = new Thickness(3,3)};
        valuePanel.Children.Add(_voltageChannel);
        valuePanel.Children.Add(_currentChannel);
        valuePanel.Children.Add(_powerChannel);
    }

    internal void AddValues(T[] voltageValues, T[] currentValues)
    {
        _voltageChannel.AddValues(voltageValues);
        _currentChannel.AddValues(currentValues);
        _powerChannel.AddValues(voltageValues.Select((v, idx) => T2.CreateChecked(v) * T2.CreateChecked(currentValues[idx])).ToArray());
    }
}