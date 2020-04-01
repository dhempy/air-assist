// Air Assist Ventilator Controller
//
// This program controls the Air Assist Ventilator.
// It activates solenoid valves to assist a patient inhaling and exhaling.
// It allows the practitioner to control the rate of respiration.
//
// Copyright (C) 2020 David Hempy - See LICENSE file
//

const byte INHALE_PIN      = 7;
const byte EXHALE_PIN      = 6;
const byte INHALE_MORE_PIN = 5;
const byte INHALE_LESS_PIN = 4;
const byte EXHALE_MORE_PIN = 3;
const byte EXHALE_LESS_PIN = 2;


long inhale_time       = 1000;
long inhale_hold_time  = 250;
long exhale_time       = 3000;
long exhale_hold_time  = 250;

enum state_enum {INHALE, INHALE_HOLD, EXHALE, EXHALE_HOLD};

long now = millis();
long next_appt = 0;
int current_state = INHALE_HOLD; // precedes EXHALE

char msg[255];

void setup() {
  Serial.begin(9600);
  Serial.println("Air Assist Activated");

  pinMode(INHALE_PIN, OUTPUT);
  pinMode(EXHALE_PIN, OUTPUT);

  pinMode(INHALE_MORE_PIN, INPUT_PULLUP);
  pinMode(INHALE_LESS_PIN, INPUT_PULLUP);
  pinMode(EXHALE_MORE_PIN, INPUT_PULLUP);
  pinMode(EXHALE_LESS_PIN, INPUT_PULLUP);

  pinMode(13, OUTPUT);
  digitalWrite(13, LOW);
}

void loop() {
  now = millis();

  status_blink(); // Visual confirmation that program is running.

  check_buttons();

  if (next_appt > now) {
    // TODO: This will fail when long int rolls over...about 50 days.
    //       Convert to a subtraction comparison to avoid that problem.
    return;
  }

  state_dump("PRE ");

  switch (current_state) {
    case INHALE:
      valve_hold();
      current_state = INHALE_HOLD;
      next_appt = now + inhale_hold_time;
      break;

    case INHALE_HOLD:
      valve_exhale();
      current_state = EXHALE;
      next_appt = now + exhale_time;
      break;

    case EXHALE:
      valve_hold();
      current_state = EXHALE_HOLD;
      next_appt = now + exhale_hold_time;
      break;

    case EXHALE_HOLD:
      valve_inhale();
      current_state = INHALE;
      next_appt = now + inhale_time;
      break;

    default:
      // Go immediately to EXHALE next loop.
      current_state = INHALE_HOLD;
      next_appt = now - 1;
  }

  state_dump("POST");
}

void valve_inhale() {
  digitalWrite(EXHALE_PIN, LOW);
  digitalWrite(INHALE_PIN, HIGH);
}

void valve_exhale() {
  digitalWrite(INHALE_PIN, LOW);
  digitalWrite(EXHALE_PIN, HIGH);
}

void valve_hold() {
  digitalWrite(INHALE_PIN, LOW);
  digitalWrite(EXHALE_PIN, LOW);
}

void status_blink() {
  digitalWrite(13, HIGH);
  delay(25);
  digitalWrite(13, LOW);
  delay(25);
}

char last_inhale_more_press = 99;
char last_inhale_less_press = 99;
char last_exhale_more_press = 99;
char last_exhale_less_press = 99;

void check_buttons() {
  char inhale_more_press = digitalRead(INHALE_MORE_PIN);
  char inhale_less_press = digitalRead(INHALE_LESS_PIN);
  char exhale_more_press = digitalRead(EXHALE_MORE_PIN);
  char exhale_less_press = digitalRead(EXHALE_LESS_PIN);

  bool activity = 0;

  if (inhale_more_press != last_inhale_more_press) {
    if (inhale_more_press == 0) {
      inhale_time = min(10000, inhale_time + 100);
      activity = 1;
    }
    last_inhale_more_press = inhale_more_press;
  }

  if (inhale_less_press != last_inhale_less_press) {
    if (inhale_less_press == 0) {
      inhale_time = max(100, inhale_time - 100);
      activity = 1;
    }
    last_inhale_less_press = inhale_less_press;
  }

  if (exhale_more_press != last_exhale_more_press) {
    if (exhale_more_press == 0) {
      exhale_time = min(10000, exhale_time + 100);
      activity = 1;
    }
    last_exhale_more_press = exhale_more_press;
  }

  if (exhale_less_press != last_exhale_less_press) {
    if (exhale_less_press == 0) {
      exhale_time = max(100, exhale_time - 100);
      activity = 1;
    }
    last_exhale_less_press = exhale_less_press;
  }

  if (activity) {
    // sprintf(msg, "INHALE_MORE: %d; INHALE_LESS: %d; EXHALE_MORE: %d; EXHALE_LESS: %d",
    //   inhale_more_press,
    //   inhale_less_press,
    //   exhale_more_press,
    //   exhale_less_press
    // );
    // Serial.println(msg);

    time_dump();
  }
}


void state_dump(char *comment) {
  // sprintf(msg, "%ld (%ld) %.10s %d ", now, next_appt, comment, current_state);
  // Serial.println(msg);
}

void time_dump() {
  sprintf(msg, "inhale time: %ld; exhale time: %ld", inhale_time, exhale_time);
  Serial.println(msg);
}
