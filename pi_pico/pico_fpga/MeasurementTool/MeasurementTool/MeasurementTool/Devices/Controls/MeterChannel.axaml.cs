using System;
using System.Linq;
using Avalonia;
using Avalonia.Controls;

namespace MeasurementTool.Devices.Controls;

public partial class MeterChannel : UserControl
{
    private readonly SevenSegControl _signControl;
    private readonly SevenSegControl[] _valueControls;
    
    internal long Value
    {
        set
        {
            _signControl.Value = value >= 0 ? SevenSegControl.Space : SevenSegControl.Minus;
            if (value < 0)
                value = -value;
            for (var i = _valueControls.Length - 1; i >= 0; i--)
            {
                _valueControls[i].Value = (int)(value % 10);
                value /= 10;
            }
        }
    }
    
    public MeterChannel(): this(MeterMeasureType.Voltage, MeterReturnValueType.Micros, 1)
    {
    }
    
    internal MeterChannel(MeterMeasureType measureType, MeterReturnValueType returnValueType, int numbersBeforePoint)
    {
        InitializeComponent();
        _signControl = new SevenSegControl { PointSymbol = " ", Value = SevenSegControl.Space };
        ValuePanel.Children.Add(_signControl); //sign
        _valueControls = Enumerable.Range(0, numbersBeforePoint + (int)returnValueType * 3)
            .Select(n => new SevenSegControl { PointSymbol = GetPointSymbol(n, numbersBeforePoint), Value = 0 })
            .ToArray();
        ValuePanel.Children.AddRange(_valueControls);
        UnitsLabel.Content = GetMeasureTypeSymbol(measureType);
    }

    private static string GetMeasureTypeSymbol(MeterMeasureType measureType)
    {
        switch (measureType)
        {
            case MeterMeasureType.Voltage:
                return "V";
            case MeterMeasureType.Current:
                return "A";
            case MeterMeasureType.Resistance:
                return "Ohm";
            case MeterMeasureType.Capacitance:
                return "F";
            case MeterMeasureType.Inductance:
                return "H";
            case MeterMeasureType.Temperature:
                return "\u00b0C";
            case MeterMeasureType.Frequency:
                return "Hz";
            case MeterMeasureType.Power:
                return "W";
            default:
                throw new ArgumentOutOfRangeException(nameof(measureType), measureType, null);
        }
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