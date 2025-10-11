void FireSolenoid() {
  digitalWrite(SOLPIN, HIGH);  // Открываем соленоид
  delay(Settings.shotTime);    // Задержка в открытом состоянии
  lastShot = millis();
  isLS = 1;
  if (Settings.Load2Select >= 2 && Settings.isLoad2 == 1) {
    isLD2 = 1;
    load2Lastshot = millis();
    digitalWrite(SOLPIN2, HIGH);
  }
  if (Settings.Load3Select >= 2 && Settings.isTracer == 1) {
    isLSTr = 1;
    tracLastShot = millis();
    digitalWrite(TRACER, HIGH);
  }
  WR.shotCount++;             // +1 к счетчику выстрелов
  digitalWrite(SOLPIN, LOW);  // Отключаем соленоид
}

void WaitForTriggerRelease() {
  while (triggerState == true) {
    triggerState = !digitalRead(TRIGPIN) ^ Settings.convTrig;  // Проверяем состояние спуска
    delay(1);
  }
}

void PerformShot() {
  FireSolenoid();
  delay(Settings.shotWait);  // Задержка между выстрелами
}

// Автоматический огонь
void AutoShot() {
  int count = 0;
  while (count < Settings.numOfShotsAuto && triggerState == true) {
    if (Settings.HPAsystemType == SSOL) {
      PerformShot();
    } else if (Settings.HPAsystemType == DSOL) {
      DoubleSolenoidCycle();
    }
    count++;
    triggerState = !digitalRead(TRIGPIN) ^ Settings.convTrig;
    if (triggerState == false) {
      break;
    }
  }
  WaitForTriggerRelease();
}

//двойной выстрел для CQB
void DoubleShot() {
  if (Settings.HPAsystemType == SSOL) {
    PerformShot();
  } else if (Settings.HPAsystemType == DSOL) {
    DoubleSolenoidCycle();
  }
  WaitForTriggerRelease();
  if (Settings.HPAsystemType == SSOL) {
    PerformShot();
  } else if (Settings.HPAsystemType == DSOL) {
    DoubleSolenoidCycle();
  }
}

// одиночка
void SingleShot() {
  if (Settings.HPAsystemType == SSOL) {
    PerformShot();
  } else if (Settings.HPAsystemType == DSOL) {
    DoubleSolenoidCycle();
  }
  if (Settings.Mode == SNIPERMODE) {
    delay(Settings.shotDelay);
  }
  WaitForTriggerRelease();
  Serial.println("SS");
}

// короткие очереди
void ShortShot() {
  int count = 0;
  while (count < Settings.numOfShotsSemi && triggerState == true) {
    if (Settings.HPAsystemType == SSOL) {
      PerformShot();
    } else if (Settings.HPAsystemType == DSOL) {
      DoubleSolenoidCycle();
    }
    count++;
    triggerState = !digitalRead(TRIGPIN) ^ Settings.convTrig;
    if (triggerState == false) {
      break;
    }
  }
  WaitForTriggerRelease();
}

void SwModeSingleDoubleSolenoid() {
  switch (Settings.Mode) {
    case AUTOMODE:  //режим автомата
      if (autoMode == SEMI) ShortShot();
      else AutoShot();
      break;
    case SNIPERMODE:  //режим снайпера
      SingleShot();
      break;
    case CQBMODE:  //режим CQB
      if (Settings.isDoubleShot) DoubleShot();
      else SingleShot();
      break;
    case MACHINEGUNMODE:  //режим пулемета
      AutoShot();
      break;
  }
}

void setSolenoid1(bool state) {
  digitalWrite(SOLPIN, state ? HIGH : LOW);
  solenoid1Active = state;
}

void setSolenoid2(bool state) {
  if (Settings.reverseSolenoid2) {
    // Инвертируем управление для обратного режима
    digitalWrite(SOLPIN2, state ? LOW : HIGH);
    solenoid2Active = state;
  } else {
    digitalWrite(SOLPIN2, state ? HIGH : LOW);
    solenoid2Active = state;
  }
}

