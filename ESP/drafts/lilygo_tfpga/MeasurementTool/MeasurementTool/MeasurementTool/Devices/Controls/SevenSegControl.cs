using System.Globalization;
using Avalonia;
using Avalonia.Controls;
using Avalonia.Media;

namespace MeasurementTool.Devices.Controls;

public sealed class SevenSegControl: Control
{
    private int _value;
    public int Value
    {
        set
        {
            _value = value;
            InvalidateVisual();
        }
    }

    public string PointSymbol {get; set;}
    public Color PenColor {get; set;}
    
    private double _margin;
    private IPen _pen;
    private FormattedText _pointText;
    
    public SevenSegControl()
    {
        PointSymbol = " ";
        _value = 0;
        _pointText = new FormattedText(PointSymbol, CultureInfo.CurrentCulture, FlowDirection.LeftToRight,
                                        new Typeface("Arial"), 10, null);
        _margin = 2.5;
        _pen = new Pen(Brushes.Red, _margin);
    }

    protected override Size MeasureOverride(Size availableSize)
    {
        _pointText = new FormattedText(PointSymbol, CultureInfo.CurrentCulture, FlowDirection.LeftToRight,
            new Typeface("Arial"), availableSize.Height / 4, null);
        _margin = availableSize.Height / 16;
        _pen = new Pen(new SolidColorBrush(PenColor), _margin);
        Width = availableSize.Height * 5 / 7;
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
            default: // minus
                DrawG(context);
                break;
        }
    }

    private void DrawA(DrawingContext context)
    {
        context.DrawLine(_pen, new Point(_margin, _margin / 2), new Point(Width - _margin, _margin / 2));
    }

    private void DrawB(DrawingContext context)
    {
        context.DrawLine(_pen, new Point(Width - _margin / 2, _margin), new Point(Width - _margin / 2, Height / 2 - _margin));
    }

    private void DrawC(DrawingContext context)
    {
        context.DrawLine(_pen, new Point(Width - _margin / 2, Height / 2 + _margin), new Point(Width - _margin / 2, Height - _margin));
    }
    
    private void DrawD(DrawingContext context)
    {
        context.DrawLine(_pen, new Point(_margin, Height - _margin / 2), new Point(Width - _margin, Height - _margin / 2));
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
        context.DrawLine(_pen, new Point(_margin, Height / 2), new Point(Width - _margin, Height / 2));
    }
    
    private void DrawPointSymbol(DrawingContext context)
    {
        context.DrawText(_pointText, new Point(_pointText.Height * 4, _pointText.Height * 6));
    }
}