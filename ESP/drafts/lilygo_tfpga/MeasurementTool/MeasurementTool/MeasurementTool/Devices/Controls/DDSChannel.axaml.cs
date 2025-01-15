using Avalonia.Controls;

namespace MeasurementTool.Devices.Controls;

public partial class DDSChannel : UserControl
{
    private readonly NumberUpDownManager _frequencyManager, _amplitudeManager;
    public DDSChannel(Dds dds, int channelNumber)
    {
        InitializeComponent();
        ChannelNumber.Content = $"CH{channelNumber+1}";
        _frequencyManager = new NumberUpDownManager(dds.MinFrequency * 10, dds.MaxFrequency * 10, dds.MinFrequency * 10,
                                                    Freqp1, Freq1, Freq10, Freq100, Freq1K, Freq10K,
                                                    Freq100K, Freq1M, Freq10M, Freq100M);
        _amplitudeManager = new NumberUpDownManager(dds.MinAmplitude, dds.MaxAmplitude, dds.MaxAmplitude,
                                                    Amp1, Amp10);
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
}