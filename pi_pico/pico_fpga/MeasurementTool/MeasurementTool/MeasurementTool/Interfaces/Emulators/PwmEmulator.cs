using System.IO;
using MeasurementTool.Devices;

namespace MeasurementTool.Interfaces.Emulators;

public sealed class PwmEmulator(ILogger logger) : IDeviceEmulator
{
    private static readonly byte[] Configuration = BuildPwmConfiguration();
    
    private static byte[] BuildPwmConfiguration()
    {
        using var ms = new MemoryStream();
        using var bw = new BinaryWriter(ms);
        bw.Write(50000000); //mClk
        bw.Write((byte)2); // channels
        bw.Write((byte)0); // DDS clock
        bw.Write((byte)32); // bits
        bw.Write((byte)'P');
        bw.Write((byte)'W');
        bw.Write((byte)'M');
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
            case (byte)PwmCommands.SetFrequency:
            {
                using var ms = new MemoryStream(command[2..]);
                using var br = new BinaryReader(ms);
                var clock = br.ReadInt32();
                var frequency = br.ReadInt32();
                var duty = br.ReadInt32();
                logger.Info(
                    $"PWM set frequency and duty command channel={command[1]} clock={clock} frequency={frequency} duty={duty}");
                break;
            }
            case (byte)PwmCommands.EnableOutput:
                logger.Info($"PWM enable output command channel={command[1]} enable={command[2]}");
                break;
            default:
                logger.Error("Unknown PWM emulator command");
                return [(byte)'e'];
        }
        return [(byte)'k'];
    }

    public byte[] TimeEvent(int id)
    {
        return [];
    }

    public byte GetId() => (byte)DeviceType.Pwm;
}