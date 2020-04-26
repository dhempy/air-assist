
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
#include <MemoryFree.h>  // Remove later
#include "pinout.h"

#define VERSION "0.0.6"

//Uncomment to enable debug mode
#define DEBUG

#ifdef DEBUG
  /* dprint() works just like Serial.print or Serial.println, when DEBUG is defined. */
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


float snr_mpr_psi;

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

bool inhale_triggered = false;

char cycle_state;

char device_state;
char MENU_SELECT;
bool menu_show = false;
int menu_duration = 10000;
unsigned long menu_started_at;

#define MSG_LEN 255
char msg[MSG_LEN + 1];

void setup() {
  snprintf(msg, MSG_LEN, "Air Assist v%s", VERSION);
  lcd_print(msg, 0,1);

  #ifdef DEBUG
    Serial.begin(115200);
    for (byte tries = 0; !Serial && tries <=100; tries++) {
      delay(10);
    }
  #endif

  dprintln(msg);
  dprintln("DEBUG: ENABLED");
  dprintln("Loading Modules");

  init_mod_lcd();
  init_mod_input();
  init_mod_relay();
  init_mod_eeprom();
  init_mod_menu();
  init_mod_sensor();

  calc_bpm();

  pinMode(PIN_LED, OUTPUT);

  device_state = READY;
  lcd_print("RDY", 17, 0);
  dprintln("end of init!");

}

unsigned long pressure_taken_at = 0;
#define pressure_interval   50     // ms - Time between pressure readings

unsigned long now = millis();

void loop() {
  // delay(100); // development only.
  now = millis();
  profile_reset();

  blink();
  profile(1, "blink");

  if (now - pressure_taken_at >= pressure_interval) {
    snr_check();
    pressure_taken_at = now;
    profile(1, "snr_check");
  }

  btn_menu_check();
  profile(1, "btn_menu_check");

  if (menu_show) menu_select();
  profile(1, "menu_select");

  // calc_bpm();  // TODO: Only calc BPM after a parameter change.

  switch(device_state) {
    case READY:
      break;
    case RUNNING:
      // lcd_print("DEVICE RUNNING", 3, 0);
      if (!menu_show) {
        lcd_show_sensors();
      }
      cycle_respiration();
      break;
    case STOP:
      // lcd_print("DEVICE STOPPED", 3, 0);
      rly_close(INHALE);
      rly_close(EXHALE);
      break;
    case ERROR:
      // lcd_print("DEVICE ERROR", 4, 0);
      break;
  }
  profile(1, "loop end");
  // dprintln();
}

void cycle_respiration() {
  switch(cycle_state) {
    case INHALE:
      // if (!menu_show) {
      //   lcd_print("IN", 18, 3);
      // }
      if (inhale_started_at == 0) {
        inhale_started_at = millis();
        rlyOPEN(INHALE);
        rly_close(EXHALE);
      }
      if (millis() - inhale_started_at > inhale_duration) {
        cycle_state = EXHALE;
        lcd_print("EX", 18, 3); // TODO: move this to start_exhale() so it only happens once.
        inhale_triggered = false;
        exhale_started_at = millis();
        rlyOPEN(EXHALE);
        rly_close(INHALE);
      }
      break;

    case EXHALE:
      // if (!menu_show) {
      //   lcd_print("EX", 18, 3); 
      // }
      if (exhale_started_at == 0) {
        exhale_started_at = millis();
        rlyOPEN(EXHALE);
        rly_close(INHALE);
      }
      if ( inhale_triggered ||
           (millis() - exhale_started_at > exhale_duration)
         ) {
        cycle_state = INHALE;
        lcd_print("IN", 18, 3);
        inhale_started_at = millis();
        rlyOPEN(INHALE);
        rly_close(EXHALE);
      }
      break;

    default:
      cycle_state = INHALE;
  }
}

#define MIN_EXHALE_BEFORE_TRIGGER 500

void trigger_inhale_cycle() {
  if (now - exhale_started_at < MIN_EXHALE_BEFORE_TRIGGER) {
    dprintln("Too soon for inhale.");
    return;
  }

  dprintln("PATIENT INITIATED AN INHALE CYCLE");
  inhale_triggered = true;
}

void calc_bpm() { // Calculates the inhale and exhale times based on the BPM(Breaths Per Minute) and the ratio
  float cycle_duration = (60000 / val_bpm);

  dprintln("calc_bpm...");

  inhale_duration = int((float)cycle_duration / (val_ie_ratio + 1.0));
  exhale_duration = cycle_duration - inhale_duration;
}


// Visual indicator that program is still running.
void blink() {
  static byte blink_on = 0;

  if ((now % 1000) < 300) {
    if (!blink_on) {
      blink_on = 1;
      digitalWrite(PIN_LED, blink_on);
      // show_mem();
      // dprint('.');
    }
  } else if (blink_on) {
    blink_on = 0;
    digitalWrite(PIN_LED, blink_on);
  }
}

void show_mem() {
//    int fm = freeMemory();
//    dprintln(fm);
//    lcd_print(String(fm), 0, 2);
//    lcd_print(String(millis()), 0, 3);
}


#define PROFILE_THRESHHOLD  100
long unsigned int last_profile = 0;
void profile_reset() {
  last_profile = now;
}

void profile(int level, char *label) {
  if (millis() - last_profile <= PROFILE_THRESHHOLD) return;  // Don't sweat the small stuff.

  snprintf(msg, MSG_LEN, "%4lu(loop)/%lu(step) %d %s",
    millis() - now,
    millis() - last_profile,
    level,
    label
  );

  dprintln(msg);

  last_profile = millis();
}
