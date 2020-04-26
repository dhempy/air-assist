#include <Wire.h>
#include "Adafruit_MPRLS.h"

#define SNR_MPR_RST_PIN -1
#define SNR_MPR_EOC_PIN -1

#define HISTORY_SIZE 10

int pressure_hPa = 0;  // Most recent pressure reading, in hectopascals. (typ. 970, range 500 - 1200 )
int pressure_history[HISTORY_SIZE];

int rolling_pressure99;
int rolling_pressure95;
int rolling_pressure50;
int rolling_pressure05;
int rolling_pressure01;

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
  rolling_pressure99 = pressure_hPa;
  rolling_pressure95 = pressure_hPa;
  rolling_pressure50 = pressure_hPa;
  rolling_pressure05 = pressure_hPa;
  rolling_pressure01 = pressure_hPa;

}

#define NOISE_THRESHHOLD 100
// Limit changes in pressure to that big of a jump.

int read_pressure() {
  int new_pressure = MPR.readPressure(); // pressure in hectopascals (int)
  // dprintln("RAW READING:"); dprintln(MPR.readPressure());

  if (pressure_hPa == 0) {
    pressure_hPa = new_pressure;
  } else if (new_pressure - pressure_hPa > NOISE_THRESHHOLD) {
    pressure_hPa += NOISE_THRESHHOLD;
  } else if (pressure_hPa - new_pressure > NOISE_THRESHHOLD) {
    pressure_hPa -= NOISE_THRESHHOLD;
  } else {
    pressure_hPa = new_pressure;
  }

  return pressure_hPa;
}



int record_pressure() {
  static byte newest = -1;

  newest = ++newest % HISTORY_SIZE;
  pressure_history[newest] = pressure_hPa;

  rolling_pressure99 = pressure_hPa * 0.01 + rolling_pressure99 * 0.99;
  rolling_pressure95 = pressure_hPa * 0.05 + rolling_pressure95 * 0.95;
  rolling_pressure50 = pressure_hPa * 0.50 + rolling_pressure50 * 0.50;
  rolling_pressure05 = pressure_hPa * 0.95 + rolling_pressure05 * 0.05;
  rolling_pressure01 = pressure_hPa * 0.99 + rolling_pressure01 * 0.01;
}


//Loads sensor data
void snr_check() {
  long int start = millis();
  int prev_pressure = pressure_hPa;
  int actual_time_interval = start - pressure_taken_at;
  int elapsed;

  read_pressure(); // takes about 12 ms
  profile(2, "sensor.read_pressure (after)");

  elapsed = millis() - now;
  if (elapsed > 100) { dprint("SLOW:"); dprintln(elapsed); }

  record_pressure(); // takes about 0.5 ms
  profile(2, "sensor.record_pressure");

  int avg = avg_pressure();
  int diff_prev = pressure_hPa - prev_pressure;
  int diff_avg = pressure_hPa - avg;
  int diff_rolling99 = pressure_hPa - rolling_pressure99;
  int diff_rolling95 = pressure_hPa - rolling_pressure95;
  int diff_rolling50 = pressure_hPa - rolling_pressure50;
  int diff_rolling05 = pressure_hPa - rolling_pressure05;
  int diff_rolling01 = pressure_hPa - rolling_pressure01;

  // profile(2, "sensor.averages");

//                     avg:969 diff:0 rolling_pressure:1130 diff:161
// 63511055 Pressure (pa): 969 [avg: 0 diff: 1130] [roll: 161 diff 1835] ...

  snprintf(msg, MSG_LEN, "%d/%d -- avg_diff: %d 50_diff: %d 05_diff: %d 01_diff: %d ...",
                           actual_time_interval,
                           pressure_interval,
                           diff_avg,
                           diff_rolling50,
                           diff_rolling05,
                           diff_rolling01
                          );
  dprintln(msg);
  profile(2, "sensor.debug");

  snprintf(msg, MSG_LEN, "%4d hPa %+03d", pressure_hPa, (pressure_hPa - prev_pressure));
  lcd_print(msg, 0, 0); // takes about 22ms
  profile(2, "sensor.lcd_print");

  // TODO next: trigger breaths based on pressure.
  // TODO next: trigger breaths based on pressure.
  // TODO next: trigger breaths based on pressure.
  // TODO next: trigger breaths based on pressure.

  int triggers = is_patient_inhaling(diff_prev) +
                 is_patient_inhaling(diff_avg) +
                 is_patient_inhaling(diff_rolling50);

  // look for at least 2 out of the 3 potential triggers to start an inhale:
  dprint("triggers: "); dprintln(triggers);
  if (triggers > 1) { trigger_inhale_cycle(); }

  dump_press(0, "raw", pressure_hPa, diff_prev);
  profile(2, "sensor.dump 1");

  dump_press(1, "avg", avg, diff_avg);
  profile(2, "sensor.dump 2");

  dump_press(2, "r50", rolling_pressure50, diff_rolling50);
  profile(2, "sensor.dump 3");

  dump_press(3, "r05", rolling_pressure05, diff_rolling05);
  profile(2, "sensor.dump 4");

  profile(2, "sensor end");
}

#define INHALE_TRIGGER  -2
#define EXHALE_TRIGGER  +2

int is_patient_inhaling(int diff) {
  return diff <= INHALE_TRIGGER;
}

// int patient_action(int diff) {
//   if (diff <= INHALE_TRIGGER) {
//     return -1;
//   } else if (diff >= EXHALE_TRIGGER) {
//     return +1;
//   } else {
//     return 0;
//   }
// }

void dump_press(int row, char *label, int avg, int diff) {
  return;  // suppress for now. Too slow!

  snprintf(msg, MSG_LEN, "%s %4d hPa %+02d %s",
    label, avg, diff, diff <= INHALE_TRIGGER ? "IN" : diff >= EXHALE_TRIGGER ? "EX" : "  ");
  profile(3, "sensor.dump.sprintf");
  lcd_print(msg, 0, row);
  profile(3, "sensor.dump.lcd_print"); // takes a whopping 50 ms!
}


int avg_pressure() {
  int sum = 0;

  for(int n = 0; n < HISTORY_SIZE; n++) {
    sum += pressure_history[n];
  }
  // dprint(" sum:"); dprint(sum);
  // dprint(" len:"); dprint(HISTORY_SIZE);
  // dprint(" avg:"); dprint(sum / HISTORY_SIZE);
  // dprintln();

  return sum / HISTORY_SIZE;
}
