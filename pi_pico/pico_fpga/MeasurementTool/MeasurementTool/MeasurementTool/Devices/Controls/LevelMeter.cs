using System.Globalization;
using Avalonia;
using Avalonia.Controls;
using Avalonia.Markup.Xaml;
using Avalonia.Media;

namespace MeasurementTool.Devices.Controls;

public class LevelMeter : Control
{
    private const int AxisOffset = 10;
    private const int FontSize = 12;
    
    private long _f1;
    public long F1 { get => _f1; set { _f1=value; UpdateLabels(); } }
    private long _f2;
    public long F2 { get => _f2; set { _f2=value; UpdateLabels(); } }
    private int[] _points;
    public int[] Data { set => ShowData(value); }
    private Pen _axisPen;
    private FormattedText _zero;
    private Typeface _typeface;
    
    public LevelMeter()
    {
        _points = [];
        _axisPen = new Pen(Brushes.Black, 1);
        _typeface = new Typeface("Arial");
        _zero = new FormattedText("0", CultureInfo.InvariantCulture, FlowDirection.LeftToRight, _typeface, FontSize, null);
    }
    
    private void UpdateLabels()
    {
        InvalidateVisual();
    }

    private void ShowData(int[] value)
    {
        InvalidateVisual();
    }

    public override void Render(DrawingContext context)
    {
        RenderAxis(context);
        var df = (_f2 - _f1) / _points.Length;
        if (_points.Length < 2 || df <= 0) return;
        RenderXLabels(df, context);
        RenderYLabels(context);
        RenderData(context);
    }

    private void RenderData(DrawingContext context)
    {
        //todo
    }

    private void RenderXLabels(long df, DrawingContext context)
    {
        //todo
    }

    private void RenderYLabels(DrawingContext context)
    {
        //todo
    }
    
    private void RenderAxis(DrawingContext context)
    {
        context.DrawLine(_axisPen, new Point(AxisOffset, AxisOffset), new Point(AxisOffset, Width - AxisOffset));
        context.DrawLine(_axisPen, new Point(AxisOffset, AxisOffset), new Point(Height - AxisOffset, AxisOffset));
        context.DrawText(_zero, new Point(0, 0));
    }
}