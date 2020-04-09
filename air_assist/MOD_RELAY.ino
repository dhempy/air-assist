#define PIN_RLY_1 8  //Inhale Valve
#define PIN_RLY_2 9  //Exhale Valve
#define PIN_RLY_3 10  //Alarm
#define PIN_RLY_4 11


//What signal opens the relay
#define RLY_OPEN_STATE HIGH

bool RLY_INHALE_STATE;
bool RLY_EXHALE_STATE;
bool RLY_ALARM_STATE;
bool RLY_4_STATE;



void initMOD_RELAY(){
  delay(500);
  DPRINTLN(" MOD_RELAY: LOADED");
  //Declares relay pins
  pinMode(PIN_RLY_1, OUTPUT);
  pinMode(PIN_RLY_2, OUTPUT);
  pinMode(PIN_RLY_3, OUTPUT);
  pinMode(PIN_RLY_4, OUTPUT);
  //closes all relays for startup.
  digitalWrite(PIN_RLY_1, !RLY_OPEN_STATE);
  digitalWrite(PIN_RLY_2, !RLY_OPEN_STATE);
  digitalWrite(PIN_RLY_3, !RLY_OPEN_STATE);
  digitalWrite(PIN_RLY_4, !RLY_OPEN_STATE);
}




void rlyOPEN(int tmpNum){
  DPRINTLN("Relay " + String(tmpNum) + ": Open");
  switch(tmpNum){
    case 1: // Inhale Valve
      digitalWrite(PIN_RLY_1, RLY_OPEN_STATE);
      RLY_INHALE_STATE = RLY_OPEN_STATE;
      break;
    case 2: // Exhale Valve
      digitalWrite(PIN_RLY_2, RLY_OPEN_STATE);
      RLY_EXHALE_STATE = RLY_OPEN_STATE;
      break;
    case 3: // Alarm
      digitalWrite(PIN_RLY_3, RLY_OPEN_STATE);
      RLY_ALARM_STATE = RLY_OPEN_STATE;
      break;
    case 4: // Unused at this time.
      digitalWrite(PIN_RLY_4, RLY_OPEN_STATE);
      RLY_4_STATE = RLY_OPEN_STATE;
      break;
  }
}

void rlyCLOSE(int tmpNum){
  DPRINTLN("Relay " + String(tmpNum) + ": Close");
  switch(tmpNum){
    case 1: // Inhale Valve
      digitalWrite(PIN_RLY_1, !RLY_OPEN_STATE);
      RLY_INHALE_STATE = !RLY_OPEN_STATE;
      break;
    case 2: // Exhale Valve
      digitalWrite(PIN_RLY_2, !RLY_OPEN_STATE);
      RLY_EXHALE_STATE = !RLY_OPEN_STATE;
      break;
    case 3: // Alarm
      digitalWrite(PIN_RLY_3, !RLY_OPEN_STATE);
      RLY_ALARM_STATE = !RLY_OPEN_STATE;
      break;
    case 4: // Unused at this time
      digitalWrite(PIN_RLY_4, !RLY_OPEN_STATE);
      RLY_4_STATE = !RLY_OPEN_STATE;
      break;
  }
}
