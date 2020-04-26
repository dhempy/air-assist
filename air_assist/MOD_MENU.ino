void init_mod_menu() {
  dprintln(" MOD_MENU: LOADED");
  MENU_SELECT = START;
}

char last_menu_shown[20];

void menu_select() {
  if (!menu_show) return;


  if (strcmp(LABEL_STR[MENU_SELECT], last_menu_shown)) {
    lcd_print("        MENU        ", 0, 1);
    lcd_print("                    ", 0, 2);
    lcd_print(LABEL_STR[MENU_SELECT], 0, 3);
    strncpy(last_menu_shown, LABEL_STR[MENU_SELECT], 20);
  }

  if (btn_select_check()) {
    switch(MENU_SELECT) {
      case START:
        lcd_clear();
        device_state = RUNNING;
        cycle_state = INHALE;
        menu_show = false;
        lcd_print("RUN", 17, 0);
        break;
      case STOP:
        lcd_clear();
        device_state = STOP;
        lcd_print("STP", 17, 0);
      break;
      case SETTINGS:
        lcd_clear();
        break;
    }
  }

  if (btn_up_check()) {
    switch(MENU_SELECT) {
      case START:
        lcd_clear();
        MENU_SELECT = SETTINGS;
        break;
      case STOP:
        lcd_clear();
        MENU_SELECT = START;
        break;
      case SETTINGS:
        lcd_clear();
        MENU_SELECT = STOP;
        break;
    }
  }

  if (btn_down_check()) {
    switch(MENU_SELECT) {
      case START:
        lcd_clear();
        MENU_SELECT = STOP;
        break;
      case STOP:
        lcd_clear();
        MENU_SELECT = SELECT;
        break;
      case SETTINGS:
        lcd_clear();
        MENU_SELECT = START;
        break;
    }
  }

  if (device_state == RUNNING) {
    if (millis() - menu_started_at > menu_duration) {
      menu_show = false;
      lcd_clear();
    }
  }
}
