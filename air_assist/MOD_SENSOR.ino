#include <Wire.h>
#include "Adafruit_MPRLS.h"

#define SNR_MPR_RST_PIN -1
#define SNR_MPR_EOC_PIN -1

#define HISTORY_SIZE 20

unsigned long pressure_pa;  // Most recent pressure reading, in pascals. (typ. 97,999 )
unsigned long pressure_history[HISTORY_SIZE];

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
  pressure_pa = read_pressure();
  for(int n = 0; n < HISTORY_SIZE; n++) { pressure_history[n] = pressure_pa; }
}

unsigned long read_pressure() {
  float pressure_hPa;
  pressure_hPa = MPR.readPressure(); // pressure in hectopascals (float)
  return pressure_hPa * 100;         // pressure in pascals (int)
}

//Loads sensor data
void snr_check() {
  unsigned long start;
  float pressure_hPa;

  static byte newest = -1;

  start = millis();
  pressure_hPa = MPR.readPressure();
  pressure_pa = pressure_hPa * 100.0; // pressure_pa is int

  newest = ++newest % HISTORY_SIZE;
  pressure_history[newest] = pressure_pa;
  unsigned long avg = avg_pressure();

  snprintf(msg, MSG_LEN, "%8ld Pressure (pa): %lu  avg: %lu  diff: %d   ",
                          start, pressure_pa, avg, avg - pressure_pa);
  dprintln(msg);

  lcd_print(String(pressure_pa), 0, 0);

  int elapsed = millis() - start;
  if (elapsed > 100) { dprint("SLOW:"); dprintln(elapsed); }
}

long unsigned int avg_pressure() {
  long unsigned int sum = 0;

  for(int n = 0; n < HISTORY_SIZE; n++) {
    sum += pressure_history[n];
  }

  return sum / HISTORY_SIZE;
}
