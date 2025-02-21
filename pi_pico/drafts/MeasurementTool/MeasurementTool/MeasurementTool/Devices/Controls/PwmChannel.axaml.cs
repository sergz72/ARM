using System;
using System.Globalization;
using Avalonia.Controls;
using Avalonia.Interactivity;

namespace MeasurementTool.Devices.Controls;

public partial class PwmChannel : UserControl
{
    private readonly NumberUpDownManager _frequencyManager, _dutyManager;
    private readonly int _channel;
    private readonly Pwm _pwm;
    private FactPwmValues _facts;

    public PwmChannel()
    {
        InitializeComponent();
    }

    public PwmChannel(Pwm pwm, int channelNumber)
    {
        InitializeComponent();
        _pwm = pwm;
        _channel = channelNumber;
        _frequencyManager = new NumberUpDownManager(1, pwm.MClk / 2, 1, null,
            Freq1, Freq10, Freq100, Freq1K, Freq10K, Freq100K, Freq1M, Freq10M);
        _frequencyManager.ValueChanged += FrequencyManagerOnValueChanged;
        _dutyManager = new NumberUpDownManager(1, 999, 500, null, Dutyp1, Duty1, Duty10);
        _dutyManager.ValueChanged += DutyManagerOnValueChanged;
        ChannelNumber.Content = $"CH{channelNumber+1}";
        UpdateFactFrequencyAndDuty();
    }
    
    private void DutyManagerOnValueChanged(object? sender, RoutedEventArgs e)
    {
        UpdateFactFrequencyAndDuty();
        if (OutputEnable.IsChecked == true && SetOnChange.IsChecked == true)
            _pwm.SetFrequencyAndDuty(_channel, _facts);
    }

    private void FrequencyManagerOnValueChanged(object? sender, RoutedEventArgs e)
    {
        UpdateFactFrequencyAndDuty();
        if (OutputEnable.IsChecked == true && SetOnChange.IsChecked == true)
            _pwm.SetFrequencyAndDuty(_channel, _facts);
    }
    
    private void UpdateFactFrequencyAndDuty()
    {
        _facts = _pwm.CalculateFactValues(_frequencyManager.Value, (double)_dutyManager.Value / 10);
        Clock.Content = _facts.Clock.ToString();
        FactFrequency.Content = _facts.Frequency.ToString();
        FactDuty.Content = _facts.Duty.ToString(CultureInfo.InvariantCulture);
        FrequencyCode.Content = _facts.FrequencyCode.ToString();
        DutyCode.Content = _facts.DutyCode.ToString();
    }
    
    private void Apply_OnClick(object? sender, RoutedEventArgs e)
    {
        if (OutputEnable.IsChecked != true)
            _pwm.OutputEnable(_channel, false);
        else
        {
            try
            {
                _pwm.SetFrequencyAndDuty(_channel, _facts);
                _pwm.OutputEnable(_channel, true);
            }
            catch (Exception ex)
            {
                MessageBox.Show("Error", ex.Message);
            }
        }
    }
    
    private void OutputEnable_OnClick(object? sender, RoutedEventArgs e)
    {
        if (SetOnChange.IsChecked == true)
            Apply_OnClick(null, new RoutedEventArgs());
    }
}