#ifndef SI5351_H_
#define SI5351_H_

#define SI5351_DEVICE_DEFAULT_ADDRESS 0xC0

enum si5351_variant {SI5351_VARIANT_A,SI5351_VARIANT_B,SI5351_VARIANT_C};
enum si5351_crystal_load {SI5351_CRYSTAL_LOAD_6PF=0x40, SI5351_CRYSTAL_LOAD_8PF=0x80, SI5351_CRYSTAL_LOAD_10PF=0xC0};
enum si5351_drive {SI5351_DRIVE_2MA, SI5351_DRIVE_4MA, SI5351_DRIVE_6MA, SI5351_DRIVE_8MA};

typedef struct
{
    unsigned char device_address;
    int channel;
    unsigned int mClk;
    enum si5351_crystal_load load;
    enum si5351_variant variant;
    int output_count;
} si5351_conf;

typedef struct
{
    const si5351_conf conf;
    double fVCO[2];
    unsigned int max_frequency;
    unsigned int max_frequency_plla;
    unsigned int min_frequency;
    unsigned char clock_enable_flags;
    unsigned char clock_control[8];
} si5351_dev;

int si5351_write(unsigned char device_address, int channel, const unsigned char *data, unsigned int length); // should be defined in hal.c
int si5351_read(unsigned char device_address, int channel, unsigned char register_no, unsigned char *data, unsigned int length); // should be defined in hal.c

int si5351_init(const si5351_conf *conf, si5351_dev *device);
int si5351_enable_output(si5351_dev *device, int output_no, int enable);
int si5351_set_frequency(si5351_dev *device, int output_no, unsigned int frequency, unsigned int divider);
int si5351_set_drive_strength(si5351_dev *device, int output_no, enum si5351_drive drive);
int si5351_get_status(const si5351_dev *device, unsigned char *status);
int si5351_set_crystal_load(const si5351_dev *device, enum si5351_crystal_load load);

#endif
