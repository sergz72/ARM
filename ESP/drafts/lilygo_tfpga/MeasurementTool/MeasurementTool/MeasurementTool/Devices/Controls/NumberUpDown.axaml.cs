using System;
using Avalonia.Controls;
using Avalonia.Interactivity;

namespace MeasurementTool.Devices.Controls;

public partial class NumberUpDown : UserControl
{
    private int _value;

    public int MinValue { get; set; } = 0;
    public int MaxValue { get; set; } = 9;
    public int Value
    {
        get => _value;
        set
        {
            if (value >= MinValue && value <= MaxValue)
                SetValue(value);
        }
    }

    private void SetValue(int value)
    {
        _value = value;
        LValue.Content = _value.ToString();
    }
    
    public static readonly RoutedEvent<ValueChangeEventArgs> ValueChangeEvent =
        RoutedEvent.Register<NumberUpDown, ValueChangeEventArgs>(nameof(ValueChanged), RoutingStrategies.Direct);

    public event EventHandler<ValueChangeEventArgs> ValueChanged
    {
        add => AddHandler(ValueChangeEvent, value);
        remove => RemoveHandler(ValueChangeEvent, value);
    }    
    
    public NumberUpDown()
    {
        InitializeComponent();
        SetValue(MinValue);
    }

    private void Up_OnClick(object? sender, RoutedEventArgs e)
    {
        if (_value < MaxValue)
        {
            var args = new ValueChangeEventArgs(ValueChangeEvent, 1);
            RaiseEvent(args);
            if (args.Handled)
                SetValue(_value + 1);
        }
    }

    private void Down_OnClick(object? sender, RoutedEventArgs e)
    {
        if (_value > MinValue)
        {
            var args = new ValueChangeEventArgs(ValueChangeEvent, -1);
            RaiseEvent(args);
            if (args.Handled)
                SetValue(_value - 1);
        }
    }
}

public class ValueChangeEventArgs(RoutedEvent? routedEvent, int delta) : RoutedEventArgs(routedEvent)
{
    public int Delta { get; init; } = delta;
}