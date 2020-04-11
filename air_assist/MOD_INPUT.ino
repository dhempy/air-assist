#include <Debouncer.h>

#define PIN_BTN_UP      2
#define PIN_BTN_DOWN    3
#define PIN_BTN_MENU   4
#define PIN_BTN_SELECT    5
#define PIN_SNR_INHALE  A0
#define PIN_SNR_EXHALE  A1
#define PIN_SNR_OXYGEN  A2

#define DB_DELAY 50




Debouncer BTN_UP(PIN_BTN_UP, DB_DELAY);
Debouncer BTN_DOWN(PIN_BTN_DOWN, DB_DELAY);
Debouncer BTN_MENU(PIN_BTN_MENU, DB_DELAY);
Debouncer BTN_SELECT(PIN_BTN_SELECT, DB_DELAY);

void initMOD_INPUT(){
  DPRINTLN(" MOD_INPUT: LOADED");
  delay(500);
  //Declares input pins
  pinMode(PIN_BTN_UP, INPUT_PULLUP);
  pinMode(PIN_BTN_DOWN, INPUT_PULLUP);
  pinMode(PIN_BTN_MENU, INPUT_PULLUP);
  pinMode(PIN_BTN_SELECT, INPUT_PULLUP);
  pinMode(PIN_SNR_INHALE, INPUT);
  pinMode(PIN_SNR_EXHALE, INPUT);
  pinMode(PIN_SNR_OXYGEN, INPUT);
}

char btnUP_CHECK(){
  BTN_UP.update();
  
  if(BTN_UP.falling()){
    DPRINTLN("BTN_UP: Pressed");
    return true;
  }else{
    return false;
  }
}

char btnDOWN_CHECK(){
  BTN_DOWN.update();
  
  if(BTN_DOWN.falling()){
    DPRINTLN("BTN_DOWN: Pressed");
    return true;
  }else{
    return false;
  }
}

char btnMENU_CHECK(){
  BTN_MENU.update();
    
  if(BTN_MENU.falling()){
    DPRINTLN("BTN_MENU: Pressed");
    lcdCLEAR();
    MENU_CURRENT = millis();
    MENU_SHOW = true;
  }
}

bool btnSELECT_CHECK(){
  BTN_SELECT.update();
    
  if(BTN_SELECT.falling()){
    DPRINTLN("BTN_SELECT: Pressed");
    return true;
  }else{
    return false;
  }
}

//Loads sensor data
void snrCHECK(){
  VAL_INHALE = map(analogRead(PIN_SNR_INHALE), 0, 1024, 0, 100);
  VAL_EXHALE = map(analogRead(PIN_SNR_EXHALE), 0, 1024, 0, 100);
  VAL_OXYGEN = map(analogRead(PIN_SNR_OXYGEN), 0, 1024, 0, 100);
  
}
