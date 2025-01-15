using Avalonia.Interactivity;
using Avalonia.Styling;

namespace MeasurementTool.Devices.Controls;

public class NumberUpDownManager
{
    private readonly NumberUpDown[] _upDowns;
    private readonly long _minValue;
    private readonly long _maxValue;
    private long _value;
    
    public NumberUpDownManager(long minValue, long maxValue, long value, params NumberUpDown[] upDowns)
    {
        _minValue = minValue;
        _maxValue = maxValue;
        _upDowns = upDowns;
        SetValue(value);
        foreach (var ud in upDowns)
            ud.ValueChanged += UdOnValueChanged;
    }

    private void UdOnValueChanged(object? sender, ValueChangeEventArgs e)
    {
        long adder = 1;
        foreach (var ud in _upDowns)
        {
            if (ud == sender)
            {
                var newValue = _value + adder * e.Delta;
                e.Handled = newValue >= _minValue && newValue <= _maxValue;
                if (e.Handled)
                    _value = newValue;
                return;
            }
            adder *= 10;
        }
    }

    public void SetValue(long value)
    {
        if (value < _minValue || value > _maxValue)
            return;
        _value = value;
        foreach (var ud in _upDowns)
        {
            ud.Value = (int)(value % 10);
            value /= 10;
        }
    }
}