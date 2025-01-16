using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
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
    
    internal abstract Control? CreateUi();

    internal abstract string GetName();
}

public interface ILogger
{
    void Info(string deviceName, int channel, string message);
    void Info(string message);
    void Error(string deviceName, int channel, string message);
    void Error(string message);
}

internal record Message(byte[] Data, Action<byte[]?> Handler);

public sealed class DeviceManager
{
    private const int MaxEvents = 10;

    private readonly IDeviceInterface _deviceInterface;
    private Dictionary<int, GenericDevice>? _channelToDevice;
    private readonly ILogger _logger;
    private int _eventId;
    private readonly Queue<Message> _messages;
    private readonly Thread _messageThread;
    private volatile bool _isShuttingDown;
    
    public DeviceManager(IDeviceInterface di, ILogger logger)
    {
        _deviceInterface = di;
        _logger = logger;
        _eventId = 0;
        _messages = new Queue<Message>();
        _isShuttingDown = false;
        _messageThread = new Thread(ProcessMessages);
        _messageThread.Start();
        EnumerateDevices();
    }

    public void Shutdown()
    {
        _isShuttingDown = true;
        _messageThread.Join();
    }

    private void ProcessMessages()
    {
        while (!_isShuttingDown)
        {
            if (_messages.TryDequeue(out var message))
            {
                try
                {
                    var response = _deviceInterface.SendCommand(message.Data);
                    message.Handler.Invoke(response);
                }
                catch (Exception ex)
                {
                    _logger.Error(ex.Message);
                    message.Handler.Invoke(null);
                }
            }
            else
                Thread.Sleep(10);
        }
    }

    public Control[] CreateUi()
    {
        if (_channelToDevice == null)
            return [];
        return _channelToDevice.Values
            .Select(d => d.CreateUi())
            .Where(ui => ui != null)
            .Select(ui => ui!)
            .ToArray();
    }

    public void TimerEvent()
    {
        if (_channelToDevice == null)
            return;
        while (_messages.Count > 0)
            Thread.Sleep(10);
        QueueCommand("t" + _eventId, ParseDeviceResponses);
        if (_eventId == MaxEvents - 1)
            _eventId = 0;
        else
            _eventId++;
        foreach (var device in _channelToDevice.Values)
            device.TimerEvent(_eventId);
    }

    internal void LogError(byte channel, string message)
    {
        if (_channelToDevice == null)
            return;
        _logger.Error(_channelToDevice[channel].GetName(), channel, message);
    }

    private void ParseDeviceResponses(byte[]? response)
    {
        if (_channelToDevice == null || response == null)
            return;
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
    
    internal void QueueCommand(byte channel, byte[] data, Action<byte[]?> handler)
    {
        var command = new byte[data.Length + 1];
        command[0] = channel;
        data.CopyTo(command, 1);
        _messages.Enqueue(new Message(command, handler));
    }

    internal void QueueCommand(byte channel, string command, Action<byte[]?> handler) =>
        QueueCommand(channel, Encoding.UTF8.GetBytes(command), handler);
    
    private void QueueCommand(string command, Action<byte[]?> handler) =>
        _messages.Enqueue(new Message(Encoding.UTF8.GetBytes(command), handler));

    private void ProcessEnumerationResult(byte[]? data)
    {
        if (data == null)
        {
            _channelToDevice = [];
            return;
        }
        using var stream = new MemoryStream(data);
        using var br = new BinaryReader(stream);
        var channelToDevice = new Dictionary<int, GenericDevice>();
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
                    channelToDevice.Add(channel, device);
                    device.Init();
                }
            }
            channel++;
        }
        _channelToDevice = channelToDevice;
    }
    
    private void EnumerateDevices()
    {
        QueueCommand("n", ProcessEnumerationResult);
    }

    public bool InitComplete()
    {
        return _channelToDevice != null;
    }
}
