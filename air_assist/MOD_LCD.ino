#include <Wire.h>
#include <LiquidCrystal_I2C.h>

//LiquidCrystal_I2C lcd(0x27,20,4);
LiquidCrystal_I2C lcd(0x3F,20,4);

void init_mod_lcd() {
  dprintln(" MOD_LCD: LOADED");
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
