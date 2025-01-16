using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading;
using Avalonia.Controls;
using Avalonia.Layout;
using MeasurementTool.Devices.Controls;

namespace MeasurementTool.Devices;

public class Voltmeter: GenericDevice
{
    private int _channels, _resolution, _numbersBeforePoint;
    private VoltmeterReturnValueType _valueType;
    private List<VoltmeterChannel> _channelsUi;
    private readonly string _measureName;
    private string _name;
    private bool _getCapabilitiesFailed;

    public Voltmeter(DeviceManager dm, byte channel, string measureName = "V") : base(dm, channel)
    {
        _channels = _resolution = _numbersBeforePoint = 0;
        _valueType = VoltmeterReturnValueType.Millis;
        _channelsUi = [];
        _measureName = measureName;
        _name = "";
        _getCapabilitiesFailed = false;
    }

    internal override void Init()
    {
        Dm.QueueCommand(Channel, [(byte)VoltmeterCommands.GetCapabilities], ProcessGetCapabilitiesResponse); // get capabilities
    }
    
    internal override Control? CreateUi()
    {
        while (_name == "")
        {
            if (_getCapabilitiesFailed)
                return null;
            Thread.Sleep(100);
        }
        var panel = new StackPanel
        {
            Orientation = Orientation.Vertical,
        };
        _channelsUi = CreateChannelsUi();
        panel.Children.AddRange(_channelsUi);
        return panel;
    }

    private List<VoltmeterChannel> CreateChannelsUi()
    {
        return Enumerable.Range(0, _channels).Select(n => new VoltmeterChannel()).ToList();
    }
    
    internal override string GetName()
    {
        return _name;
    }

    private void ProcessGetCapabilitiesResponse(byte[]? response)
    {
        if (response == null || response.Length < 4)
        {
            _getCapabilitiesFailed = true;
            return;
        }
        _channels = response[0];
        _resolution = response[1];
        _valueType = (VoltmeterReturnValueType)response[2];
        _numbersBeforePoint = response[3];
        _name = Encoding.UTF8.GetString(response[4..]);
    }

    internal override void ParseResponse(byte[] data)
    {
        if (data.Length != _channels * _resolution || _channelsUi.Count != _channels)
        {
            Dm.LogError(Channel, "Invalid response length");
            return;
        }

        for (var i = 0; i < _channels; i++)
        {
            //todo
        }
    }
}

internal enum VoltmeterCommands
{
    GetCapabilities = 'c'
}

internal enum VoltmeterReturnValueType
{
    Ones,
    Millis,
    Micros
}