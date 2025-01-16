using System.Threading.Tasks;
using Avalonia;
using Avalonia.Controls;
using Avalonia.Interactivity;
using Avalonia.Markup.Xaml;

namespace MeasurementTool.Devices.Controls;

public partial class MessageBox : Window
{
    public static Window? MainWindow { get; set; }
    
    public MessageBox(string title, string text)
    {
        InitializeComponent();
        LText.Content = text;
        Title = title;
    }

    private void Button_OnClick(object? sender, RoutedEventArgs e)
    {
        Close();
    }

    public async static Task Show(string title, string text)
    {
        if (MainWindow != null)
        {
            var window = new MessageBox(title, text);
            await window.ShowDialog(MainWindow);
        }
    }
}