void DoubleSolenoidCycle() {
  // 1. Дергаем нозлом
  setSolenoid2(true);
  delay(Settings.del2SolBegin);

  // 2. подаем воздух
  setSolenoid1(true);
  delay(Settings.shotTime);

  lastShot = millis();
  isLS = 1;
  if (Settings.Load2Select >= 2 && Settings.isLoad2 == 1) {
    isLD2 = 1;
    load2Lastshot = millis();
    digitalWrite(SOLPIN2, HIGH);
  }
  if (Settings.Load3Select >= 2 && Settings.isTracer == 1) {
    isLSTr = 1;
    tracLastShot = millis();
    digitalWrite(TRACER, HIGH);
  }
  WR.shotCount++;  // +1 к счетчику выстрелов

  // 3. выключаем воздух
  setSolenoid1(false);
  delay(Settings.del2SolEnd);

  // 4. Задвигаем нозл
  setSolenoid2(false);

  // 5. Задержка между выстрелами
  delay(Settings.shotWait);

  // 6. Установка конечного состояния второго соленоида
  if (Settings.openBolt) {
    setSolenoid2(false);
  } else {
    setSolenoid2(true);
  }
}

void CheckButtons() {
  triggerState = !digitalRead(TRIGPIN) ^ Settings.convTrig;
  autoModeState = !digitalRead(FIREMODESW) ^ Settings.convSel;
  if (Settings.isSWProgSafe) {
    safetyState = !digitalRead(PROGSAFE) ^ Settings.isConvSafe;
  } else {
    safetyState = false;
  }
}

void runSolenoidSinus() {
  // Расчет параметров цикла
  cycleTime = Settings.shotTime + Settings.shotWait;
  phaseShift = round(cycleTime / (float)solenoidCount);  // Округляем сдвиг

  unsigned long cycleStartTime = millis();
  bool buttonReleased = false;
  unsigned long buttonReleaseTime = 0;

  // Массивы для отслеживания состояния каждого соленоида
  bool previousState[3] = { false, false, false };
  bool currentState[3] = { false, false, false };

  // Основной рабочий цикл
  while (true) {
    unsigned long currentTime = millis();
    unsigned long timeSinceStart = currentTime - cycleStartTime;

    // Управление соленоидами с фазовым сдвигом
    for (int i = 0; i < solenoidCount; i++) {
      // Позиция соленоида в цикле с учетом фазового сдвига
      unsigned long solenoidTime = (timeSinceStart + i * phaseShift) % cycleTime;

      // Определяем текущее состояние
      previousState[i] = currentState[i];
      currentState[i] = (solenoidTime < Settings.shotTime);
      if (currentState[i] != previousState[i]) {
        switch (i) {
          case 0:
            digitalWrite(SOLPIN, (solenoidTime < Settings.shotTime) ? HIGH : LOW);
            break;
          case 1:
            digitalWrite(SOLPIN2, (solenoidTime < Settings.shotTime) ? HIGH : LOW);
            break;
          case 2:
            digitalWrite(TRACER, (solenoidTime < Settings.shotTime) ? HIGH : LOW);
            break;
        }
        if (previousState[i] && !currentState[i]) {
          WR.shotCount++;
        }
      }
    }

    // Проверка состояния кнопки
    triggerState = !digitalRead(TRIGPIN) ^ Settings.convTrig;

    if (!triggerState && !buttonReleased) {
      // Кнопка только что отпущена
      buttonReleased = true;
      buttonReleaseTime = currentTime;
    }

    // Условие выхода: кнопка отпущена и прошло полное время цикла
    if (buttonReleased && (currentTime - buttonReleaseTime >= cycleTime)) {
      break;
    }
  }

  // Выключаем все задействованные соленоиды
  digitalWrite(SOLPIN, LOW);
  digitalWrite(SOLPIN2, LOW);
  digitalWrite(TRACER, LOW);
}