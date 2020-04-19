#include <Wire.h>
#include "Adafruit_MPRLS.h"

#define SNR_MPR_RST_PIN -1
#define SNR_MPR_EOC_PIN -1

#define HISTORY_SIZE 10

int pressure_hPa;  // Most recent pressure reading, in hectopascals. (typ. 970, range 500 - 1200 )
int pressure_history[HISTORY_SIZE];

Adafruit_MPRLS MPR = Adafruit_MPRLS(SNR_MPR_RST_PIN, SNR_MPR_EOC_PIN);

void init_mod_sensor() {
  dprint(" MOD_SENSOR: ");
  if(!MPR.begin()){
    dprint("ERROR");
    dprintln("Failed to find MPRLS Sensor");
  }else{
    dprintln("LOADED");
  }

  // seed pressure_history with current val:
  read_pressure();
  for(int n = 0; n < HISTORY_SIZE; n++) { pressure_history[n] = pressure_hPa; }
}

int read_pressure() {
  return pressure_hPa = MPR.readPressure(); // pressure in hectopascals (int)
}

//Loads sensor data
void snr_check() {
  int start;
  // int pressure_hPa;

  static byte newest = -1;

  start = millis();
  read_pressure();
  // pressure_hPa = MPR.readPressure();
  // pressure_pa = pressure_hPa * 100.0; // pressure_pa is int

  newest = ++newest % HISTORY_SIZE;
  pressure_history[newest] = pressure_hPa;
  int avg = avg_pressure();
  int diff = avg - pressure_hPa;

  snprintf(msg, MSG_LEN, "%d Pressure (pa): %d avg: %d diff: %d",
                          start, pressure_hPa, avg, diff);
  dprintln(msg);


  snprintf(msg, MSG_LEN, " %4d Pa", pressure_hPa);
  lcd_print(msg, 7, 0);
  snprintf(msg, MSG_LEN, " %4d Pa av", avg);
  lcd_print(msg, 7, 1);
  snprintf(msg, MSG_LEN, " %4d Pa Δ", diff);
  lcd_print(msg, 7, 2);

  // snprintf(msg, MSG_LEN, "%d %d %d %d", pressure_history[0], pressure_history[1], pressure_history[2], pressure_history[3] );
  // lcd_print(msg, 0, 3);

  // int l = pressure_hPa;
  // snprintf(msg, MSG_LEN, " %4ld hPa", l);
  // lcd_print(msg, 7, 3);

  int elapsed = millis() - start;
  if (elapsed > 100) { dprint("SLOW:"); dprintln(elapsed); }
}

int avg_pressure() {
  int sum = 0;

  for(int n = 0; n < HISTORY_SIZE; n++) {
    sum += pressure_history[n];
  }
  dprint(" sum:"); dprint(sum);
  dprint(" len:"); dprint(HISTORY_SIZE);
  dprint(" avg:"); dprint(sum / HISTORY_SIZE);
  dprintln();

  return sum / HISTORY_SIZE;
}
