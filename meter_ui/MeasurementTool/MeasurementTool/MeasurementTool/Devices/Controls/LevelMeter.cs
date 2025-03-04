using System;
using System.Collections.Generic;
using System.Globalization;
using System.Linq;
using Avalonia;
using Avalonia.Controls;
using Avalonia.Input;
using Avalonia.Media;
using Avalonia.Threading;

namespace MeasurementTool.Devices.Controls;

public class LevelMeter : Control
{
    private record MeterInfo(string Name, int MinDb, int MaxDb, Func<short, int> MvToDb);
    
    private static readonly Dictionary<MeterType, MeterInfo> MeterInfos = new()
    {
        {MeterType.Ad8307, new MeterInfo("AD8307", -80, 20, mv => -80 + mv / 25)},
        {MeterType.Ad8318, new MeterInfo("AD8318", -65, 10, mv => -65 + (2200 - mv) / 23)}
    };
    
    private const int AxisOffsetY = 20;
    private const int AxisOffsetX = 20;
    private const int FontSize = 12;

    private readonly int _minDb, _maxDb;
    private readonly MeterInfo _meterInfo;
    private readonly Typeface _typeface;
    private readonly Pen _axisPen, _gridPen, _dataPen, _selectionPen;
    private readonly FormattedText[] _yLabels;
    
    private long _f1;
    private long _f2;
    private short[] _points;
    private double _selectionPoint;
    public short[] Data { set => ShowData(value); }
    private FormattedText _title, _selectionText;

    public LevelMeter(): this(MeterType.Ad8307)
    {
    }

    public LevelMeter(MeterType meterType)
    {
        _meterInfo = MeterInfos[meterType];
        _points = [];
        _axisPen = new Pen(Brushes.Black, 1);
        _gridPen = new Pen(Brushes.LightGray, 1);
        _dataPen = new Pen(Brushes.Green, 1);
        _selectionPen = new Pen(Brushes.Yellow, 1);
        _typeface = new Typeface("Arial");
        _yLabels = BuildYLabels(_meterInfo);
        _selectionPoint = -1;
        BuildSelectionText();
        UpdateTitle();
    }

    public void SetF(long f1, long f2)
    {
        _f1 = f1;
        _f2 = f2;
        UpdateTitle();
    }
    
    private FormattedText[] BuildYLabels(MeterInfo meterInfo)
    {
        var min = meterInfo.MinDb / 10;
        return Enumerable.Range(min, meterInfo.MaxDb / 10 - min + 1)
            .Select(l => new FormattedText((l * 10).ToString(), CultureInfo.InvariantCulture,
                                                FlowDirection.LeftToRight, _typeface, FontSize, Brushes.Black))
            .ToArray();
    }

    private void UpdateTitle()
    {
        var step = (_f2 - _f1) / 10; 
        var title = $"Level meter {_meterInfo.Name} F1: {_f1}, F2: {_f2} Step: {step}";
        _title = new FormattedText(title, CultureInfo.InvariantCulture,
                                    FlowDirection.LeftToRight, _typeface, FontSize, Brushes.Black);
        InvalidateVisual();
    }

    private void ShowData(short[] value)
    {
        _points = value;
        Dispatcher.UIThread.Invoke(InvalidateVisual);
    }

    public override void Render(DrawingContext context)
    {
        context.FillRectangle(Brushes.White, new Rect(Bounds.Size));
        RenderTitle(context);
        RenderAxis(context);
        RenderYLabels(context);
        RenderXGrid(context);
        RenderSelection(context);
        if (_points.Length < 2) return;
        RenderData(context);
    }

    private void RenderSelection(DrawingContext context)
    {
        if (_selectionPoint >= 0)
        {
            context.DrawLine(_selectionPen, new Point(_selectionPoint, AxisOffsetY), new Point(_selectionPoint, Bounds.Height - AxisOffsetY));
            context.DrawText(_selectionText, new Point(Bounds.Width - _selectionText.Width, 0));
        }
    }

    private void RenderTitle(DrawingContext context)
    {
        context.DrawText(_title, new Point(0, 0));
    }

    private int CalculateDataX(int startX, int width, int pointNo) => startX + pointNo * width / (_points.Length - 1);
    
    private double CalculateDataY(int pointNo)
    {
        var db = _meterInfo.MvToDb(_points[pointNo]);
        var startY = Bounds.Height - AxisOffsetY;
        var width = startY - AxisOffsetY;
        var y = startY - (db - _meterInfo.MinDb) * width / (_meterInfo.MaxDb - _meterInfo.MinDb);
        return y;
    }

    private void RenderData(DrawingContext context)
    {
        var startX = AxisOffsetX;
        var width = (int)(Bounds.Width - AxisOffsetX - startX);
        for (var i = 1; i < _points.Length; i++)
        {
            var x1 = CalculateDataX(startX, width, i - 1);
            var y1 = CalculateDataY(i - 1);
            var x2 = CalculateDataX(startX, width, i);
            var y2 = CalculateDataY(i);
            context.DrawLine(_dataPen, new Point(x1, y1), new Point(x2, y2));
        }
    }
    
    private void RenderXGrid(DrawingContext context)
    {
        var startX = AxisOffsetX;
        var width = Bounds.Width - AxisOffsetX - startX;
        for (var i = 0; i < 10; i++)
        {
            var x = startX + width * i / 9;
            context.DrawLine(_gridPen, new Point(x, AxisOffsetY), new Point(x, Bounds.Height - AxisOffsetY));
        }
    }

    private void RenderYLabels(DrawingContext context)
    {
        var startY = Bounds.Height - AxisOffsetY;
        var height = startY - AxisOffsetY;
        for (var i = 0; i < _yLabels.Length; i++)
        {
            var y = startY - height * i / (_yLabels.Length - 1);
            context.DrawText(_yLabels[i], new Point(0, y - _yLabels[0].Height / 2));
            context.DrawLine(_gridPen, new Point(AxisOffsetX, y), new Point(Bounds.Width - AxisOffsetX, y));
        }
    }
    
    private void RenderAxis(DrawingContext context)
    {
        var start = new Point(AxisOffsetX, Bounds.Height - AxisOffsetY);
        // Y axis
        context.DrawLine(_axisPen, start, new Point(AxisOffsetX, AxisOffsetY));
        // X axis
        context.DrawLine(_axisPen, start, new Point(Bounds.Width - AxisOffsetX, Bounds.Height - AxisOffsetY));
    }

    protected override void OnPointerPressed(PointerPressedEventArgs e)
    {
        _selectionPoint = e.GetPosition(this).X;
        BuildSelectionText();
        InvalidateVisual();
    }

    private void BuildSelectionText()
    {
        var f = "";
        if (_selectionPoint >= 0)
        {
            var offset = _selectionPoint - AxisOffsetX;
            var freq = _f1 + (_f2 - _f1) * offset / (Bounds.Width - AxisOffsetX * 2);
            f = freq.ToString(CultureInfo.InvariantCulture);
        }

        _selectionText = new FormattedText($"F = {f}", CultureInfo.InvariantCulture,
                                            FlowDirection.LeftToRight, _typeface, FontSize, Brushes.Black);
    }
}