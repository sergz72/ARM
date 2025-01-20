using Avalonia;
using Avalonia.Controls;

namespace MeasurementTool.Devices.Controls;

public partial class MeterChannel : UserControl
{
    internal long Value
    {
        set
        {
            
        }
    }
    
    public MeterChannel()
    {
        InitializeComponent();
        ValuePanel.Children.Add(new SevenSegControl { PointSymbol = " " });
    }
    
    internal MeterChannel(MeterMeasureType measureType, MeterReturnValueType returnValueType, int numbersBeforePoint)
    {
        InitializeComponent();
        ValuePanel.Children.Add(new SevenSegControl { PointSymbol = " ", Value = -1});
    }
}