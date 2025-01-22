using System;
using System.IO.Ports;

namespace MeasurementTool.Interfaces;

public class SerialInterface: IDeviceInterface, IDisposable
{
    private readonly SerialPort _port;
    
    public SerialInterface(string serialPort)
    {
        _port = new SerialPort(serialPort);
        _port.ReadTimeout = 100;
        _port.Open();
    }
    
    public byte[] SendCommand(byte[] command)
    {
        _port.Write(command, 0, command.Length);
        var buffer = new byte[1024 * 100];
        try
        {
            var l = _port.Read(buffer, 0, buffer.Length);
            return buffer[..l];
        }
        catch (TimeoutException)
        {
            return [];
        }
    }

    public void Dispose()
    {
        _port.Close();
        _port.Dispose();
    }
}