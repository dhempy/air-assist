// #include <jm_Scheduler.h>
#include <jm_Wire.h>
// #include <Wire.h>
#include <LiquidCrystal_I2C.h>

//LiquidCrystal_I2C lcd(0x27,20,4);
LiquidCrystal_I2C lcd(0x3F,20,4);
// jm_LiquidCrystal_I2C lcd;

void init_mod_lcd() {
  dprintln(" MOD_LCD: LOADED");


  // // jm_wire init:
  // lcd.begin();
  // while (lcd._i2cio.yield_request()) jm_Scheduler::yield();

  lcd.init();
  lcd.backlight();
}

void lcd_clear() {
  lcd.clear();
}

void lcd_print(String str, byte col, byte row) {
  lcd.setCursor(col, row);
  lcd.print(str);
}

void lcd_show_sensors() {
  return;  // Not yet

  if (device_state == RUNNING) {
    lcd_print("PSI:" + String(snr_mpr_psi), 11, 3);
  }
}
