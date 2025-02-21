using System;
using System.IO;
using System.Runtime.InteropServices;
using System.Threading;

namespace MeasurementTool.Interfaces;

public sealed class SerialInterface: IDeviceInterface, IDisposable
{
    [DllImport("libserial.so", EntryPoint = "serial_open", CharSet = CharSet.Ansi)]
    private static extern int SerialOpen(string fileName, int baud);
    
    [DllImport("libserial.so", EntryPoint = "serial_close", SetLastError = true)]
    private static extern void SerialClose(int handle);
    
    [DllImport("libserial.so", EntryPoint = "serial_read", SetLastError = true)]
    private static extern int SerialRead(int handle, byte[] data, int length);    

    [DllImport("libserial.so", EntryPoint = "serial_write", SetLastError = true)]
    private static extern int SerialWrite(int handle, byte[] data, int length);    
    
    private readonly int _port;
    
    public SerialInterface(string serialPort, int baud)
    {
        _port = SerialOpen(serialPort, baud);
        if (_port <= 0)
            throw new IOException("Cannot open serial port.");
    }
    
    public byte[] SendCommand(byte[] command)
    {
        var l = SerialWrite(_port, command, command.Length);
        if (l < 0)
            throw new IOException("Serial port write error.");
        Thread.Sleep(100);
        var buffer = new byte[1024 * 100];
        l = SerialRead(_port, buffer, buffer.Length);
        if (l < 0)
            throw new IOException("Serial port read error.");
        return buffer[..l];
    }

    public void Dispose()
    {
        SerialClose(_port);
    }
}