

void initMOD_MENU(){
  DPRINTLN(" MOD_MENU: LOADED");
  MENU_SELECT = START;
}

void menuSELECT(){

  if(MENU_SHOW){
    lcdPRINT("        MENU        ", 0, 1);
    lcdPRINT("                    ", 0, 2);
    lcdPRINT(LABEL_STR[MENU_SELECT], 0, 3);
    if(btnSELECT_CHECK()){
      switch(MENU_SELECT){
        case START:
          lcdCLEAR();
          DEVICE_STATE = RUNNING;
          CYCLE_STATE = INHALE;
          MENU_SHOW = false;
          break;
        case STOP:
          lcdCLEAR();
          DEVICE_STATE = STOP;
          break;
        case SETTINGS:
          lcdCLEAR();
          break;
      }
    }
    if(btnUP_CHECK()){
      switch(MENU_SELECT){
        case START:
          lcdCLEAR();
          MENU_SELECT = SETTINGS;
          break;
        case STOP:
          lcdCLEAR();
          MENU_SELECT = START;
          break;
        case SETTINGS:
          lcdCLEAR();
          MENU_SELECT = STOP;
          break;
      }
    }
    if(btnDOWN_CHECK()){
      switch(MENU_SELECT){
        case START:
          lcdCLEAR();
          MENU_SELECT = STOP;
          break;
        case STOP:
          lcdCLEAR();
          MENU_SELECT = SELECT;
          break;
        case SETTINGS:
          lcdCLEAR();
          MENU_SELECT = START;
          break;
      }
    }
  }
}
