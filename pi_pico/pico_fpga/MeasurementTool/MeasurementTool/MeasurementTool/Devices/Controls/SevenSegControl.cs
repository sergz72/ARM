using System.Globalization;
using Avalonia;
using Avalonia.Controls;
using Avalonia.Media;
using Avalonia.Media.TextFormatting;
using Avalonia.Threading;

namespace MeasurementTool.Devices.Controls;

public sealed class SevenSegControl: Control
{
    public const int Space = 10;
    public const int Minus = -1;
    
    private int _value;
    public int Value
    {
        set
        {
            _value = value;
            Dispatcher.UIThread.InvokeAsync(InvalidateVisual);
        }
    }

    public string PointSymbol {get; set;}
    public Color PenColor {get; set;}
    
    private readonly Typeface _typeface;
    
    private double _margin;
    private IPen _pen;
    private FormattedText _pointText;
    private Size _pointSize;
    private double _segWidth;
    
    public SevenSegControl()
    {
        PointSymbol = " ";
        PenColor = Colors.Red;
        _value = 0;
        _typeface = new Typeface("Arial");
        _pointSize = CalculateTextSize(PointSymbol, _typeface, 10);
        _pointText = new FormattedText(PointSymbol, CultureInfo.CurrentCulture, FlowDirection.LeftToRight,
                                        _typeface, 10, Brushes.Black);
        _margin = 2.5;
        _pen = new Pen(Brushes.Red, _margin);
    }

    public static Size CalculateTextSize(string text, Typeface typeface, double fontSize)
    {
        var ts = TextShaper.Current;
        var shaped = ts.ShapeText(text, new TextShaperOptions(typeface.GlyphTypeface, fontSize));
        var run = new ShapedTextRun(shaped, new GenericTextRunProperties(typeface, fontSize));
        return run.Size;
    }
    
    protected override Size MeasureOverride(Size availableSize)
    {
        var fontSize = availableSize.Height / 2;
        _pointText = new FormattedText(PointSymbol, CultureInfo.CurrentCulture, FlowDirection.LeftToRight,
            _typeface, fontSize, Brushes.Black);
        _pointSize = CalculateTextSize(PointSymbol, _typeface, fontSize);
        _margin = availableSize.Height / 16;
        _pen = new Pen(new SolidColorBrush(PenColor), _margin);
        _segWidth = availableSize.Height * 5 / 7;
        Width = _segWidth + _pointSize.Width + 1;
        Height = availableSize.Height;
        return new Size(Width, availableSize.Height);
    }

    public override void Render(DrawingContext context)
    {
        DrawPointSymbol(context);
        switch (_value)
        {
            case 0:
                DrawA(context);
                DrawB(context);
                DrawC(context);
                DrawD(context);
                DrawE(context);
                DrawF(context);
                break;
            case 1:
                DrawB(context);
                DrawC(context);
                break;
            case 2:
                DrawA(context);
                DrawB(context);
                DrawD(context);
                DrawE(context);
                DrawG(context);
                break;
            case 3:
                DrawA(context);
                DrawB(context);
                DrawC(context);
                DrawD(context);
                DrawG(context);
                break;
            case 4:
                DrawB(context);
                DrawC(context);
                DrawF(context);
                DrawG(context);
                break;
            case 5:
                DrawA(context);
                DrawC(context);
                DrawD(context);
                DrawF(context);
                DrawG(context);
                break;
            case 6:
                DrawA(context);
                DrawC(context);
                DrawD(context);
                DrawE(context);
                DrawF(context);
                DrawG(context);
                break;
            case 7:
                DrawA(context);
                DrawB(context);
                DrawC(context);
                break;
            case 8:
                DrawA(context);
                DrawB(context);
                DrawC(context);
                DrawD(context);
                DrawE(context);
                DrawF(context);
                DrawG(context);
                break;
            case 9:
                DrawA(context);
                DrawB(context);
                DrawC(context);
                DrawD(context);
                DrawF(context);
                DrawG(context);
                break;
            case Minus: // minus
                DrawG(context);
                break;
        }
    }

    private void DrawA(DrawingContext context)
    {
        context.DrawLine(_pen, new Point(_margin, _margin / 2), new Point(_segWidth - _margin, _margin / 2));
    }

    private void DrawB(DrawingContext context)
    {
        context.DrawLine(_pen, new Point(_segWidth - _margin / 2, _margin), new Point(_segWidth - _margin / 2, Height / 2 - _margin));
    }

    private void DrawC(DrawingContext context)
    {
        context.DrawLine(_pen, new Point(_segWidth - _margin / 2, Height / 2 + _margin), new Point(_segWidth - _margin / 2, Height - _margin));
    }
    
    private void DrawD(DrawingContext context)
    {
        context.DrawLine(_pen, new Point(_margin, Height - _margin / 2), new Point(_segWidth - _margin, Height - _margin / 2));
    }

    private void DrawE(DrawingContext context)
    {
        context.DrawLine(_pen, new Point(_margin / 2, Height - _margin), new Point(_margin / 2, Height / 2 + _margin));
    }

    private void DrawF(DrawingContext context)
    {
        context.DrawLine(_pen, new Point(_margin / 2, _margin), new Point(_margin / 2, Height / 2 - _margin));
    }
    
    private void DrawG(DrawingContext context)
    {
        context.DrawLine(_pen, new Point(_margin, Height / 2), new Point(_segWidth - _margin, Height / 2));
    }
    
    private void DrawPointSymbol(DrawingContext context)
    {
        context.DrawText(_pointText, new Point(_segWidth + 1, Height - _pointSize.Height));
    }
}