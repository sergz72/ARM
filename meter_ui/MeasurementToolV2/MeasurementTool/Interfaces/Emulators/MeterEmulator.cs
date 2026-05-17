using MeasurementTool.Devices;

namespace MeasurementTool.Interfaces.Emulators;

internal sealed class MeterEmulator(ILogger logger) : IDeviceEmulator
{
    private static readonly byte[] Configuration = BuildMeterConfiguration();
    
    private const int Channels = 4;

    private static byte[] BuildMeterConfiguration()
    {
        var result = new byte[10];
        result[0] = (byte)Channels;
        result[1] = (byte)MeterMeasureType.Voltage;
        result[2] = (byte)MeterReturnValueType.Millis;
        result[3] = 2; // numbersBeforePoint
        result[4] = 0; // measurementPeriodMs(lo)
        result[5] = 0; // measurementPeriodMs(hi)
        result[6] = (byte)'T';
        result[7] = (byte)'e';
        result[8] = (byte)'s';
        result[9] = (byte)'t';
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
        if (id == EmulatorInterface.MaxEventId)
            return [0, 4, 0, 5, 0, 6, 0, 7];
        return [];
    }

    public byte GetId() => (byte)DeviceType.Meter16;
}
