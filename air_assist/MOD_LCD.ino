#include "Wire.h"
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27,20,4);

void initMOD_LCD(){
  DPRINTLN(" MOD_LCD: LOADED");
  lcd.init();
  lcd.backlight();
  
  
}
void lcdCLEAR(){
  lcd.clear();
}

void lcdPRINT(String tmpStr, byte tmpCol, byte tmpRow){
  lcd.setCursor(tmpCol, tmpRow);
  lcd.print(tmpStr);
}

void lcdSNR(){
  if(DEVICE_STATE == RUNNING){
    lcdPRINT("PSI:" + String(SNR_MPR_PSI), 11, 3);
  }
}
