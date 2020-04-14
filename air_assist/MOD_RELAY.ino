#define PIN_INHALE 8  //Inhale Valve
#define PIN_RLY_2 9
#define PIN_RLY_3 10  //Alarm
#define PIN_RLY_4 11

//What signal opens the relay
#define RLY_OPEN_STATE HIGH

void init_mod_relay() {
  delay(500);
  dprintln(" MOD_RELAY: LOADED");
  //Declares relay pins
  pinMode(PIN_INHALE, OUTPUT);
  pinMode(PIN_RLY_2, OUTPUT);
  pinMode(PIN_RLY_3, OUTPUT);
  pinMode(PIN_RLY_4, OUTPUT);
  //closes all relays for startup.
  digitalWrite(PIN_INHALE, !RLY_OPEN_STATE);
  digitalWrite(PIN_RLY_2, !RLY_OPEN_STATE);
  digitalWrite(PIN_RLY_3, !RLY_OPEN_STATE);
  digitalWrite(PIN_RLY_4, !RLY_OPEN_STATE);
}

void rlyOPEN(char relay) {
  dprintln("Relay " + String(relay) + ": Open");

  switch(relay) {
    case INHALE:
      digitalWrite(PIN_INHALE, RLY_OPEN_STATE);
      rly_inhale_state = RLY_OPEN_STATE;
      break;
    case EXHALE:
      digitalWrite(PIN_RLY_2, RLY_OPEN_STATE);
      rly_exhale_state = RLY_OPEN_STATE;
      break;
    case ERROR:
      digitalWrite(PIN_RLY_3, RLY_OPEN_STATE);
      rly_alarm_state = RLY_OPEN_STATE;
      break;
    case 4:
      digitalWrite(PIN_RLY_4, RLY_OPEN_STATE);
      rly_4_state = RLY_OPEN_STATE;
      break;
  }
}

void rly_close(byte relay) {
  dprintln("Relay " + String(relay) + ": Close");

  switch(relay) {
    case INHALE: // Inhale Valve
      digitalWrite(PIN_INHALE, !RLY_OPEN_STATE);
      rly_inhale_state = !RLY_OPEN_STATE;
      break;
    case EXHALE: // Exhale Valve
      digitalWrite(PIN_RLY_2, !RLY_OPEN_STATE);
      rly_exhale_state = !RLY_OPEN_STATE;
      break;
    case ERROR: // Error
      digitalWrite(PIN_RLY_3, !RLY_OPEN_STATE);
      rly_alarm_state = !RLY_OPEN_STATE;
      break;
    case 4: // Unused at this time
      digitalWrite(PIN_RLY_4, !RLY_OPEN_STATE);
      rly_4_state = !RLY_OPEN_STATE;
      break;
  }
}
