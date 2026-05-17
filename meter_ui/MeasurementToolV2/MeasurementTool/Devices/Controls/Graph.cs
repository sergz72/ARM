using System;
using System.Collections.Generic;
using System.Globalization;
using System.Numerics;
using Avalonia;
using Avalonia.Controls;
using Avalonia.Input;
using Avalonia.Media;
using Avalonia.Threading;

namespace MeasurementTool.Devices.Controls;

public class Graph<T> : Control where T: INumber<T>
{
    private record TextPoint(int Value, FormattedText Text);
    
    private const int AxisOffsetY = 20;
    private const int AxisOffsetX = 20;
    private const int FontSize = 12;

    private readonly Typeface _typeface;
    private readonly Pen _axisPen, _gridPen, _dataPen, _selectionPen;
    private readonly TextPoint[] _yLabels, _xLabels;
    private readonly Func<int, string> _xLabelsFormatter;
    private readonly Func<T, string> _yLabelsFormatter;
    
    private List<T> _values;
    private List<int> _selectionPoints;
    private int _windowStart;
    private T _minValue, _maxValue;

    public Graph(Func<int, string> xLabelsFormatter, Func<T, string> yLabelsFormatter)
    {
        _xLabelsFormatter = xLabelsFormatter;
        _yLabelsFormatter = yLabelsFormatter;
        _windowStart = 0;
        _values = [];
        _axisPen = new Pen(Brushes.Black, 1);
        _gridPen = new Pen(Brushes.LightGray, 1);
        _dataPen = new Pen(Brushes.Green, 1);
        _selectionPen = new Pen(Brushes.Yellow, 1);
        _typeface = new Typeface("Arial");
        _yLabels = [];
        _xLabels = [];
        _selectionPoints = [];
    }

    public void SetWindowStart(int windowStart)
    {
        if (windowStart != _windowStart && windowStart >= 0)
        {
            _windowStart = windowStart;
            BuildYLabels();
            BuildXLabels();
            Dispatcher.UIThread.Invoke(InvalidateVisual);
        }
    }

    public void AddValues(T[] values)
    {
        var render = _values.Count < _windowStart + values.Length && _values.Count + values.Length >= _windowStart;
        _values.AddRange(values);
        if (render)
        {
            BuildYLabels();
            Dispatcher.UIThread.Invoke(InvalidateVisual);
        }
    }

    private void BuildYLabels()
    {
        throw new NotImplementedException();
    }

    private void BuildXLabels()
    {
        throw new NotImplementedException();
    }
    
    public override void Render(DrawingContext context)
    {
        context.FillRectangle(Brushes.White, new Rect(Bounds.Size));
        RenderAxis(context);
        RenderYLabels(context);
        RenderXGrid(context);
        RenderSelections(context);
        if (_values.Count < 2) return;
        RenderData(context);
    }

    private void RenderSelections(DrawingContext context)
    {
        if (_selectionPoint >= 0)
        {
            context.DrawLine(_selectionPen, new Point(_selectionPoint, AxisOffsetY), new Point(_selectionPoint, Bounds.Height - AxisOffsetY));
            context.DrawText(_selectionText, new Point(Bounds.Width - _selectionText.Width, 0));
        }
    }
    
    private int CalculateDataX(int startX, int width, int pointNo) => startX + pointNo * width / (_points.Length - 1);
    

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
}