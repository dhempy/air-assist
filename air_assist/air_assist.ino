// Air Assist Ventilator Controller
//
// This program controls the Air Assist Ventilator.
// It activates solenoid valves to assist a patient inhaling and exhaling.
// It allows the practitioner to control the rate of respiration.
//
// Copyright (C) 2020 David Hempy - See LICENSE file
//

const byte INHALE_PIN = 7;
const byte EXHALE_PIN = 6;

long INHALE_TIME       = 1000;
long INHALE_HOLD_TIME  = 250;
long EXHALE_TIME       = 3000;
long EXHALE_HOLD_TIME  = 500;

enum state_enum {INHALE, INHALE_HOLD, EXHALE, EXHALE_HOLD};

long now = millis();
long next_appt = 0;
int current_state = INHALE_HOLD; // precedes EXHALE

void setup() {
  Serial.begin(9600);
  Serial.println("Air Assist Activated");
  pinMode(INHALE_PIN, OUTPUT);
  pinMode(EXHALE_PIN, OUTPUT);
  pinMode(13, OUTPUT);
  digitalWrite(13, LOW);
}

void loop() {
  now = millis();

  status_blink(); // Visual confirmation that program is running.

  if (next_appt > now) {
    // TODO: This will fail when long int rolls over...about 20 days.
    //       Convert to a subtraction comparison to avoid that problem.
    return;
  }

  status_dump("PRE ");

  switch (current_state) {
    case INHALE:
      valve_hold();
      current_state = INHALE_HOLD;
      next_appt = now + INHALE_HOLD_TIME;
      break;

    case INHALE_HOLD:
      valve_exhale();
      current_state = EXHALE;
      next_appt = now + EXHALE_TIME;
      break;

    case EXHALE:
      valve_hold();
      current_state = EXHALE_HOLD;
      next_appt = now + EXHALE_HOLD_TIME;
      break;

    case EXHALE_HOLD:
      valve_inhale();
      current_state = INHALE;
      next_appt = now + INHALE_TIME;
      break;

    default:
      // Go immediately to EXHALE next loop.
      current_state = INHALE_HOLD;
      next_appt = now - 1;
  }

  status_dump("POST");
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

void status_dump(char *comment) {
  char msg[80];
  sprintf(msg, "%ld (%ld) %s %d ", now, next_appt, comment, current_state);
  Serial.println(msg);
}
