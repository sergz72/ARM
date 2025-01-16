using System;
using Avalonia.Controls;

namespace MeasurementTool.Views;

public partial class MainWindow : Window
{
    public MainWindow()
    {
        InitializeComponent();
    }

    protected override void OnClosed(EventArgs e)
    {
        base.OnClosed(e);
        if (DataContext is MainViewModel vm)
            vm.Shutdown();
    }
}