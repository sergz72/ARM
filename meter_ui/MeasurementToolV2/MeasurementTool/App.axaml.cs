using Avalonia;
using Avalonia.Controls.ApplicationLifetimes;
using Avalonia.Markup.Xaml;
using MeasurementTool.Devices.Controls;

namespace MeasurementTool;

public partial class App : Application
{
    public override void Initialize()
    {
        AvaloniaXamlLoader.Load(this);
    }

    public override void OnFrameworkInitializationCompleted()
    {
        if (ApplicationLifetime is IClassicDesktopStyleApplicationLifetime desktop)
        {
            desktop.MainWindow = new MainWindow
            {
                DataContext = new MainViewModel()
            };
            MessageBox.MainWindow = desktop.MainWindow;
            EepromWindow.MainWindow = desktop.MainWindow;
        }

        base.OnFrameworkInitializationCompleted();
    }
}