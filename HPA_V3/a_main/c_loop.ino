void loop() {
  ui.tick();

  mem1.tick();
  mem2.tick();
  mem3.tick();

  if (WiFi.status() != WL_CONNECTED && millis() - lastShot >= Settings.timeLastShot * 60000 && isLS == 1) {  //сохранение настрела если не включен WiFi
    WR.writeCount++;
    mem2.update();
    isLS = 0;
  }

  if (Settings.isLoad2 == 1 && isLD2 == 1) {
    switch (Settings.Load2Select) {
      case DELAFTERSHOTSEC:
        if (load2Lastshot + (Settings.Load2Time * 1000) <= millis()) {  //ВЫключение трассерки после заданного времени после выстрела
          digitalWrite(SOLPIN2, LOW);
          isLD2 = 0;
        }
        break;
      case DELAFTERSHOTMIN:
        if (load2Lastshot + (Settings.Load2Time * 1000 * 1000) <= millis()) {  //ВЫключение трассерки после заданного времени после выстрела
          digitalWrite(SOLPIN2, LOW);
          isLD2 = 0;
        }
        break;
      case TRASHOP:
        if (load2Lastshot + (Settings.Load2Time * 1000) <= millis()) {  //ВЫключение трассерки после заданного времени после выстрела
          analogWrite(SOLPIN2, map(Settings.load2PWMDej, 0, 100, 0, 255));
          isLD2 = 0;
        }
        break;
      case BUBEN:
        if (load2Lastshot + (Settings.Load2Time * 1000) <= millis()) {  //ВЫключение трассерки после заданного времени после выстрела
          digitalWrite(SOLPIN2, LOW);
          isLD2 = 0;
        }
        break;
    }
  }

  if (Settings.isTracer == 1 && isLSTr == 1) {
    switch (Settings.Load3Select) {
      case DELAFTERSHOTSEC:
        if (tracLastShot + (Settings.tracTime * 1000) <= millis()) {  //ВЫключение трассерки после заданного времени после выстрела
          digitalWrite(TRACER, LOW);
          isLSTr = 0;
        }
        break;
      case DELAFTERSHOTMIN:
        if (tracLastShot + (Settings.tracTime * 1000 * 1000) <= millis()) {  //ВЫключение трассерки после заданного времени после выстрела
          digitalWrite(TRACER, LOW);
          isLSTr = 0;
        }
        break;
      case TRASHOP:
        if (tracLastShot + (Settings.tracTime * 1000) <= millis()) {  //ВЫключение трассерки после заданного времени после выстрела
          analogWrite(TRACER, map(Settings.load3PWMDej, 0, 100, 0, 255));
          isLSTr = 0;
        }
        break;
      case BUBEN:
        if (tracLastShot + (Settings.tracTime * 1000) <= millis()) {  //ВЫключение трассерки после заданного времени после выстрела
          digitalWrite(TRACER, LOW);
          isLSTr = 0;
        }
        break;
    }
  }

  if (millis() - voltTime > 2000) {  //проверка напряжения
    Volt();
  }

  if (millis() - lastShot >= deepSleepTime && Settings.isDeepSleep || isBatSafeFlag == true) {
    // Переход в глубокий сон
    Serial.println("сон");  //засыпает и не просыпается зараза
    enterDeepSleep();
  }

  if (millis() - lastShot >= LIGHTSLEEPTIME && isSetupMode == false && Settings.isLightSleep == true) {
    lastShot = millis();
    light_sleep();
    delay(1);
    WiFi.mode(WIFI_OFF);
    WiFi.forceSleepBegin();
  }

  CheckButtons();

  autoMode = autoModeState ? 2 : 1;

  if (safetyState == true || isWiFiFlag == false) {
    // Если предохранитель нажат, соленоид отключен
    digitalWrite(SOLPIN, LOW);
    return;
  }

  if (triggerState == true) {  //проверка нажатия на спуск

    switch (Settings.PCB) {
      //
      case CUSTOMGUN:
        switch (Settings.HPAsystemType) {
          case SSOL:
            SwModeSingleDoubleSolenoid();
            break;
          case DSOL:
            SwModeSingleDoubleSolenoid();
            break;
          case DOUBLEBURREL:
            break;
          case NONSINGLESYSTEM:
            runSolenoidSinus();
            break;
        }
        break;
      case M4:
        switch (Settings.HPAsystemType) {
          case SSOL:
            SwModeSingleDoubleSolenoid();
            break;
          case DSOL:
            SwModeSingleDoubleSolenoid();
            break;
          case DOUBLEBURREL:
            break;
          case NONSINGLESYSTEM:
            runSolenoidSinus();
            break;
        }
        break;
      case AK:
        switch (Settings.HPAsystemType) {
          case SSOL:
            SwModeSingleDoubleSolenoid();
            break;
          case DSOL:
            SwModeSingleDoubleSolenoid();
            break;
          case DOUBLEBURREL:
            break;
          case NONSINGLESYSTEM:
            runSolenoidSinus();
            break;
        }
        break;
      case M4A:
        switch (Settings.HPAsystemType) {
          case SSOL:
            SwModeSingleDoubleSolenoid();
            break;
          case DSOL:
            SwModeSingleDoubleSolenoid();
            break;
          case DOUBLEBURREL:
            break;
          case NONSINGLESYSTEM:
            runSolenoidSinus();
            break;
        }
        break;
      case KAC:
        switch (Settings.HPAsystemType) {
          case SSOL:
            AutoShot();
            break;
          case DSOL:
            AutoShot();
            break;
          case DOUBLEBURREL:
            break;
          case NONSINGLESYSTEM:
            runSolenoidSinus();
            break;
        }
        break;
      default:
        Serial.println("SELECT HPA");
        break;
    }
  }
  if (Serial.available()) {
    char c = Serial.read();

    // Команда 'r' - ручной сброс системы
    if (c == 'r' || c == 'R') {
      mem1.reset();  // Сброс настроек
      mem3.reset();
      Serial.println("Сброс");
      ESP.restart();
    }
  }
}