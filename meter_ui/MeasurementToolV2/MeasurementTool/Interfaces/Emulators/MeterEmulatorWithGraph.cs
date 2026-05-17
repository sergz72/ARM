using System;
using System.Security.Cryptography;
using MeasurementTool.Devices;

namespace MeasurementTool.Interfaces.Emulators;

internal sealed class MeterEmulatorWithGraph(ILogger logger) : IDeviceEmulator
{
    private static readonly byte[] Configuration = BuildMeterConfiguration();
    
    private const int Channels = 1;

    private static byte[] BuildMeterConfiguration()
    {
        var result = new byte[11];
        result[0] = Channels;
        result[1] = 2; // 16 bit
        result[2] = (byte)MeterMeasureType.Voltage;
        result[3] = (byte)MeterReturnValueType.Millis;
        result[4] = 2; // numbersBeforePoint
        result[5] = 1; // measurementPeriodMs(lo)
        result[6] = 0; // measurementPeriodMs(hi)
        result[7] = (byte)'T';
        result[8] = (byte)'e';
        result[9] = (byte)'s';
        result[10] = (byte)'t';
        return result;
    }

    public byte[] BuildConfiguration()
    {
        return Configuration;
    }

    public byte[] Command(byte[] command)
    {
        logger.Error("Unexpected meter command");
        return [(byte)'e'];
    }

    public byte[] TimeEvent(int id)
    {
        var result = new byte[100 * 2];
        for (var i = 0; i < 100; i++)
            BitConverter.GetBytes((short)(0x400 + RandomNumberGenerator.GetInt32(-0x100, 0x100))).CopyTo(result, i * 2);
        return result;
    }

    public byte GetId() => (byte)DeviceType.Meter16;
}
