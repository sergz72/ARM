using System;
using Avalonia.Controls;
using Avalonia.Interactivity;

namespace MeasurementTool.Devices.Controls;

public partial class DDSChannel : UserControl
{
    private readonly NumberUpDownManager _frequencyManager, _amplitudeManager;
    private readonly Dds _dds;
    private readonly int _channel;

    public DDSChannel()
    {
        InitializeComponent();
    }

    public DDSChannel(Dds dds, int channelNumber)
    {
        InitializeComponent();
        _dds = dds;
        _channel = channelNumber;
        ChannelNumber.Content = $"CH{channelNumber+1}";
        var startf = dds.MinFrequency <= 1000000 ? 10000000 : dds.MinFrequency * 10;
        _frequencyManager = new NumberUpDownManager(dds.MinFrequency * 10, dds.MaxFrequency * 10, startf, null,
                                                    Freqp1, Freq1, Freq10, Freq100, Freq1K, Freq10K,
                                                    Freq100K, Freq1M, Freq10M, Freq100M, Freq1G, Freq10G);
        _frequencyManager.ValueChanged += FrequencyManagerOnValueChanged;
        _amplitudeManager = new NumberUpDownManager(dds.MinAmplitude, dds.MaxAmplitude, dds.MaxAmplitude, AmpSign,
                                                    Amp1, Amp10);
        _amplitudeManager.ValueChanged += AmplitudeManagerOnValueChanged;
        if ((dds.SupportedNodes & DdsModes.Sine) != 0)
            Modes.Items.Add(DdsModes.Sine);
        if ((dds.SupportedNodes & DdsModes.Triangle) != 0)
            Modes.Items.Add(DdsModes.Triangle);
        if ((dds.SupportedNodes & DdsModes.Square) != 0)
            Modes.Items.Add(DdsModes.Square);
        Modes.SelectedIndex = 0;
        Divider.ItemsSource = dds.Dividers;
        Divider.SelectedIndex = 0;
    }

    private void AmplitudeManagerOnValueChanged(object? sender, RoutedEventArgs e)
    {
    }

    private void FrequencyManagerOnValueChanged(object? sender, RoutedEventArgs e)
    {
    }

    private void Apply_OnClick(object? sender, RoutedEventArgs e)
    {
        if (OutputEnable.IsChecked != true)
            _dds.OutputEnable(_channel, false);
        else
        {
            try
            {
                if (!SetSweep())
                    _dds.SetFrequency(_channel, _frequencyManager.Value, (int)(Divider.SelectedItem ?? 1));
                _dds.SetMode(_channel, (DdsModes)(Modes.SelectedItem ?? DdsModes.Sine));
                _dds.SetAmplitude(_channel, (int)_amplitudeManager.Value);
                _dds.OutputEnable(_channel, true);
            }
            catch (Exception ex)
            {
               MessageBox.Show("Error", ex.Message);
            }
        }
    }

    private bool SetSweep()
    {
        var (width, step) = ParseSweepString();
        if (width == 0 || step == 0)
            return false;
        var freq = _frequencyManager.Value;
        var f1 = freq - width;
        var f2 = freq + width;
        if (f1 < _dds.MinFrequency)
            f1 = _dds.MinFrequency;
        if (f2 > _dds.MaxFrequency)
            f2 = _dds.MaxFrequency;
        if (f2 <= f1)
            throw new Exception("Invalid frequency/width");
        width = (f2 - f1) / 2;
        if (width == 0 || step > width / 50)
            throw new Exception("Invalid width");
        var steps = width / step;
        if (steps > 200)
            throw new Exception("Invalid step");
        _dds.SetSweep(_channel, f1, (int)(Divider.SelectedItem ?? 1), (int)step, (int)steps);
        return true;
    }

    private (long width, long step) ParseSweepString()
    {
        var parts = Sweep.Text?.Split(',') ?? [];
        if (parts.Length != 2 || !ParseFrequencyValue(parts[0], out var width) ||
            !ParseFrequencyValue(parts[1], out var step) || width < 0 || step < 0)
            throw new Exception("Invalid sweep string");
        return (width, step);
    }

    private bool ParseFrequencyValue(string s, out long value)
    {
        long multiplier = 1;
        s = s.ToLower();
        if (s.EndsWith('k'))
        {
            s = s[..^1];
            multiplier = 1000;
        }
        else if (s.EndsWith('m'))
        {
            s = s[..^1];
            multiplier = 1000000;
        }

        if (!long.TryParse(s, out var v))
        {
            value = 0;
            return false;
        }
        
        value = multiplier * v;
        return true;
    }

    private void OutputEnable_OnClick(object? sender, RoutedEventArgs e)
    {
        if (SetOnChange.IsChecked == true)
            Apply_OnClick(null, new RoutedEventArgs());
    }

    private void Modes_OnSelectionChanged(object? sender, SelectionChangedEventArgs e)
    {
        if (SetOnChange.IsChecked == true)
            _dds.SetMode(_channel, (DdsModes)(Modes.SelectedItem ?? DdsModes.Sine));
    }

    private void Divider_OnSelectionChanged(object? sender, SelectionChangedEventArgs e)
    {
        if (SetOnChange.IsChecked == true)
            _dds.SetFrequency(_channel, _frequencyManager.Value, (int)(Divider.SelectedItem ?? 1));
    }
}