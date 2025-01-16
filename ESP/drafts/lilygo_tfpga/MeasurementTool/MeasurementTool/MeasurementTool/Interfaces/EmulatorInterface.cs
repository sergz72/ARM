using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using MeasurementTool.Devices;

namespace MeasurementTool.Interfaces;

public class EmulatorInterface: IDeviceInterface
{
    private const int MaxChannels = 10;

    private readonly Dictionary<byte, IDeviceEmulator> _devices;
    private readonly ILogger _logger;

    internal EmulatorInterface(ILogger logger)
    {
        _logger = logger;
        _devices = new Dictionary<byte, IDeviceEmulator>
        {
            { 1, new DDSEmulator(logger) }
        };
    }

    public byte[] SendCommand(byte[] command)
    {
        var cmd = command[0];
        switch (cmd)
        {
            case (byte)'n': return BuildEnumerationResponse();
            case (byte)'t': return TimeEvent(command[1]);
            default:
                if (cmd < MaxChannels && _devices.TryGetValue(cmd, out var device))
                    return device.Command(command[1..]);
                _logger.Error("Invalid command " + cmd);
                return [(byte)'e'];
        }
    }

    private byte[] BuildEnumerationResponse()
    {
        return Enumerable.Range(0, MaxChannels)
            .SelectMany(v => BitConverter.GetBytes(BuildEnumerationResponse((byte)v)))
            .ToArray();
    }

    private short BuildEnumerationResponse(byte channel)
    {
        if (_devices.TryGetValue(channel, out var device))
            return device.GetId();
        return 0;
    }
    
    private byte[] TimeEvent(int id)
    {
        return _devices
            .SelectMany(kvp => BuildTimeEventResponse(kvp.Key, kvp.Value.TimeEvent(id)))
            .ToArray();
    }

    private static byte[] BuildTimeEventResponse(byte channel, byte[] response)
    {
        if (response.Length == 0)
            return response;
        var result = new byte[response.Length + 1];
        result[0] = channel;
        response.CopyTo(result, 1);
        return result;
    }
}

internal interface IDeviceEmulator
{
    byte[] Command(byte[] command);
    byte[] TimeEvent(int id);

    short GetId();
}

internal sealed class DDSEmulator(ILogger logger) : IDeviceEmulator
{
    private static void BuildSi5351Capabilities(BinaryWriter bw)
    {
        bw.Write((short)DdsType.Si5351);
        bw.Write((byte)0); //minDb
        bw.Write((byte)0); //maxDb
        bw.Write(27000000); //mClk
        bw.Write((short)3300); //maxMv
        bw.Write((byte)0); //maxAttenuator
    }
    
    public byte[] Command(byte[] command)
    {
        switch (command[0])
        {
            case (byte)DdsCommands.GetCapabilities:
            {
                logger.Info($"DDS get capabilities command");
                using var ms = new MemoryStream();
                using var bw = new BinaryWriter(ms);
                BuildSi5351Capabilities(bw);
                return ms.ToArray();
            }
            case (byte)DdsCommands.EnableOutput:
                logger.Info($"DDS enable output command channel={command[1]} enable={command[2]}");
                return [(byte)'k'];
            case (byte)DdsCommands.SetFrequency:
            {
                using var ms = new MemoryStream(command[2..]);
                using var br = new BinaryReader(ms);
                var frequency = br.ReadInt64();
                var divider = br.ReadInt16();
                logger.Info($"DDS set frequency command channel={command[1]} frequency={frequency} divider={divider}");
                return [(byte)'k'];
            }
            case (byte)DdsCommands.SetAttenuator:
                logger.Info($"DDS set attenuator command channel={command[1]} value={command[2]}");
                return [(byte)'k'];
            case (byte)DdsCommands.SetMode:
                logger.Info($"DDS set mode command channel={command[1]} mode={command[2]}");
                return [(byte)'k'];
            case (byte)DdsCommands.Sweep:
            {
                using var ms = new MemoryStream(command[2..]);
                using var br = new BinaryReader(ms);
                var frequency1 = br.ReadInt64();
                var frequency2 = br.ReadInt64();
                var divider = br.ReadInt16();
                var step = br.ReadInt32();
                logger.Info($"DDS sweep command channel={command[1]} frequency1={frequency1} frequency2={frequency2} divider={divider} step={step}");
                return [(byte)'k'];
            }
            default:
                logger.Error("Unknown DDS emulator command");
                return [(byte)'e'];
        }
    }

    public byte[] TimeEvent(int id)
    {
        return [];
    }

    public short GetId() => 1;
}