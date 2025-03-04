using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using MeasurementTool.Devices;
using MeasurementTool.Interfaces.Emulators;

namespace MeasurementTool.Interfaces;

public class EmulatorInterface: IDeviceInterface
{
    private const int MaxChannels = 10;
    public const int MaxEventId = 9;

    private readonly Dictionary<byte, IDeviceEmulator> _devices;
    private readonly ILogger _logger;
    private int _eventId, _id;

    internal EmulatorInterface(ILogger logger)
    {
        _logger = logger;
        _devices = new Dictionary<byte, IDeviceEmulator>
        {
            //{ 1, new DdsEmulator(logger) },
            //{ 2, new MeterEmulator(logger) }
            { 2, new PwmEmulator(logger) }
        };
        _eventId = 0;
    }

    public byte[] SendCommand(byte[] command)
    {
        var cmd = command[0];
        switch (cmd)
        {
            case (byte)'n': return BuildEnumerationResponse();
            case (byte)'t': return TimeEvent();
            default:
                if (cmd < MaxChannels && _devices.TryGetValue(cmd, out var device))
                    return device.Command(command[1..]);
                _logger.Error("Invalid command " + BitConverter.ToString(command));
                return [(byte)'e'];
        }
    }

    private byte[] BuildEnumerationResponse()
    {
        return Enumerable.Range(0, MaxChannels)
            .SelectMany(v => BuildEnumerationResponse((byte)v))
            .ToArray();
    }

    private byte[] BuildEnumerationResponse(byte channel)
    {
        if (_devices.TryGetValue(channel, out var device))
        {
            var config = device.BuildConfiguration();
            var response = new List<byte>();
            var l = config.Length + 1;
            response.Add((byte)(l & 0xFF));
            response.Add((byte)((l>>8) & 0xFF));
            response.Add(device.GetId());
            response.AddRange(config);
            return response.ToArray();
        }
        return [0, 0];
    }
    
    private byte[] TimeEvent()
    {
        _id = _eventId;
        var rc = _devices
            .SelectMany(kvp => BuildTimeEventResponse(kvp.Key, kvp.Value.TimeEvent(GetEventId())))
            .ToList();
        rc.Insert(0, (byte)'k');
        if (_eventId == MaxEventId)
            _eventId = 0;
        else
            _eventId++;
        return rc.ToArray();
    }

    private int GetEventId()
    {
        var rc = _id;
        if (_id == MaxEventId)
            _id = 0;
        else
            _id++;
        return rc;
    }

    private static byte[] BuildTimeEventResponse(byte channel, byte[] response)
    {
        if (response.Length == 0)
            return response;
        var result = new byte[response.Length + 3];
        result[0] = channel;
        result[1] = (byte)(response.Length & 0xFF);
        result[2] = (byte)((response.Length >> 8) & 0xFF);
        response.CopyTo(result, 3);
        return result;
    }
}

internal interface IDeviceEmulator
{
    byte[] BuildConfiguration();
    byte[] Command(byte[] command);
    byte[] TimeEvent(int id);

    byte GetId();
}
