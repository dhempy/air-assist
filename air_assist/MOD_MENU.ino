void init_mod_menu() {
  dprintln(" MOD_MENU: LOADED");
  menu_selected = START;
}

void menu_select() {
  if (!menu_show) { return; }

  lcd_print("        MENU        ", 0, 1);
  lcd_print("                    ", 0, 2);
  lcd_print(LABEL_STR[MENU_SELECT], 0, 3);

  if (btn_select_check()) {
    switch(menu_selected) {
      case START:
        lcd_clear();
        device_state = RUNNING;
        cycle_state = INHALE;
        menu_show = false;
        break;
      case STOP:
        lcd_clear();
        device_state = STOP;
        break;
      case SETTINGS:
        lcd_clear();
        break;
    }
  }

  if (btn_up_check()) {
    switch(menu_selected) {
      case START:
        lcd_clear();
        menu_selected = SETTINGS;
        break;
      case STOP:
        lcd_clear();
        menu_selected = START;
        break;
      case SETTINGS:
        lcd_clear();
        menu_selected = STOP;
        break;
    }
  }

  if (btn_down_check()) {
    switch(menu_selected) {
      case START:
        lcd_clear();
        menu_selected = STOP;
        break;
      case STOP:
        lcd_clear();
        menu_selected = SELECT;
        break;
      case SETTINGS:
        lcd_clear();
        menu_selected = START;
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
