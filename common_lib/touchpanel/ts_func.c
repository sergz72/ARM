#include <board.h>
#include <ts_func.h>

void ts_calibrate(ts_calibrate_point *p)
{
  int i;
  TS_DATATYPE a[3],b[3],c[3],d[3],k;
  ts_calibration_data dt;

  for(i=0; i<3; i++)
  {
    a[i]=0;
    b[i]=0;
    c[i]=0;
    d[i]=0;
  }

  for(i=0; i<TS_CALIBRATE_POINTS; i++)
  {
    a[2]=a[2]+(TS_DATATYPE)(p[i].adc_val.x);
    b[2]=b[2]+(TS_DATATYPE)(p[i].adc_val.y);
    c[2]=c[2]+(TS_DATATYPE)(p[i].screen.x);
    d[2]=d[2]+(TS_DATATYPE)(p[i].screen.y);
    a[0]=a[0]+(TS_DATATYPE)(p[i].adc_val.x)*(TS_DATATYPE)(p[i].adc_val.x);
    a[1]=a[1]+(TS_DATATYPE)(p[i].adc_val.x)*(TS_DATATYPE)(p[i].adc_val.y);
    b[0]=a[1];
    b[1]=b[1]+(TS_DATATYPE)(p[i].adc_val.y)*(TS_DATATYPE)(p[i].adc_val.y);
    c[0]=c[0]+(TS_DATATYPE)(p[i].adc_val.x)*(TS_DATATYPE)(p[i].screen.x);
    c[1]=c[1]+(TS_DATATYPE)(p[i].adc_val.y)*(TS_DATATYPE)(p[i].screen.x);
    d[0]=d[0]+(TS_DATATYPE)(p[i].adc_val.x)*(TS_DATATYPE)(p[i].screen.y);
    d[1]=d[1]+(TS_DATATYPE)(p[i].adc_val.y)*(TS_DATATYPE)(p[i].screen.y);
  }
  a[0]=a[0]/a[2];
  a[1]=a[1]/b[2];
  b[0]=b[0]/a[2];
  b[1]=b[1]/b[2];
  c[0]=c[0]/a[2];
  c[1]=c[1]/b[2];
  d[0]=d[0]/a[2];
  d[1]=d[1]/b[2];
  a[2]=a[2]/TS_CALIBRATE_POINTS;
  b[2]=b[2]/TS_CALIBRATE_POINTS;
  c[2]=c[2]/TS_CALIBRATE_POINTS;
  d[2]=d[2]/TS_CALIBRATE_POINTS;

  k=(a[0]-a[2])*(b[1]-b[2])-(a[1]-a[2])*(b[0]-b[2]);
  dt.alpha_x=((c[0]-c[2])*(b[1]-b[2])-(c[1]-c[2])*(b[0]-b[2]))/k;
  dt.beta_x=((c[1]-c[2])*(a[0]-a[2])-(c[0]-c[2])*(a[1]-a[2]))/k;
  dt.delta_x=(b[0]*(a[2]*c[1]-a[1]*c[2])+b[1]*(a[0]*c[2]-a[2]*c[0])+b[2]*(a[1]*c[0]-a[0]*c[1]))/k;
  dt.alpha_y=((d[0]-d[2])*(b[1]-b[2])-(d[1]-d[2])*(b[0]-b[2]))/k;
  dt.beta_y=((d[1]-d[2])*(a[0]-a[2])-(d[0]-d[2])*(a[1]-a[2]))/k;
  dt.delta_y=(b[0]*(a[2]*d[1]-a[1]*d[2])+b[1]*(a[0]*d[2]-a[2]*d[0])+b[2]*(a[1]*d[0]-a[0]*d[1]))/k;

  TS_SAVE_CALIBRATION_DATA(&dt);
}

ts_point ts_get_touch_pos(void)
{
  ts_calibration_data dt;
  ts_point p, adc_val;

  TS_GET_CALIBRATION_DATA(&dt);

  adc_val = ts_get_adc_val();

  if (adc_val.x < 0) return adc_val;

  p.x = dt.alpha_x * adc_val.x + dt.beta_x * adc_val.y + dt.delta_x;
  p.y = dt.alpha_y * adc_val.x + dt.beta_y * adc_val.y + dt.delta_y;

  return p;
}
