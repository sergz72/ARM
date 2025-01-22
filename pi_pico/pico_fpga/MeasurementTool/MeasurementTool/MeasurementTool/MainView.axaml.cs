using System;
using Avalonia.Controls;

namespace MeasurementTool;

public partial class MainView : UserControl
{
    public MainView()
    {
        InitializeComponent();
    }

    protected override void OnDataContextChanged(EventArgs e)
    {
        if (DataContext is MainViewModel vm)
        {
            var controls = vm.CreateUi();
            DevicePanel.Children.AddRange(controls);
        }        
    }
}