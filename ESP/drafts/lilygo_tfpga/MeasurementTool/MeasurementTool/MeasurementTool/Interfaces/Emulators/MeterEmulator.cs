using MeasurementTool.Devices;

namespace MeasurementTool.Interfaces.Emulators;

internal sealed class MeterEmulator(ILogger logger) : IDeviceEmulator
{
    private static readonly byte[] Configuration = BuildMeterConfiguration();
    
    private const int Channels = 4;

    private static byte[] BuildMeterConfiguration()
    {
        var result = new byte[9];
        result[0] = (byte)Channels;
        result[1] = 2; // 16 bit
        result[2] = (byte)MeterMeasureType.Voltage;
        result[3] = (byte)MeterReturnValueType.Millis;
        result[4] = 2; // numbersBeforePoint
        result[5] = (byte)'T';
        result[6] = (byte)'e';
        result[7] = (byte)'s';
        result[8] = (byte)'t';
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
        {
            
        }
        return [];
    }

    public byte GetId() => (byte)DeviceType.Meter;
}
