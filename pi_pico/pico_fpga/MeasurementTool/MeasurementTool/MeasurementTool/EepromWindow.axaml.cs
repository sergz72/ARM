using System.Threading.Tasks;
using Avalonia;
using Avalonia.Controls;
using Avalonia.Markup.Xaml;
using MeasurementTool.Devices.Controls;

namespace MeasurementTool;

public partial class EepromWindow : Window
{
    public static Window? MainWindow { get; set; }

    public EepromWindow()
    {
        InitializeComponent();
    }
    
    public async static Task Show(bool write)
    {
        if (MainWindow != null)
        {
            var window = new EepromWindow();
            await window.ShowDialog(MainWindow);
        }
    }
}