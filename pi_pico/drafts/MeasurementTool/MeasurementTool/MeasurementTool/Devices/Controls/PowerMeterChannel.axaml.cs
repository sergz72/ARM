using Avalonia;
using Avalonia.Controls;

namespace MeasurementTool.Devices.Controls;

public partial class PowerMeterChannel : UserControl
{
    private readonly MeterChannel _voltageChannel, _currentChannel, _powerChannel;
    
    public PowerMeterChannel(): this(MeterReturnValueType.Millis, 2, MeterReturnValueType.Millis, 1)
    {
    }

    internal PowerMeterChannel(MeterReturnValueType voltageValueType, int voltageNumbersBeforePoint,
        MeterReturnValueType currentValueType, int currentNumbersBeforePoint)
    {
        InitializeComponent();
        _voltageChannel = new MeterChannel(MeterMeasureType.Voltage, voltageValueType, voltageNumbersBeforePoint) {Margin = new Thickness(3,3)};
        _currentChannel = new MeterChannel(MeterMeasureType.Current, currentValueType, currentNumbersBeforePoint) {Margin = new Thickness(3,3)};
        _powerChannel = new MeterChannel(MeterMeasureType.Power, (int)voltageValueType + currentValueType,
            currentNumbersBeforePoint + voltageNumbersBeforePoint) {Margin = new Thickness(3,3)};
        ValuePanel.Children.Add(_voltageChannel);
        ValuePanel.Children.Add(_currentChannel);
        ValuePanel.Children.Add(_powerChannel);
    }

    internal void SetValues(long voltage, long current)
    {
        _voltageChannel.Value = voltage;
        _currentChannel.Value = current;
        _powerChannel.Value = voltage * current;
    }
}