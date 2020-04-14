#include <Wire.h>
#include "Adafruit_MPRLS.h"

#define SNR_MPR_RST_PIN 7
#define SNR_MPR_EOC_PIN 6

Adafruit_MPRLS MPR = Adafruit_MPRLS(SNR_MPR_RST_PIN, SNR_MPR_EOC_PIN);

void initMOD_SENSOR(){
  DPRINT(" MOD_SENSOR: ");
  if(!MPR.begin()){
    DPRINT("ERROR");
    DPRINTLN("Failed to find MPRLS Sensor");
  }else{
    DPRINT("LOADED");
  }
}
//Loads sensor data
void snrCHECK(){
  SNR_MPR_PSI = MPR.readPressure() / 68.947572932;
}
