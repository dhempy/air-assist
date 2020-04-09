

// Air Assist Ventilator Controller
//
// This program controls the Air Assist Ventilator.
// It activates solenoid valves to assist a patient inhaling and exhaling.
// It allows the practitioner to control the rate of respiration.
//
// Copyright (C) 2020 David Hempy
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


#define VERSION 0.0.4

//Uncomment to enable debug mode
#define DEBUG


/*
 * This script works just like Serial.print or Serial.println but it will only be called
 * when the #define DEBUG is uncommented.
 */
#ifdef DEBUG   
  #define DPRINT(...)    Serial.print(__VA_ARGS__)  
  #define DPRINTLN(...)  Serial.println(__VA_ARGS__) 
#else
  #define DPRINT(...)    
  #define DPRINTLN(...)   
#endif

byte VAL_INHALE;
byte VAL_EXHALE;
byte VAL_OXYGEN;

bool BTN_UP_STATE;
bool BTN_DOWN_STATE;
bool BTN_RATIO_STATE;
bool BTN_RATE_STATE;

long CYCLE_TIME = 3000;
long INHALE_TIME = 1000;
long EXHALE_TIME = 2000;
float IE_RATIO = 2.0;





enum state_enum {START, INHALE, EXHALE};

unsigned long now = millis();
unsigned long last_event = 0;
unsigned long wait_for = 0;
char current_state = START;




void setup() {
  #ifdef DEBUG
    Serial.begin(115200);
    delay(1000);
  #endif
  DPRINTLN("Air Assist");
  DPRINTLN("DEBUG: ENABLED");
  DPRINTLN("Loading Modules");
  
  initMOD_LCD();
  lcdPrint("Air Assist",0,0);
  initMOD_INPUT();
  initMOD_RELAY();
  //TODO: load settings from EEPROM


  
  DPRINTLN("Device Ready");
  lcdPrint("Device Ready",2, 1);

 // compute_delays();
}

void loop() {
  btnCheck();
  snrCheck();
  lcdOxygen();
  
  //now = millis();
  //if (now - last_event < wait_for) { return; }

 // advance_state();



 
}

/*
void advance_state() {
  state_dump("advance_state: PRE ");

  switch (current_state) {
    case INHALE:
      exhale();
      current_state = EXHALE;
      wait_for = EXHALE_TIME;
      break;

    case EXHALE:
      inhale;
      current_state = INHALE;
      wait_for = INHALE_TIME;
      break;

    default:
      current_state = EXHALE;
      wait_for = 500;
  }

  state_dump("advance_state: POST");
}

void inhale() {
  digitalWrite(RELAY_INHALE_PIN, HIGH);
  digitalWrite(RELAY_EXHALE_PIN, LOW);
}

void exhale() {
  digitalWrite(RELAY_INHALE_PIN, LOW);
  digitalWrite(RELAY_EXHALE_PIN, LOW);
}

void hold() {
  digitalWrite(RELAY_INHALE_PIN, LOW);
  digitalWrite(RELAY_EXHALE_PIN, LOW);
}






  // sprintf(msg, "UP:%d DN:%d RATE:%d RATIO:%d  <---- ADJUSTING...",
  //              up_pressed, down_pressed, rate_state, ratio_state);
  // Serial.println(msg);

  if (rate_state && up_pressed) { rate_increase(); }
  if (rate_state && down_pressed) { rate_decrease(); }
  if (ratio_state && up_pressed) { ratio_increase(); }
  if (ratio_state && down_pressed) { ratio_decrease(); }

  compute_delays();
}

void rate_increase() {
  CYCLE_TIME = min(6000, CYCLE_TIME + 100);
}

void rate_decrease() {
  CYCLE_TIME = max(2000, CYCLE_TIME - 100);
}

void ratio_increase() {
  IE_RATIO = min(3.0, IE_RATIO + 0.1);
}

void ratio_decrease() {
  IE_RATIO = max(1.5, IE_RATIO - 0.1);

}

void compute_delays() {
  // TODO: write tests.
  float one_cycle = 1.0 + IE_RATIO;
  INHALE_TIME = int((float)CYCLE_TIME / one_cycle);
  EXHALE_TIME = int(IE_RATIO * INHALE_TIME);

  settings_dump();
}

void state_dump(char *comment) {
  // sprintf(msg, "%ld (%ld) %.10s %d ", now, next_appt, comment, current_state);
  // Serial.println(msg);
}

void settings_dump() {
  char c[16];
  char ie[16];
  char i[16];
  char e[16];

  dtostrf((float)CYCLE_TIME/1000.0, 3, 1, c);
  dtostrf(IE_RATIO,                 3, 1, ie);
  dtostrf(INHALE_TIME/1000.0,  3, 1, i);
  dtostrf(EXHALE_TIME/1000.0,  3, 1, e);

  sprintf(msg, "C=%s I:E=%s i=%s e=%s", c, ie, i, e);
  Serial.println(msg);

  lcd.setCursor(0, 1);
  lcd.print(msg);
}
*/
