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

    internal virtual void ParseResponse(byte[] data)
    {
        Dm.LogError(Channel, "Unexpected response from device");
    }

    internal virtual void TimerEvent()
    {
    }
    
    internal abstract Control? CreateUi();

    internal abstract string GetName();
    
    protected void CheckError(byte[] request, byte[]? response)
    {
        if (response == null)
            return;
        if (response.Length == 0)
            Dm.LogError(Channel, "Empty response");
        if (response[0] != (byte)'k')
            Dm.LogError(Channel, (char)request[1] + " " + Encoding.UTF8.GetString(response));
    }
}

public interface ILogger
{
    void Info(string deviceName, int channel, string message);
    void Info(string message);
    void Error(string deviceName, int channel, string message);
    void Error(string message);
}

internal record Message(byte[] Data, Action<byte[], byte[]?> Handler);

public sealed class DeviceManager
{
    private readonly IDeviceInterface _deviceInterface;
    private Dictionary<int, GenericDevice>? _channelToDevice;
    private readonly ILogger _logger;
    private readonly Queue<Message> _messages;
    private readonly Thread _messageThread;
    private volatile bool _isShuttingDown;
    
    internal int MaxDeviceId { get; private set; }
    
    public DeviceManager(IDeviceInterface di, ILogger logger)
    {
        _deviceInterface = di;
        _logger = logger;
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
                    message.Handler.Invoke(message.Data, response);
                }
                catch (Exception ex)
                {
                    _logger.Error(ex.Message);
                    message.Handler.Invoke(message.Data, null);
                }
            }
            else
                Thread.Sleep(10);
        }
    }

    public DeviceControl[] CreateUi()
    {
        if (_channelToDevice == null)
            return [];
        return _channelToDevice
            .Select(cd => new DeviceControl(cd.Value.CreateUi(), cd.Key, cd.Value.GetName()))
            .Where(dc => dc.DControl != null)
            .ToArray();
    }

    public void TimerEvent()
    {
        if (_channelToDevice == null)
            return;
        while (_messages.Count > 0)
            Thread.Sleep(10);
        QueueCommand("t", ParseDeviceResponses);
        foreach (var device in _channelToDevice.Values)
            device.TimerEvent();
    }

    internal void LogError(byte channel, string message)
    {
        if (_channelToDevice == null)
            return;
        _logger.Error(_channelToDevice[channel].GetName(), channel, message);
    }

    private void ParseDeviceResponses(byte[] request, byte[]? response)
    {
        if (_channelToDevice == null || response == null)
            return;
        using var stream = new MemoryStream(response);
        using var br = new BinaryReader(stream);
        try
        {
            var status = (char)br.ReadByte();
            if (status != 'k')
            {
                _logger.Error($"Non-ok status code from device: {status}");
                return;
            }
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
    
    internal void QueueCommand(byte channel, byte[] data, Action<byte[], byte[]?> handler)
    {
        var command = new byte[data.Length + 1];
        command[0] = channel;
        data.CopyTo(command, 1);
        _messages.Enqueue(new Message(command, handler));
    }

    internal void QueueCommand(byte channel, string command, Action<byte[], byte[]?> handler) =>
        QueueCommand(channel, Encoding.UTF8.GetBytes(command), handler);
    
    private void QueueCommand(string command, Action<byte[], byte[]?> handler) =>
        _messages.Enqueue(new Message(Encoding.UTF8.GetBytes(command), handler));

    internal void QueueCommand(byte[] data, Action<byte[], byte[]?> handler) => _messages.Enqueue(new Message(data, handler));
    
    private void ProcessEnumerationResult(byte[] request, byte[]? data)
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
        MaxDeviceId = -1;
        while (stream.Position < stream.Length)
        {
            MaxDeviceId++;
            var length = br.ReadInt16();
            if (length > 0)
            {
                var deviceId = br.ReadByte();
                var config = br.ReadBytes(length - 1);
                GenericDevice? device = null;
                try
                {
                    switch ((DeviceType)deviceId)
                    {
                        case DeviceType.Dds:
                            device = new Dds(this, config, channel);
                            break;
                        case DeviceType.Meter:
                            device = new Meter(this, config, channel);
                            break;
                        case DeviceType.PowerMeter:
                            device = new PowerMeter(this, config, channel);
                            break;
                        case DeviceType.Pwm:
                            device = new Pwm(this, config, channel);
                            break;
                        default:
                            _logger.Error("Unknown device id " + deviceId);
                            break;
                    }
                    if (device != null)
                        channelToDevice.Add(channel, device);
                }
                catch (Exception e)
                {
                    _logger.Error(e.Message);
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

internal enum DeviceType
{
    Dds = 1,
    Meter,
    PowerMeter,
    Pwm
}

public record DeviceControl(Control? DControl, int Channel, string Name);