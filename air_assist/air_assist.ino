

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

#define DEFAULT_BPM                 12
#define DEFAULT_BPM_LOW_LIMIT       10
#define DEFAULT_BPM_HIGH_LIMIT      20
#define DEFAULT_BPM_RATIO           2

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


byte VAL_BPM = DEFAULT_BPM;
byte VAL_BPM_RATIO = DEFAULT_BPM_RATIO;
int VAL_INHALE_TIME;
unsigned long VAL_INHALE_CURRENT;
int VAL_EXHALE_TIME;
unsigned long VAL_EXHALE_CURRENT;



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
  calcBPM();
  
  
  switch(DEVICE_STATE){
    case READY:
      lcdPRINT("DEVICE READY", 4, 0);
      break;
    case RUNNING:
      lcdPRINT("DEVICE RUNNING", 3, 0);
      if(!MENU_SHOW){
        //lcdSNR();
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
        lcdPRINT(LABEL_STR[INHALE], 7, 1);
      }
      if(VAL_INHALE_CURRENT == 0){
        VAL_INHALE_CURRENT = millis();
        rlyOPEN(INHALE);
        rlyCLOSE(EXHALE);
      }
      if(millis() - VAL_INHALE_CURRENT > VAL_INHALE_TIME){
        CYCLE_STATE = EXHALE;
        VAL_EXHALE_CURRENT = millis();
        rlyOPEN(EXHALE);
        rlyCLOSE(INHALE);
      }
      break;
    case EXHALE:
      if(!MENU_SHOW){
        lcdPRINT(LABEL_STR[EXHALE], 7, 1);
      }
      if(VAL_EXHALE_CURRENT == 0){
        VAL_EXHALE_CURRENT = millis();
        rlyOPEN(EXHALE);
        rlyCLOSE(INHALE);
      }
      if(millis() - VAL_EXHALE_CURRENT > VAL_EXHALE_TIME){
        CYCLE_STATE = INHALE;
        VAL_INHALE_CURRENT = millis();
        rlyOPEN(INHALE);
        rlyCLOSE(EXHALE);
      }
      break;
  }
}


void calcBPM(){ //Calculates the inhale and exhale times based on the BPM(Breaths Per Minute) and the ratio
  double tmpCYCLE_TIME = (60000 / VAL_BPM);
  VAL_INHALE_TIME =  tmpCYCLE_TIME / (VAL_BPM_RATIO +1);
  VAL_EXHALE_TIME = VAL_BPM_RATIO * VAL_INHALE_TIME;
}
