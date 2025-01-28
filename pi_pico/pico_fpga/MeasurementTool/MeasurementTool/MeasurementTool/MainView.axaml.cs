using System;
using System.Linq;
using Avalonia;
using Avalonia.Controls;
using Avalonia.Interactivity;
using Avalonia.Layout;
using MeasurementTool.Devices;

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
            var controls = vm.CreateUi().Select(BuildChannelControl);
            DevicePanel.Children.AddRange(controls);
        }        
    }

    private Control BuildChannelControl(DeviceControl control)
    {
        var panel = new StackPanel
        {
            Orientation = Orientation.Vertical
        };
        panel.Children.Add(new Label{Content = $"Channel {control.Channel}: {control.Name}",
                                        HorizontalAlignment = HorizontalAlignment.Center});
        panel.Children.Add(control.DControl!);
        return panel;
    }

    private void EepromRead_OnClick(object? sender, RoutedEventArgs e)
    {
        EepromWindow.Show(false);
    }

    private void EepromWrite_OnClick(object? sender, RoutedEventArgs e)
    {
        EepromWindow.Show(true);
    }
}