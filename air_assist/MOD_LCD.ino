#include "Wire.h"
#include <LiquidCrystal_I2C.h>



LiquidCrystal_I2C lcd(LCD_I2C_ADDRESS,20,4);

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
    lcdPRINT("O:" + String(VAL_OXYGEN), 15, 1);
    lcdPRINT("I:" + String(VAL_INHALE), 15, 2);
    lcdPRINT("H:" + String(VAL_EXHALE), 15, 3);
  }
}
