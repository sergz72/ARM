#include "hal.h"
#include <generic_dds.h>

#define DDS_COMMAND         3
#define GET_RESULTS_COMMAND 4

static unsigned short txbuf[MAX_TRANSFER_SIZE / 2];
static unsigned int sweep_points, sweep_current_point;
static unsigned long sweep_freq_code_start, sweep_freq_code;
static unsigned int sweep_step;
static unsigned short sweep_div;
static int sweep_channel;
static int last_command;
#ifdef LED_TIMER_ON
static int led_timer_counter, led_timer_state;
#endif
#ifdef LED_COMMAND_ON
static int led_command_state;
#endif

volatile unsigned int command_ready, timer_interrupt;
volatile dds_i2c_command commands[DDS_COMMAND_QUEUE_LENGTH];
unsigned char status;
int command_to_execute;

extern const unsigned char device_id;
extern const DdsConfig device_config;

const void *txbufs[] =
{
    &device_id,     // get device id command
    &device_config, // get device config command
    &status,        // get status command
    &status,        // dds command
    txbuf,          // get sweep results command
    &status,        // command 5
    &status,        // command 6
    &status,        // command 7
};

static void timer_event(void)
{
    if (sweep_points && !status)
    {
        if (sweep_current_point)
            txbuf[sweep_current_point - 1] = adc_get();
        if (sweep_current_point == sweep_points)
        {
            sweep_current_point = 0;
            sweep_freq_code = sweep_freq_code_start;
            status = 1;
            status_updated();
            return;
        }
        sweep_current_point++;
        set_frequency_code(sweep_channel, sweep_freq_code, sweep_div);
        sweep_freq_code += sweep_step;
    }
#ifdef LED_TIMER_ON
    led_timer_counter++;
    if (led_timer_counter == 200)
    {
        led_timer_state = !led_timer_state;
        if (led_timer_state)
            LED_TIMER_ON;
        else
            LED_TIMER_OFF;
        led_timer_counter = 0;
    }
#endif
}

static void exec_dds_command(const dds_i2c_command *cmd)
{
    switch (cmd->command)
    {
        case DDS_COMMAND_SET_FREQUENCY_CODE:
            sweep_points = 0;
            set_frequency_code(cmd->channel, cmd->c10.freq, cmd->c10.div);
            break;
        case DDS_COMMAND_SET_MODE:
            set_mode(cmd->channel, cmd->c1.parameter);
            break;
        case DDS_COMMAND_ENABLE_OUTPUT:
            enable_output(cmd->channel, cmd->c1.parameter);
            break;
        case DDS_COMMAND_SWEEP_CODES:
            sweep_current_point = 0;
            sweep_freq_code_start = sweep_freq_code = (unsigned long)cmd->c18.freq;
            sweep_div = cmd->c18.div;
            sweep_step = cmd->c18.step;
            sweep_channel = cmd->channel;
            sweep_points = cmd->c18.points;
            break;
        default:
            break;
    }
}

int main(void)
{
#ifdef LED_COMMAND_ON
    led_command_state = 0;
#endif
#ifdef LED_TIMER_ON
    led_timer_state = 0;
#endif
    command_ready = timer_interrupt = 0;
    status = 0;
    sweep_points = sweep_current_point = 0;
    last_command = 0;
    command_to_execute = 0;

    SysInit(&device_id);

    handler_init();

    timer_enable();

    while(1)
    {
        WAIT_FOR_INTERRUPT;
        if (timer_interrupt)
        {
            timer_event();
            timer_interrupt = 0;
        }
        if (command_ready)
        {
#ifdef LED_COMMAND_ON
            led_command_state = !led_command_state;
            if (led_command_state)
                LED_COMMAND_ON;
            else
                LED_COMMAND_OFF;
#endif
            command_ready = 0;
        }
        while (command_to_execute != command_to_process)
        {
            dds_i2c_command *cmd = &commands[command_to_execute];
            if (command_to_execute == DDS_COMMAND_QUEUE_LENGTH - 1)
                command_to_execute = 0;
            else
                command_to_execute++;
            if (last_command == GET_RESULTS_COMMAND)
            {
                status = 0;
                status_updated();
            }
            switch (cmd->device_command)
            {
                case DDS_COMMAND:
                    exec_dds_command(cmd);
                    break;
                default:
                    break;
            }
            last_command = cmd->device_command;
        }
    }
}
