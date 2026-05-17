using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Threading.Tasks;
using Avalonia;
using Avalonia.Controls;
using Avalonia.Input;
using Avalonia.Interactivity;
using Avalonia.Markup.Xaml;
using Avalonia.Threading;
using MeasurementTool.Devices;
using MeasurementTool.Devices.Controls;

namespace MeasurementTool;

public partial class EepromWindow : Window
{
    public static Window? MainWindow { get; set; }
    private readonly DeviceManager? _dm;
    private byte[] _writeData;

    public EepromWindow()
    {
        InitializeComponent();
        _dm = null;
        _writeData = [];
    }
    
    public EepromWindow(DeviceManager dm)
    {
        InitializeComponent();
        NuDeviceId.Maximum = dm.MaxDeviceId;
        _dm = dm;
        _writeData = [];
    }
    
    public async static Task Show(DeviceManager dm)
    {
        if (MainWindow != null)
        {
            var window = new EepromWindow(dm);
            await window.ShowDialog(MainWindow);
        }
    }

    private void BnRead_OnClick(object? sender, RoutedEventArgs e)
    {
        var l = (int)NuBytes.Value!;
        _dm?.QueueCommand([(byte)'r', (byte)(l & 0xFF), (byte)((l >> 8) & 0xFF), (byte)NuDeviceId.Value!],
                            ResponseHandler);
    }
    
    private void BnWrite_OnClick(object? sender, RoutedEventArgs e)
    {
        using var ms = new MemoryStream();
        using var bw = new BinaryWriter(ms);
        var l = _writeData.Length + 1;
        bw.Write((byte)'w');
        bw.Write((byte)(l & 0xFF));
        bw.Write((byte)((l >> 8) & 0xFF));
        bw.Write((byte)NuDeviceId.Value!);
        bw.Write(_writeData);
        _dm?.QueueCommand(ms.ToArray(), ResponseHandler);
    }

    private void ResponseHandler(byte[] request, byte[]? data)
    {
        var s = data is { Length: > 0 } ? BitConverter.ToString(data) : string.Empty;
        var sc = data is { Length: > 0 } ?
            new string(data.Select(b => b is >= 0x20 and <= 0x7F ? (char)b : '?').ToArray()) : string.Empty;
        Dispatcher.UIThread.InvokeAsync(() =>
        {
            TbResponse.Text = s;
            TbResponseChars.Text = sc;
        });
    }
    
    private void TbData_OnTextChanged(object? sender, TextChangedEventArgs e)
    {
        BnWrite.IsEnabled = BuildWriteData();
    }

    private bool BuildWriteData()
    {
        var text = TbData.Text!;
        if (text.Length == 0 || !text.All(c => char.IsAsciiHexDigit(c) || c == ' '))
            return false;
        var parts = text.Split(' ', StringSplitOptions.RemoveEmptyEntries);
        if (parts.Any(p => p.Length % 2 != 0))
            return false;
        _writeData = parts.SelectMany(BuildWriteData).ToArray();
        return true;
    }

    private static IEnumerable<byte> BuildWriteData(string s)
    {
        return Enumerable.Range(0, s.Length)
            .Where(x => x % 2 == 0)
            .Select(x => Convert.ToByte(s.Substring(x, 2), 16));
    }
}