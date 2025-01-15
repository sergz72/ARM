using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using Avalonia.Controls;
using MeasurementTool.Interfaces;

namespace MeasurementTool.Devices;

public abstract class GenericDevice
{
    protected readonly DeviceManager Dm;
    protected readonly byte Channel;

    internal GenericDevice(DeviceManager dm, byte channel)
    {
        Dm = dm;
        Channel = channel;
    }

    internal virtual void Init()
    {
    }

    internal virtual void ParseResponse(byte[] data)
    {
        Dm.LogError(Channel, "Unexpected response from device");
    }

    internal virtual void TimerEvent(int eventId)
    {
    }
    
    internal abstract Control CreateUi();

    internal abstract string GetName();
}

public interface ILogger
{
    void Info(string deviceName, int channel, string message);
    void Info(string message);
    void Error(string deviceName, int channel, string message);
    void Error(string message);
}

public sealed class DeviceManager
{
    private const int MaxEvents = 10;

    private readonly IDeviceInterface _deviceInterface;
    private readonly Dictionary<int, GenericDevice> _channelToDevice;
    private readonly ILogger _logger;
    private int _eventId;
    
    public DeviceManager(IDeviceInterface di, ILogger logger)
    {
        _deviceInterface = di;
        _logger = logger;
        _channelToDevice = EnumerateDevices();
        _eventId = 0;
    }

    public Control[] CreateUi()
    {
        return _channelToDevice.Values.Select(d => d.CreateUi()).ToArray();
    }

    public void TimerEvent()
    {
        var response = SendCommand("t" + _eventId);
        if (_eventId == MaxEvents - 1)
            _eventId = 0;
        else
            _eventId++;
        ParseDeviceResponses(response);
        foreach (var device in _channelToDevice.Values)
            device.TimerEvent(_eventId);
    }

    internal void LogError(byte channel, string message)
    {
        _logger.Error(_channelToDevice[channel].GetName(), channel, message);
    }

    private void ParseDeviceResponses(byte[] response)
    {
        using var stream = new MemoryStream(response);
        using var br = new BinaryReader(stream);
        try
        {
            while (stream.Position < stream.Length)
            {
                int channel = br.ReadByte();
                int length = br.ReadInt16();
                var data = br.ReadBytes(length);
                if (_channelToDevice.TryGetValue(channel, out var device))
                    device.ParseResponse(data);
                else
                    _logger.Error("Unexpected response for channel " + channel);
            }
        }
        catch (Exception e)
        {
            _logger.Error(e.Message);
        }
    }
    
    internal byte[] SendCommand(byte channel, byte[] data)
    {
        var command = new byte[data.Length + 1];
        command[0] = channel;
        data.CopyTo(command, 1);
        return _deviceInterface.SendCommand(command);
    }

    internal byte[] SendCommand(byte channel, string command) => SendCommand(channel, Encoding.UTF8.GetBytes(command));
    
    private byte[] SendCommand(string command) => _deviceInterface.SendCommand(Encoding.UTF8.GetBytes(command));
    
    private Dictionary<int, GenericDevice> EnumerateDevices()
    {
        var response = SendCommand("n");
        using var stream = new MemoryStream(response);
        using var br = new BinaryReader(stream);
        var result = new Dictionary<int, GenericDevice>();
        byte channel = 0;
        while (stream.Position < stream.Length)
        {
            var deviceId = br.ReadInt16();
            if (deviceId != 0)
            {
                GenericDevice? device = null;
                switch (deviceId)
                {
                    case 1:
                        device = new Dds(this, channel);
                        break;
                    default:
                        _logger.Error("Unknown device id " + deviceId);
                        break;
                }
                if (device != null)
                {
                    result.Add(channel, device);
                    device.Init();
                }
            }
            channel++;
        }
        return result;
    }
}