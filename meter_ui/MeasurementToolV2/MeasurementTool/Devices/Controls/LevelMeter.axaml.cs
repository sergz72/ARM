using System;
using System.Collections.Generic;
using Avalonia.Controls;

namespace MeasurementTool.Devices.Controls;

public partial class LevelMeter : UserControl
{
    private record MeterInfo(string Name, int MinDb, int MaxDb, Func<short, int> MvToDb);

    private static readonly Dictionary<MeterType, MeterInfo> MeterInfos = new()
    {
        {MeterType.Ad8307, new MeterInfo("AD8307", -80, 20, mv => -80 + mv / 25)},
        {MeterType.Ad8318, new MeterInfo("AD8318", -65, 10, mv => -65 + (2200 - mv) / 23)}
    };

    private readonly MeterInfo _meterInfo;
    
    public short[] Data { set => ShowData(value); }
    
    public LevelMeter()
    {
        InitializeComponent();
    }
    
    public LevelMeter(MeterType meterType)
    {
        InitializeComponent();
        _meterInfo = MeterInfos[meterType];
    }

    private void ShowData(short[] value)
    {
        throw new NotImplementedException();
    }
    
    public void SetF(long f1, long f2)
    {
        throw new NotImplementedException();
    }
    
    private double CalculateDataY(int pointNo)
    {
        /*var db = _meterInfo.MvToDb(_points[pointNo]);
        var startY = Bounds.Height - AxisOffsetY;
        var width = startY - AxisOffsetY;
        var y = startY - (db - _meterInfo.MinDb) * width / (_meterInfo.MaxDb - _meterInfo.MinDb);
        return y;*/
        throw new NotImplementedException();
    }
}