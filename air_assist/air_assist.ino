

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

byte VAL_INHALE;
byte VAL_EXHALE;
byte VAL_OXYGEN;



bool RLY_INHALE_STATE;
bool RLY_EXHALE_STATE;
bool RLY_ALARM_STATE;
bool RLY_4_STATE;

int CYCLE_TIME = 3000; //1000 = 1 second
double IE_RATIO = 1.2;
int INHALE_TIME = 1000; 
unsigned long INHALE_CURRENT;
int EXHALE_TIME = 2000;
unsigned long EXHALE_CURRENT;

char CYCLE_STATE;

char DEVICE_STATE;
char MENU_SELECT;
bool MENU_SHOW = true;
int MENU_TIME = 10000;
unsigned long MENU_CURRENT;

void setup() {
  #ifdef DEBUG
    Serial.begin(115200);
    delay(1000);
  #endif
  
  DPRINTLN("Air Assist");
  DPRINTLN("DEBUG: ENABLED");
  DPRINTLN("Loading Modules");
  
  initMOD_LCD();
  initMOD_INPUT();
  initMOD_RELAY();
  initMOD_EEPROM();
  initMOD_MENU();

  DEVICE_STATE = READY;


}

void loop() {
  btnMENU_CHECK();
  snrCHECK();
  menuSELECT();
  
  
  
  switch(DEVICE_STATE){
    case READY:
      lcdPRINT("DEVICE READY", 4, 0);
      break;
    case RUNNING:
      lcdPRINT("DEVICE RUNNING", 3, 0);
      if(!MENU_SHOW){
        lcdSNR();
      }
      cycleRESPIRATION();
      break;
    case STOP:
      lcdPRINT("DEVICE STOPPED", 3, 0);
      rlyCLOSE(INHALE);
      rlyCLOSE(EXHALE);
      break;
    case ERROR:
      lcdPRINT("DEVICE ERROR", 4, 0);
      break;
  }
}

void cycleRESPIRATION(){
  switch(CYCLE_STATE){
    case INHALE:
      if(!MENU_SHOW){
        lcdPRINT("INHALING", 4, 2);
      }
      if(INHALE_CURRENT == 0){
        INHALE_CURRENT = millis();
        rlyOPEN(INHALE);
        rlyCLOSE(EXHALE);
      }
      if(millis() - INHALE_CURRENT > INHALE_TIME){
        CYCLE_STATE = EXHALE;
        EXHALE_CURRENT = millis();
        rlyOPEN(EXHALE);
        rlyCLOSE(INHALE);
      }
      break;
    case EXHALE:
      if(!MENU_SHOW){
        lcdPRINT("ENHALING", 4, 2);
      }
      if(EXHALE_CURRENT == 0){
        EXHALE_CURRENT = millis();
        rlyOPEN(EXHALE);
        rlyCLOSE(INHALE);
      }
      if(millis() - EXHALE_CURRENT > EXHALE_TIME){
        CYCLE_STATE = INHALE;
        INHALE_CURRENT = millis();
        rlyOPEN(INHALE);
        rlyCLOSE(EXHALE);
      }
      break;
  }
}

/*





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



*/
