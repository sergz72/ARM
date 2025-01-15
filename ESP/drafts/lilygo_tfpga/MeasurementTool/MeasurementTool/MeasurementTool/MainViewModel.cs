using System;
using System.Collections.Generic;
using Avalonia.Controls;
using CommunityToolkit.Mvvm.ComponentModel;
using MeasurementTool.Devices;
using MeasurementTool.Interfaces;

namespace MeasurementTool;

internal partial class MainViewModel : ObservableObject, ILogger
{ 
    [ObservableProperty] private List<LogRecord> _log = [];
    
    private readonly DeviceManager _deviceManager;

    internal MainViewModel()
    {
        var iface = FindAvailableInterface();
        _deviceManager = new DeviceManager(iface, this);
    }

    private IDeviceInterface FindAvailableInterface()
    {
        var iface = SearchForComPorts();
        if (iface == null)
        {
            Warning("No communication ports found");
            Info("Using device emulator");
            return new EmulatorInterface(this);
        }
        return iface;
    }

    private static IDeviceInterface? SearchForComPorts()
    {
        return null;
    }

    internal Control[] CreateUi() => _deviceManager.CreateUi();
    
    public void Error(string deviceName, int channel, string message)
    {
        Log.Add(new LogRecord(DateTime.Now, LogLevel.Error, deviceName, channel, message));
    }

    public void Warning(string deviceName, int channel, string message)
    {
        Log.Add(new LogRecord(DateTime.Now, LogLevel.Warning, deviceName, channel, message));
    }
    
    public void Info(string deviceName, int channel, string message)
    {
        Log.Add(new LogRecord(DateTime.Now, LogLevel.Info, deviceName, channel, message));
    }
    
    public void Error(string message)
    {
        Log.Add(new LogRecord(DateTime.Now, LogLevel.Error, "", 0, message));
    }

    public void Warning(string message)
    {
        Log.Add(new LogRecord(DateTime.Now, LogLevel.Warning, "", 0, message));
    }
    
    public void Info(string message)
    {
        Log.Add(new LogRecord(DateTime.Now, LogLevel.Info, "", 0, message));
    }
}

internal enum LogLevel
{
    Debug,
    Info,
    Warning,
    Error
}

internal record LogRecord(DateTime Time, LogLevel Level, string DeviceName, int Channel, string Message)
{
    public override string ToString()
    {
        if (DeviceName == "")
            return $"{Time:HH:mm:ss} {Level} {Message}";
        return $"{Time:HH:mm:ss} {Level} {DeviceName} {Channel} {Message}";
    }
}