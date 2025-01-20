using System.IO;
using MeasurementTool.Devices;

namespace MeasurementTool.Interfaces.Emulators;

internal sealed class DdsEmulator(ILogger logger) : IDeviceEmulator
{
    private static readonly byte[] Configuration = BuildSi5351Configuration();
    
    private static byte[] BuildSi5351Configuration()
    {
        using var ms = new MemoryStream();
        using var bw = new BinaryWriter(ms);
        bw.Write((short)DdsType.Si5351);
        bw.Write((byte)0); //minDb
        bw.Write((byte)0); //maxDb
        bw.Write(27000000); //mClk
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

    public byte GetId() => (byte)DeviceType.Dds;
}
