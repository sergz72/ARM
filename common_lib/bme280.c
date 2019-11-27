#include <board.h>
#include <bme280.h>

#define BME280_REGISTER_DIG_T1        0x88
#define BME280_REGISTER_DIG_H1        0xA1
#define BME280_REGISTER_DIG_H2        0xE1

/*
* Immutable calibration data read from bme280
*/
typedef struct __attribute__((__packed__))
{
  unsigned short dig_T1;
  short  dig_T2;
  short  dig_T3;

  unsigned short dig_P1;
  short  dig_P2;
  short  dig_P3;
  short  dig_P4;
  short  dig_P5;
  short  dig_P6;
  short  dig_P7;
  short  dig_P8;
  short  dig_P9;

  unsigned char  dig_H1;
  short  dig_H2;
  unsigned char  dig_H3;
  short  dig_H4;
  short  dig_H5;
  char   dig_H6;
} bme280_calib_data;

/*
* Raw sensor measurement data from bme280
*/
typedef struct __attribute__((__packed__))
{
  unsigned char pmsb;
  unsigned char plsb;
  unsigned char pxsb;

  unsigned char tmsb;
  unsigned char tlsb;
  unsigned char txsb;

  unsigned char hmsb;
  unsigned char hlsb;

  unsigned int temperature;
  unsigned int pressure;
  unsigned int humidity;

} bme280_raw_data;

static bme280_calib_data cal;

static int getTemperatureCalibration(int adc_T)
{
  int var1  = ((((adc_T>>3) - ((int)cal.dig_T1 <<1))) * ((int)cal.dig_T2)) >> 11;

  int var2  = (((((adc_T>>4) - ((int)cal.dig_T1)) *
       ((adc_T>>4) - ((int)cal.dig_T1))) >> 12) *
     ((int)cal.dig_T3)) >> 14;

  return var1 + var2;
}

unsigned int bme280_readCalibrationData(void)
{
  unsigned int rc = bme280Read(BME280_REGISTER_DIG_T1, (unsigned char*)&cal, 24);
  if (BME280_ERROR(rc))
    return rc;

  rc = bme280Read(BME280_REGISTER_DIG_H1, &cal.dig_H1, 1);
  if (BME280_ERROR(rc))
    return rc;
  rc = bme280Read(BME280_REGISTER_DIG_H2, (unsigned char*)&cal.dig_H2, 7);
  if (BME280_ERROR(rc))
    return rc;

  cal.dig_H6 = cal.dig_H5 >> 8;
  cal.dig_H5 = (cal.dig_H5 & 0xFF) << 4;
  if (cal.dig_H5 & 0x800)
    cal.dig_H5 |= 0xF000;
  cal.dig_H5 |= (cal.dig_H4 >> 12) & 0xF;
  cal.dig_H4 = ((cal.dig_H4 & 0xFF) << 4) | ((cal.dig_H4 >> 8) & 0xF);
  if (cal.dig_H4 & 0x800)
    cal.dig_H4 |= 0xF000;

  return rc;
}

static int compensateTemperature(int t_fine)
{
  return ((t_fine * 5 + 128) >> 8) / 10;
}

static int compensatePressure(int adc_P, int t_fine)
{
  long long var1, var2, p;

  var1 = ((long long)t_fine) - 128000;
  var2 = var1 * var1 * (long long)cal.dig_P6;
  var2 = var2 + ((var1*(long long)cal.dig_P5)<<17);
  var2 = var2 + (((long long)cal.dig_P4)<<35);
  var1 = ((var1 * var1 * (long long)cal.dig_P3)>>8) +
    ((var1 * (long long)cal.dig_P2)<<12);
  var1 = (((((long long)1)<<47)+var1))*((long long)cal.dig_P1)>>33;

  if (var1 == 0) {
    return 0;  // avoid exception caused by division by zero
  }
  p = 1048576 - adc_P;
  p = (((p<<31) - var2)*3125) / var1;
  var1 = (((long long)cal.dig_P9) * (p>>13) * (p>>13)) >> 25;
  var2 = (((long long)cal.dig_P8) * p) >> 19;

  p = ((p + var1 + var2) >> 8) + (((long long)cal.dig_P7)<<4);
  return p/256;
}


static int compensateHumidity(int adc_H, int t_fine)
{
  int v_x1_u32r;

  v_x1_u32r = (t_fine - ((int)76800));

  v_x1_u32r = (((((adc_H << 14) - (((int)cal.dig_H4) << 20) -
      (((int)cal.dig_H5) * v_x1_u32r)) + ((int)16384)) >> 15) *
         (((((((v_x1_u32r * ((int)cal.dig_H6)) >> 10) *
        (((v_x1_u32r * ((int)cal.dig_H3)) >> 11) + ((int)32768))) >> 10) +
      ((int)2097152)) * ((int)cal.dig_H2) + 8192) >> 14));

  v_x1_u32r = (v_x1_u32r - (((((v_x1_u32r >> 15) * (v_x1_u32r >> 15)) >> 7) *
           ((int)cal.dig_H1)) >> 4));

  v_x1_u32r = (v_x1_u32r < 0) ? 0 : v_x1_u32r;
  v_x1_u32r = (v_x1_u32r > 419430400) ? 419430400 : v_x1_u32r;
  return (v_x1_u32r>>12) / 1024;
}

static unsigned int getRawData(bme280_raw_data *raw, unsigned int getPressure)
{
  unsigned int rc;
  unsigned char c;

  do
  {
    rc = bme280Read(0xf3, &c, 1); // status
    if (BME280_ERROR(rc))
      return rc;
    c &= 8U;
    if (c)
      delayms(100);
  } while (c);

  if (getPressure)
    rc = bme280Read(0xf7, &raw->pmsb, 8);
  else
    rc = bme280Read(0xfa, &raw->tmsb, 5);
  if (BME280_ERROR(rc))
    return rc;

  raw->temperature = raw->tmsb << 8;
  raw->temperature = (raw->temperature | raw->tlsb) << 8;
  raw->temperature = (raw->temperature | raw->txsb) >> 4;

  if (getPressure)
  {
    raw->pressure = raw->pmsb << 8;
    raw->pressure = (raw->pressure | raw->plsb) << 8;
    raw->pressure = (raw->pressure | raw->pxsb) >> 4;
  }

  raw->humidity = raw->hmsb << 8;
  raw->humidity = (raw->humidity | raw->hlsb);

  return rc;
}

static unsigned int bme280_setup(unsigned int getPressure)
{
  unsigned int rc;
  rc = bme280Write(0xf2, 0x01);   // humidity oversampling x 1
  if (BME280_ERROR(rc))
    return rc;
  return bme280Write(0xf4, getPressure ? 0x25 : 0x21);   // pressure and temperature oversampling, mode forced
}

unsigned int bme280_get_data(bme280_data *data, unsigned int getPressure)
{
  unsigned int rc;

  rc = bme280_setup(getPressure);
  if (BME280_ERROR(rc))
    return rc;

  //bme280_readCalibrationData();

  bme280_raw_data raw;
  rc = getRawData(&raw, getPressure);
  if (BME280_ERROR(rc))
    return rc;

  int t_fine = getTemperatureCalibration(raw.temperature);
  data->temperature = compensateTemperature(t_fine); // C
  data->humidity = compensateHumidity(raw.humidity, t_fine);       // %
  if (getPressure)
    data->pressure = compensatePressure(raw.pressure, t_fine) / 100; // hPa

  return rc;
}
