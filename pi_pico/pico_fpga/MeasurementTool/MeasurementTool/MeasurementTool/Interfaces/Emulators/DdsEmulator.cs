using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using MeasurementTool.Devices;

namespace MeasurementTool.Interfaces.Emulators;

internal sealed class DdsEmulator(ILogger logger) : IDeviceEmulator
{
    private static readonly byte[] Configuration = BuildSi5351Configuration();

    private int[] _sweepPoints = new int[10];
    private bool[] _channelEnabled = new bool[10];
    
    private static byte[] BuildSi5351Configuration()
    {
        using var ms = new MemoryStream();
        using var bw = new BinaryWriter(ms);
        bw.Write((short)DdsType.Si5351);
        bw.Write((short)1); //level meter type
        bw.Write(27000000L); //mClk
        bw.Write((short)3300); //maxMv
        bw.Write((byte)0); //maxAttenuator
        return ms.ToArray();
    }

    public byte[] BuildConfiguration()
    {
        return Configuration;
    }

    public byte[] Command(byte[] command)
    {
        switch (command[0])
        {
            case (byte)DdsCommands.EnableOutput:
                _channelEnabled[command[1]] = command[2] != 0;
                logger.Info($"DDS enable output command channel={command[1]} enable={command[2]}");
                return [(byte)'k'];
            case (byte)DdsCommands.SetFrequency:
            case (byte)DdsCommands.SetFrequencyCode:
            {
                _sweepPoints[command[1]] = 0; // sweep off
                using var ms = new MemoryStream(command[2..]);
                using var br = new BinaryReader(ms);
                var frequency = br.ReadInt64();
                var divider = br.ReadInt16();
                var t = command[0] == (byte)DdsCommands.SetFrequencyCode ? "code" : "";
                logger.Info($"DDS set frequency {t} command channel={command[1]} frequency{t}={frequency} divider={divider}");
                return [(byte)'k'];
            }
            case (byte)DdsCommands.SetAttenuator:
                logger.Info($"DDS set attenuator command channel={command[1]} value={command[2]}");
                return [(byte)'k'];
            case (byte)DdsCommands.SetMode:
                logger.Info($"DDS set mode command channel={command[1]} mode={command[2]}");
                return [(byte)'k'];
            case (byte)DdsCommands.Sweep:
            case (byte)DdsCommands.SweepCodes:
            {
                using var ms = new MemoryStream(command[2..]);
                using var br = new BinaryReader(ms);
                var frequency1 = br.ReadInt64();
                var divider = br.ReadInt16();
                var step = br.ReadInt32();
                var numPoints = br.ReadInt16();
                _sweepPoints[command[1]] = numPoints;
                var t = command[0] == (byte)DdsCommands.SweepCodes ? "code" : "";
                logger.Info($"DDS sweep {t} command channel={command[1]} frequency1{t}={frequency1} divider={divider} step={step} numPoints={numPoints}");
                return [(byte)'k'];
            }
            default:
                logger.Error("Unknown DDS emulator command");
                return [(byte)'e'];
        }
    }

    public byte[] TimeEvent(int id)
    {
        if (id == 9)
            return _sweepPoints.SelectMany(GeneratePoints).ToArray();
        return [];
    }

    private static IEnumerable<byte> GeneratePoints(int points)
    {
        var r = new Random();
        return Enumerable.Range(0, points)
            .Select(_ => (short)(r.Next() % 2500))
            .SelectMany(BitConverter.GetBytes);
    }

    public byte GetId() => (byte)DeviceType.Dds;
}
