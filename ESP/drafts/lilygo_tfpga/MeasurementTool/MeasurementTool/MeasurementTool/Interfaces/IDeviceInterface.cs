namespace MeasurementTool.Interfaces;

public interface IDeviceInterface
{
    byte[] SendCommand(byte[] command);
}