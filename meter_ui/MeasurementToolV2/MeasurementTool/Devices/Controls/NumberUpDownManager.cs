using System;
using Avalonia.Controls;
using Avalonia.Interactivity;

namespace MeasurementTool.Devices.Controls;

public class NumberUpDownManager: Interactive
{
    private readonly NumberUpDown[] _upDowns;
    private readonly Label? _signLabel; 
    private readonly long _minValue;
    private readonly long _maxValue;
    private long _value;

    public static readonly RoutedEvent<RoutedEventArgs> ValueChangeEvent =
        RoutedEvent.Register<NumberUpDownManager, RoutedEventArgs>(nameof(ValueChanged), RoutingStrategies.Direct);
    
    public event EventHandler<RoutedEventArgs> ValueChanged
    {
        add => AddHandler(ValueChangeEvent, value);
        remove => RemoveHandler(ValueChangeEvent, value);
    }    
    
    public long Value {
        get => _value;
        set {
            if (value < _minValue || value > _maxValue)
                return;
            _value = value;
            UpdateSign();
            foreach (var ud in _upDowns)
            {
                ud.Value = (int)(value % 10);
                value /= 10;
            }
        }
    }
    
    public NumberUpDownManager(long minValue, long maxValue, long value, Label? signLabel, params NumberUpDown[] upDowns)
    {
        _minValue = minValue;
        _maxValue = maxValue;
        _upDowns = upDowns;
        _signLabel = signLabel;
        Value = value;
        foreach (var ud in upDowns)
            ud.ValueChanged += UdOnValueChanged;
        UpdateSign();
    }

    private void UdOnValueChanged(object? sender, ValueChangeEventArgs e)
    {
        long adder = 1;
        for (int i = 0; i < _upDowns.Length; i++)
        {
            var ud = _upDowns[i];
            if (ud == sender)
            {
                var newValue = _value + adder * e.Delta;
                e.Handled = newValue >= _minValue && newValue <= _maxValue;
                if (e.Handled)
                {
                    UpdateSign();
                    _value = newValue;
                    var args = new RoutedEventArgs(ValueChangeEvent);
                    RaiseEvent(args);
                    if (UpdateNext(ud, e.Delta, i))
                        UpdateNext(i + 1, e.Delta);
                }
                return;
            }
            adder *= 10;
        }
    }

    private bool UpdateNext(NumberUpDown ud, int delta, int i)
    {
        return ((ud.Value == 9 && delta > 0) || (ud.Value == 0 && delta < 0)) && i < _upDowns.Length - 1;
    }
    
    private void UpdateNext(int n, int delta)
    {
        var ud = _upDowns[n];
        var updateNext = UpdateNext(ud, delta, n);
        ud.Update(delta);
        if (updateNext)
            UpdateNext(n + 1, delta);
    }

    private void UpdateSign()
    {
        if (_signLabel != null) _signLabel.Content = _value < 0 ? "-" : " ";
    }
}