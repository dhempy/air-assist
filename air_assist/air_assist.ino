

// Air Assist Ventilator Controller
//
// This program controls the Air Assist Ventilator.
// It activates solenoid valves to assist a patient inhaling and exhaling.
// It allows the practitioner to control the rate of respiration.
//
// Copyright (C) 2020 David Hempy, Russell Hummerick
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.
//
#include <EEPROM.h>


#define VERSION 0.0.5

//Uncomment to enable debug mode
#define DEBUG


/*
 * This script works just like Serial.print or Serial.println but it will only be called
 * when the #define DEBUG is uncommented.
 */
#ifdef DEBUG
  #define dprint(...)    Serial.print(__VA_ARGS__)
  #define dprintln(...)  Serial.println(__VA_ARGS__)
#else
  #define dprint(...)
  #define dprintln(...)
#endif

#define BPM_DEFAULT        12
#define BPM_MIN            10
#define BPM_MAX            20
#define BPM_INCREMENT     100

#define IE_RATIO_DEFAULT    2.5
#define IE_RATIO_MIN        1.5
#define IE_RATIO_DEFAULT    3.0
#define IE_RATIO_INCREMENT  0.25

#define FOREACH_LABEL(LABEL) \
        LABEL(UP)   \
        LABEL(DOWN)   \
        LABEL(SELECT)   \
        LABEL(READY)   \
        LABEL(START)  \
        LABEL(STOP)   \
        LABEL(INHALE) \
        LABEL(EXHALE)  \
        LABEL(RUNNING)  \
        LABEL(ERROR)  \
        LABEL(SETTINGS)  \

#define GENERATE_ENUM(ENUM) ENUM,
#define GENERATE_STRING(STRING) #STRING,



enum ENUM_LABELS {
    FOREACH_LABEL(GENERATE_ENUM)
};

static const char *LABEL_STR[] = {
    FOREACH_LABEL(GENERATE_STRING)
};

byte val_inhale;
byte val_exhale;
byte val_oxygen;

bool rly_inhale_state;
bool rly_exhale_state;
bool rly_alarm_state;
bool rly_4_state;

byte val_bpm = BPM_DEFAULT;
float val_ie_ratio = IE_RATIO_DEFAULT;

unsigned long inhale_started_at;
unsigned long exhale_started_at;
int exhale_duration;
int inhale_duration;

char cycle_state;

char device_state;
char MENU_SELECT;
bool menu_show = true;
int menu_duration = 10000;
unsigned long menu_started_at;

#define MSG_LEN 254
char msg[MSG_LEN + 1];

void setup() {
  #ifdef DEBUG
    Serial.begin(115200);
    delay(1000);
  #endif

  dprintln("Air Assist");
  dprintln("DEBUG: ENABLED");
  dprintln("Loading Modules");

  init_mod_lcd();
  init_mod_input();
  init_mod_relay();
  init_mod_eeprom();
  init_mod_menu();

  test_calc_bpm();

  device_state = READY;
}

void loop() {
  btn_menu_check();
  snr_check();
  menu_select();
  calc_bpm();

  switch(device_state) {
    case READY:
      lcd_print("DEVICE READY", 4, 0);
      break;
    case RUNNING:
      lcd_print("DEVICE RUNNING", 3, 0);
      if (!menu_show) {
        //lcd_show_sensors();
      }
      cycle_respiration();
      break;
    case STOP:
      lcd_print("DEVICE STOPPED", 3, 0);
      rly_close(INHALE);
      rly_close(EXHALE);
      break;
    case ERROR:
      lcd_print("DEVICE ERROR", 4, 0);
      break;
  }
}

void cycle_respiration() {
  switch(cycle_state) {
    case INHALE:
      if (!menu_show) {
        lcd_print(LABEL_STR[INHALE], 7, 1);
      }
      if (inhale_started_at == 0) {
        inhale_started_at = millis();
        rlyOPEN(INHALE);
        rly_close(EXHALE);
      }
      if (millis() - inhale_started_at > inhale_duration) {
        cycle_state = EXHALE;
        exhale_started_at = millis();
        rlyOPEN(EXHALE);
        rly_close(INHALE);
      }
      break;

    case EXHALE:
      if (!menu_show) {
        lcd_print(LABEL_STR[EXHALE], 7, 1);
      }
      if (exhale_started_at == 0) {
        exhale_started_at = millis();
        rlyOPEN(EXHALE);
        rly_close(INHALE);
      }
      if (millis() - exhale_started_at > exhale_duration) {
        cycle_state = INHALE;
        inhale_started_at = millis();
        rlyOPEN(INHALE);
        rly_close(EXHALE);
      }
      break;

    default:
      cycle_state = INHALE;
  }
}

void calc_bpm() { // Calculates the inhale and exhale times based on the BPM(Breaths Per Minute) and the ratio
  float cycle_duration = (60000 / val_bpm);
  inhale_duration = int((float)cycle_duration / (val_ie_ratio + 1.0));
  exhale_duration = cycle_duration - inhale_duration;
}

void test_calc_bpm() {
  test_calc_bpm_example(15, 3.0, 1000, 3000);
  test_calc_bpm_example(20, 1.0, 1500, 1500);
  test_calc_bpm_example(12, 2.0, 1666, 3334);
  test_calc_bpm_example(13, 2.5, 1318, 3297);

  val_bpm = BPM_DEFAULT;
  val_ie_ratio = IE_RATIO_DEFAULT;
}

void test_calc_bpm_example(byte bpm, float ie_ratio, int expected_inhale_time, int expected_exhale_time) {
  val_bpm = bpm;
  val_ie_ratio = ie_ratio;

  calc_bpm();

  if (inhale_duration != expected_inhale_time) { dprintln("Oops: ============>"); }

  snprintf(msg, MSG_LEN, "For (bpm=%d, ie=%s), expected inhale_duration to be %d and got %d",
                          bpm, f2s(ie_ratio), expected_inhale_time, inhale_duration);
  dprintln(msg);

  if (exhale_duration != expected_exhale_time) { dprintln("Oops: ============>"); }
  snprintf(msg, MSG_LEN, "For (bpm=%d, ie=%s), expected exhale_duration to be %d and got %d",
                          bpm, f2s(ie_ratio), expected_exhale_time, exhale_duration);
  dprintln(msg);
}

char *f2s(float val) {
  static char fstr[6];  // Note - this will fail if you have two calls to f2s simultaneously.
  dtostrf(val, 5, 3, fstr);
  return fstr;
}
