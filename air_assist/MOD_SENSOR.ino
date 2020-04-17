#include <Wire.h>
#include "Adafruit_MPRLS.h"

#define SNR_MPR_RST_PIN -1
#define SNR_MPR_EOC_PIN -1

Adafruit_MPRLS MPR = Adafruit_MPRLS(SNR_MPR_RST_PIN, SNR_MPR_EOC_PIN);

void init_mod_sensor() {
  dprint(" MOD_SENSOR: ");
  if(!MPR.begin()){
    dprint("ERROR");
    dprintln("Failed to find MPRLS Sensor");
  }else{
    dprintln("LOADED");
  }
}


float pressure_hPa = 0;
char pressure_str[10];  


//Loads sensor data
void snr_check() {
  unsigned long start;
  start = millis();

  snprintf(msg, MSG_LEN, "%8ld readPressure...", start);
  dprintln(msg);

  pressure_hPa = MPR.readPressure();
//  snr_mpr_psi = pressure_hPa / 68.947572932;

  dtostrf(pressure_hPa, 8, 3, pressure_str);
//  dtostrf(snr_mpr_psi,  8, 3, pressure_psi_str);

  dprintln();

  snprintf(msg, MSG_LEN, "%8ld Pressure: %s hPa;  (measurement took %ld ms)",
                          start, pressure_str, millis() - start);
  dprintln(msg);

  snprintf(msg, MSG_LEN, "%8ld Pressure: %d hPa; (measurement took %ld ms)",
                          start, (int) pressure_hPa, millis() - start);

  dprintln(msg);


  lcd_print(pressure_str, 0, 0);                          
}
