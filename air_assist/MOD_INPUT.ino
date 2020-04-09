#include <Debouncer.h>

#define PIN_BTN_UP      2
#define PIN_BTN_DOWN    3
#define PIN_BTN_RATIO   4
#define PIN_BTN_RATE    5
#define PIN_SNR_INHALE  A0
#define PIN_SNR_EXHALE  A1
#define PIN_SNR_OXYGEN  A2

#define DB_DELAY 50




Debouncer BTN_UP(PIN_BTN_UP, DB_DELAY);
Debouncer BTN_DOWN(PIN_BTN_DOWN, DB_DELAY);
Debouncer BTN_RATIO(PIN_BTN_RATIO, DB_DELAY);
Debouncer BTN_RATE(PIN_BTN_RATE, DB_DELAY);

void initMOD_INPUT(){
  DPRINTLN(" MOD_INPUT: LOADED");
  delay(500);
  //Declares input pins
  pinMode(PIN_BTN_UP, INPUT_PULLUP);
  pinMode(PIN_BTN_DOWN, INPUT_PULLUP);
  pinMode(PIN_BTN_RATIO, INPUT_PULLUP);
  pinMode(PIN_BTN_RATE, INPUT_PULLUP);
  pinMode(PIN_SNR_INHALE, INPUT);
  pinMode(PIN_SNR_EXHALE, INPUT);
  pinMode(PIN_SNR_OXYGEN, INPUT);
}

void btnCheck(){
  BTN_UP.update();
  BTN_DOWN.update();
  BTN_RATIO.update();
  BTN_RATE.update();

  if(BTN_UP.edge()){
    DPRINT("BTN_UP: ");
    if(BTN_UP.rising()){
      DPRINTLN("Released");
      BTN_UP_STATE = HIGH;
    }
    if(BTN_UP.falling()){
      DPRINTLN("Pressed");
      BTN_UP_STATE = LOW;
    }
  }

  if(BTN_DOWN.edge()){
    DPRINT("BTN_DOWN: ");
    if(BTN_DOWN.rising()){
      DPRINTLN("Released");
      BTN_DOWN_STATE = HIGH;
    }
    if(BTN_DOWN.falling()){
      DPRINTLN("Pressed");
      BTN_DOWN_STATE = LOW;
    }
  }
  
  if(BTN_RATIO.edge()){
    DPRINT("BTN_RATIO: ");
    if(BTN_RATIO.rising()){
      DPRINTLN("Released");
      BTN_RATIO_STATE = HIGH;
    }
    if(BTN_RATIO.falling()){
      DPRINTLN("Pressed");
      BTN_RATIO_STATE = LOW;
    }
  }
  
  if(BTN_RATE.edge()){
    DPRINT("BTN_RATE: ");
    if(BTN_RATE.rising()){
      DPRINTLN("Released");
      BTN_RATE_STATE = HIGH;
    }
    if(BTN_RATE.falling()){
      DPRINTLN("Pressed");
      BTN_RATE_STATE = LOW;
    }
  }
}

//Loads sensor data
void snrCheck(){
  VAL_INHALE = analogRead(PIN_SNR_INHALE);
  VAL_EXHALE = analogRead(PIN_SNR_EXHALE);
  VAL_OXYGEN = analogRead(PIN_SNR_OXYGEN);
}
