using System;
using System.Linq;
using System.Numerics;
using Avalonia.Controls;
using Avalonia.Layout;

namespace MeasurementTool.Devices.Controls;

public class MeterChannel<T> : UserControl where T: INumber<T>
{
    private static readonly T Ten = T.CreateChecked(10);

    private readonly SevenSegControl _signControl;
    private readonly SevenSegControl[] _valueControls;
    private readonly Graph<T>? _graph;
    private readonly int _measurementPeriodMs;
    private readonly MeterReturnValueType _returnValueType;
    
    internal void AddValues(T[] values)
    {
        if (values.Length == 0)
            return;
        var value = values.Last();
        _signControl.Value = value >= T.Zero ? SevenSegControl.Space : SevenSegControl.Minus;
        if (value < T.Zero)
            value = -value;
        for (var i = _valueControls.Length - 1; i >= 0; i--)
        {
            _valueControls[i].Value = int.CreateChecked(value % Ten);
            value /= Ten;
        }
        if (_graph != null)
            _graph.AddValues(values);
    }
    
    public MeterChannel(): this(MeterMeasureType.Voltage, MeterReturnValueType.Micros, 1, 0)
    {
    }
    
    internal MeterChannel(MeterMeasureType measureType, MeterReturnValueType returnValueType, int numbersBeforePoint, int measurementPeriodMs)
    {
        var verticalPanel = new StackPanel
        {
            Orientation = Orientation.Vertical,
            Spacing = 3
        };
        var horizontalPanel = new StackPanel
        {
            Orientation = Orientation.Horizontal,
            Spacing = 3,
            Height = 40
        };
        var valuePanel = new StackPanel
        {
            Orientation = Orientation.Horizontal,
            Spacing = 3
        };
        var unitsLabel = new Label
        {
            FontSize = 30
        };
        horizontalPanel.Children.Add(valuePanel);
        horizontalPanel.Children.Add(unitsLabel);
        verticalPanel.Children.Add(horizontalPanel);
        Content = verticalPanel;
        
        _returnValueType = returnValueType;
        _measurementPeriodMs = measurementPeriodMs;
        _signControl = new SevenSegControl { PointSymbol = " ", Value = SevenSegControl.Space };
        valuePanel.Children.Add(_signControl); //sign
        _valueControls = Enumerable.Range(0, numbersBeforePoint + (int)returnValueType * 3)
            .Select(n => new SevenSegControl { PointSymbol = GetPointSymbol(n, numbersBeforePoint), Value = 0 })
            .ToArray();
        valuePanel.Children.AddRange(_valueControls);
        unitsLabel.Content = GetMeasureTypeSymbol(measureType);
        if (measurementPeriodMs != 0)
        {
            _graph = new Graph<T>(x => TimeSpan.FromMilliseconds(x * _measurementPeriodMs).ToString(@"hh\:mm\:ss.fff}"),
                y => MeterReturnValueTypeMethods<T>.FormatY(returnValueType, y))
            {
                MinHeight = 200
            };
            verticalPanel.Children.Add(_graph);
        }
    }
    
    private static string GetMeasureTypeSymbol(MeterMeasureType measureType)
    {
        return measureType switch
        {
            MeterMeasureType.Voltage => "V",
            MeterMeasureType.Current => "A",
            MeterMeasureType.Resistance => "Ohm",
            MeterMeasureType.Capacitance => "F",
            MeterMeasureType.Inductance => "H",
            MeterMeasureType.Temperature => "\u00b0C",
            MeterMeasureType.Frequency => "Hz",
            MeterMeasureType.Power => "W",
            _ => throw new ArgumentOutOfRangeException(nameof(measureType), measureType, null)
        };
    }

    private static string GetPointSymbol(int idx, int numbersBeforePoint)
    {
        if (idx == numbersBeforePoint - 10)
            return "G";
        if (idx == numbersBeforePoint - 7)
            return "M";
        if (idx == numbersBeforePoint - 4)
            return "K";
        if (idx == numbersBeforePoint - 1)
            return ".";
        if (idx == numbersBeforePoint + 2)
            return "m";
        if (idx == numbersBeforePoint + 5)
            return "u";
        if (idx == numbersBeforePoint + 8)
            return "n";
        if (idx == numbersBeforePoint + 11)
            return "p";
        return " ";
    }
}