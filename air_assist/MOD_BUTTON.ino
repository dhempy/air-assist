#include <Debouncer.h>
#include "pinout.h"

#define DEBOUNCE_DELAY 50

Debouncer btn_up(PIN_BTN_UP, DEBOUNCE_DELAY);
Debouncer btn_down(PIN_BTN_DOWN, DEBOUNCE_DELAY);
Debouncer btn_menu(PIN_BTN_MENU, DEBOUNCE_DELAY);
Debouncer btn_select(PIN_BTN_SELECT, DEBOUNCE_DELAY);

void init_mod_input() {
  dprintln(" MOD_INPUT: LOADED");
  //Declares input pins
  pinMode(PIN_BTN_UP, INPUT_PULLUP);
  pinMode(PIN_BTN_DOWN, INPUT_PULLUP);
  pinMode(PIN_BTN_MENU, INPUT_PULLUP);
  pinMode(PIN_BTN_SELECT, INPUT_PULLUP);
}

bool btn_up_check() {
  btn_up.update();

  if (btn_up.falling()) {
    dprintln("BTN_UP: Pressed");
    return true;
  } else {
    return false;
  }
}

bool btn_down_check() {
  btn_down.update();

  if (btn_down.falling()) {
    dprintln("BTN_DOWN: Pressed");
    return true;
  } else {
    return false;
  }
}

void btn_menu_check() {
  btn_menu.update();

  if (btn_menu.falling()) {
    dprintln("BTN_MENU: Pressed");
    lcd_clear();
    menu_started_at = millis();
    menu_show = true;
  }
}

bool btn_select_check() {
  btn_select.update();

  if (btn_select.falling()) {
    dprintln("BTN_SELECT: Pressed");
    return true;
  } else {
    return false;
  }
}
