using System;
using System.Collections.Generic;
using System.IO;
using System.Threading;
using Avalonia.Controls;
using CommunityToolkit.Mvvm.ComponentModel;
using MeasurementTool.Devices;
using MeasurementTool.Interfaces;
using System.Text.Json;

namespace MeasurementTool;

internal record Settings(string ComPortName);

internal partial class MainViewModel : ObservableObject, ILogger
{ 
    [ObservableProperty] private List<LogRecord> _log = [];
    private volatile bool _isShuttingDown;
    
    private readonly DeviceManager _deviceManager;
    private readonly Thread _deviceThread;
    private readonly Settings _settings;

    internal MainViewModel()
    {
        if (File.Exists("settings.json"))
        {
            var jsonString = File.ReadAllText("settings.json");
            _settings = JsonSerializer.Deserialize<Settings>(jsonString) ??
                        throw new Exception("Invalid settings file");
        }
        else
            _settings = new Settings("/dev/ttyUSB0");
        var iface = FindAvailableInterface();
        _deviceManager = new DeviceManager(iface, this);
        while (!_deviceManager.InitComplete())
            Thread.Sleep(100);
        _deviceThread = new Thread(DeviceLoop);
        _deviceThread.Start();
        _isShuttingDown = false;
    }

    public void Shutdown()
    {
        _isShuttingDown = true;
        _deviceThread.Join();
        _deviceManager.Shutdown();
    }

    private void DeviceLoop()
    {
        while (!_isShuttingDown)
        {
            var start = DateTime.Now.Ticks;
            _deviceManager.TimerEvent();
            var end = DateTime.Now.Ticks;
            var elapsed = (end - start) / TimeSpan.TicksPerMillisecond;
            if (elapsed < 100)
                Thread.Sleep(100 - (int)elapsed);
        }
    }

    private IDeviceInterface FindAvailableInterface()
    {
        var iface = InitComPort();
        if (iface == null)
        {
            Warning("No communication ports found");
            Info("Using device emulator");
            return new EmulatorInterface(this);
        }
        return iface;
    }

    private IDeviceInterface? InitComPort()
    {
        if (File.Exists(_settings.ComPortName))
            return new SerialInterface(_settings.ComPortName);
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