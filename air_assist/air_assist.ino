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

#include <LiquidCrystal.h>

const byte RX_PIN                = 0;
const byte TX_PIN                = 1;
const byte BUTTON_UP_PIN         = 2;
const byte BUTTON_DOWN_PIN       = 3;
const byte RELAY_4_PIN           = 4;
const byte RELAY_ALARM_PIN       = 5;
const byte RELAY_EXHALE_PIN      = 6;
const byte RELAY_INHALE_PIN      = 7;
const byte BUTTON_RATIO_PIN      = 8;
const byte BUTTON_RATE_PIN       = 9;
const byte LCD_D4_PIN            = 10;
const byte LCD_D5_PIN            = 11;
const byte LCD_D6_PIN            = 12;
const byte LCD_D7_PIN            = 13;
const byte EXH_PRESSURE_PIN      = A0;
const byte INH_PRESSURE_PIN      = A1;
const byte OXY_SENSOR_PIN        = A2;
const byte ZZZ_PIN               = A3;
const byte LCD_RS_PIN            = A4;
const byte LCD_ENABLE_PIN        = A5;

long cycle_time  = 3000;
float ie_ratio    = 2.0;
long inhale_hold_time  = 1000;
long exhale_hold_time  = 2000;

enum state_enum {START, INHALE, EXHALE};

unsigned long now = millis();
unsigned long last_event = 0;
unsigned long wait_for = 0;
char current_state = START;

char msg[255];

LiquidCrystal lcd(LCD_ENABLE_PIN, LCD_RS_PIN,
  LCD_D4_PIN, LCD_D5_PIN, LCD_D6_PIN, LCD_D7_PIN);

void setup() {
  lcd.begin(16, 2);
  lcd.print("Air Assist");

  Serial.begin(9600);
  Serial.println("\n----------------- Air Assist Activating -----------------\n");

  pinMode(BUTTON_UP_PIN,     INPUT_PULLUP);
  pinMode(BUTTON_DOWN_PIN,   INPUT_PULLUP);
  pinMode(BUTTON_RATIO_PIN,  INPUT_PULLUP);
  pinMode(BUTTON_RATE_PIN,   INPUT_PULLUP);

  pinMode(RELAY_4_PIN,       OUTPUT);
  pinMode(RELAY_ALARM_PIN,   OUTPUT);
  pinMode(RELAY_EXHALE_PIN,  OUTPUT);
  pinMode(RELAY_INHALE_PIN,  OUTPUT);

  compute_delays();
}

void loop() {
  check_buttons();
  now = millis();
  if (now - last_event < wait_for) { return; }

  advance_state();
}

void advance_state() {
  state_dump("advance_state: PRE ");

  switch (current_state) {
    case INHALE:
      exhale();
      current_state = EXHALE;
      wait_for = exhale_hold_time;
      break;

    case EXHALE:
      inhale;
      current_state = INHALE;
      wait_for = inhale_hold_time;
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


char last_up_state = 1;
char last_down_state = 1;
char last_ratio_state = 1;
char last_rate_state = 1;

char button_stats[128];


void check_buttons() {
  char up_state    = !digitalRead(BUTTON_UP_PIN);
  char down_state  = !digitalRead(BUTTON_DOWN_PIN);
  char ratio_state = !digitalRead(BUTTON_RATIO_PIN);
  char rate_state  = !digitalRead(BUTTON_RATE_PIN);
  char up_pressed   = 0;
  char down_pressed = 0;

  if (up_state != last_up_state) {
    up_pressed = up_state;
    last_up_state = up_state;
  }

  if (down_state != last_down_state) {
    down_pressed = down_state;
    last_down_state = down_state;
  }

  if (
      (rate_state && ratio_state) ||
      (!rate_state && !ratio_state) ||
      (up_pressed && down_pressed) ||
      (!up_pressed && !down_pressed)
     ) {
    return;
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
  cycle_time = min(6000, cycle_time + 100);
}

void rate_decrease() {
  cycle_time = max(2000, cycle_time - 100);
}

void ratio_increase() {
  ie_ratio = min(3.0, ie_ratio + 0.1);
}

void ratio_decrease() {
  ie_ratio = max(1.5, ie_ratio - 0.1);

}

void compute_delays() {
  // TODO: write tests.
  float one_cycle = 1.0 + ie_ratio;
  inhale_hold_time = int((float)cycle_time / one_cycle);
  exhale_hold_time = int(ie_ratio * inhale_hold_time);

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

  dtostrf((float)cycle_time/1000.0, 3, 1, c);
  dtostrf(ie_ratio,                 3, 1, ie);
  dtostrf(inhale_hold_time/1000.0,  3, 1, i);
  dtostrf(exhale_hold_time/1000.0,  3, 1, e);

  sprintf(msg, "C=%s I:E=%s i=%s e=%s", c, ie, i, e);
  Serial.println(msg);

  lcd.setCursor(0, 1);
  lcd.print(msg);
}

