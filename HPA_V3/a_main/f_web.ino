void build() {
  GP.BUILD_BEGIN();
  GP.THEME(GP_DARK);
  //GP.THEME(GP_LIGHT);

  GP.UI_MENU("Меню", GP_RED);  // начать меню
  GP.UI_LINK("/", "Домашняя страница");
  GP.UI_LINK("/hpasettings", "Настройки конфигурации");
  GP.UI_LINK("/settings", "Дополнительные настройки");
  GP.UI_LINK("/info", "Информация");
  GP.UI_LINK("/ota_update", "Обновление");

  GP.UI_BODY();

  GP.UPDATE("rof,cof,vol,pr,vo,vc,vd,vi,volcor,btn,cow,sf,sa,na,OV,R1,R2,TrSl,DSSl,DSTr,led1,led2,led3,pcb,hpa,OpBo,InNoz");
  GP.RELOAD_CLICK("SOPS,Ld2Sw,TrSw,DSTr,OpBo,InNoz");

  GP.TITLE("ZTV ВВД v.3.1");
  GP.HR();

  if (ui.uri("/")) {
    if (Settings.HPAsystemType < NONSINGLESYSTEM) {
      M_BOX(GP.LABEL("Режим"); GP.SELECT("vi", "Автомат,Снайпер,CQB режим,Режим пулемета", Settings.Mode, false, false, true); GP.BREAK(););
    }

    M_BLOCK_THIN_TAB(
      "Скорострельность",
      M_BOX(GP.SWITCH("WFTr", isWiFiFlag); GP.LABEL("включить спуск"););
      if (Settings.HPAsystemType == DSOL) {
        GP.LABEL("Задержка нозла в начале");
        M_BOX(GP.LABEL("мс"); GP.SLIDER("nozBeg", Settings.del2SolBegin, 0, 40););
        GP.BREAK();
      }

      switch (Settings.HPAsystemType) {
        case DSOL:
          GP.LABEL("Время подачи воздуха");
          break;
        default:
          GP.LABEL("Время открытия клапана");
          break;
      }

      M_BOX(GP.LABEL("мс"); GP.BUTTON_MINI("bkwO", "◄"); GP.SLIDER("vo", Settings.shotTime, 5, 100); GP.BUTTON_MINI("frwO", "►"););
      GP.BREAK();
      switch (Settings.HPAsystemType) {
        case DSOL:
          GP.LABEL("Время отката нозла");
          break;
        default:
          GP.LABEL("Время закрытия клапана");
          break;
      }

      M_BOX(GP.LABEL("мс"); GP.BUTTON_MINI("bkwC", "◄"); GP.SLIDER("vc", Settings.shotWait, 5, 100); GP.BUTTON_MINI("frwC", "►"););
      GP.BREAK();

      if (Settings.HPAsystemType == DSOL) {
        GP.LABEL("Задержка нозла в конце");
        M_BOX(GP.LABEL("мс"); GP.SLIDER("nozEnd", Settings.del2SolEnd, 0, 25););
        GP.BREAK();
      }

      if (Settings.Mode == SNIPERMODE) {
        GP.LABEL("Время между выстрелами");
        M_BOX(GP.LABEL("мс"); GP.BUTTON_MINI("bkwD", "◄"); GP.SLIDER("vd", Settings.shotDelay, 0, 1000); GP.BUTTON_MINI("frwD", "►"););
        GP.BREAK();
      }

      GP.LABEL("Выстрелов в минуту: ");
      GP.LABEL_BLOCK("NAN", "rof"); GP.BREAK();
      M_BOX(GP.SWITCH("CvTr", Settings.convTrig); GP.LED("led3"); GP.LABEL("инверт. спуск"););

      if (Settings.PCB == M4 || Settings.PCB == M4A || Settings.PCB == AK) {
        M_BOX(GP.SWITCH("CvFM", Settings.convSel); GP.LED("led2"); GP.LABEL("инверт. переключ. огня"););
      }

      if (Settings.PCB == M4 || Settings.PCB == AK) {
        if (Settings.isSWProgSafe) {
          M_BOX(GP.SWITCH("CvSf", Settings.isConvSafe); GP.LED("led1"); GP.LABEL("инверт. предохранитель"););
        }
        M_BOX(GP.SWITCH("SOPS", Settings.isSWProgSafe); GP.LABEL("вкл. электр. предохр."););
      }

      if (Settings.Mode == CQBMODE) {
        M_BOX(GP.SWITCH("TWF", Settings.isDoubleShot); GP.LABEL("двойной спуск для CQB"););
        GP.BREAK();
      });

    M_BLOCK_THIN_TAB(
      "Ограничения",
      switch (Settings.Mode) {
        case AUTOMODE:
          GP.LABEL("Кол-во выстрелов на одиночке");
          M_BOX(GP.LABEL(""); GP.BUTTON_MINI("bkwS", "◄"); GP.SLIDER("sf", Settings.numOfShotsSemi, 1, 10); GP.BUTTON_MINI("frwS", "►"););
          GP.BREAK();
          GP.LABEL("Кол-во выстрелов при авто");
          M_BOX(GP.LABEL(""); GP.BUTTON_MINI("bkwA", "◄"); GP.SLIDER("sa", Settings.numOfShotsAuto, 0, 100); GP.BUTTON_MINI("frwA", "►"););
          GP.BREAK();
          break;
        case MACHINEGUNMODE:
          GP.LABEL("Кол-во выстрелов при авто");
          M_BOX(GP.LABEL(""); GP.BUTTON_MINI("bkwA", "◄"); GP.SLIDER("sa", Settings.numOfShotsAuto, 0, 100); GP.BUTTON_MINI("frwA", "►"););
          GP.BREAK();
          break;
      }

      GP.LABEL("Минут до сохранения настрела");
      M_BOX(GP.LABEL(""); GP.BUTTON_MINI("bkwT", "◄"); GP.SLIDER("na", Settings.timeLastShot, 1, 10); GP.BUTTON_MINI("frwT", "►");); GP.BREAK();

      GP.LABEL("Настрел: ");
      GP.LABEL("NAN", "cof"); GP.BREAK();
      GP.LABEL("Кол-во записей: ");
      GP.LABEL("NAN", "cow"); GP.BREAK(););

    if (Settings.Load2Select > 1) {
      M_BLOCK_THIN_TAB(
        "2 нагрузка",
        GP.SWITCH("Ld2Sw", Settings.isLoad2);
        GP.BREAK();
        switch (Settings.Load2Select) {
          case DELAFTERSHOTSEC:
            GP.LABEL("Сек. до отключения");
            break;
          case DELAFTERSHOTMIN:
            GP.LABEL("Мин. до отключения");
            break;
        } M_BOX(GP.LABEL(""); GP.SLIDER("LD2Sl", Settings.Load2Time, 1, 5, 1, 0, GP_BLUE, !Settings.isLoad2););
        GP.BREAK(););
    }

    if (Settings.Load3Select > 1) {
      M_BLOCK_THIN_TAB(
        "3 нагрузка",
        GP.SWITCH("TrSw", Settings.isTracer);
        GP.BREAK();
        switch (Settings.Load3Select) {
          case DELAFTERSHOTSEC:
            GP.LABEL("Сек. до отключения");
            break;
          case DELAFTERSHOTMIN:
            GP.LABEL("Мин. до отключения");
            break;
        }

        M_BOX(GP.LABEL(""); GP.SLIDER("TrSl", Settings.tracTime, 1, 5, 1, 0, GP_BLUE, !Settings.isTracer););
        GP.BREAK(););
    }

    M_BLOCK_THIN_TAB(
      "Автоотключение при бездействии",
      GP.SWITCH("DSTr", Settings.isDeepSleep);
      GP.BREAK();
      GP.LABEL("Минут до отключения");
      M_BOX(GP.LABEL(""); GP.SLIDER("DSSl", Settings.deepSleepMin, 1, 300, 1, 0, GP_YELLOW, !Settings.isDeepSleep););
      GP.BREAK(););
    // GP.LABEL("коррект. напр.");
    // GP.SPINNER("volcor", Settings.voltCorr);
    // GP.BREAK();
    GP.LABEL("Напряжение: ");
    GP.LABEL("", "vol");
    GP.LABEL("В");
    GP.BREAK();
    GP.LABEL("Процент заряда: ");
    GP.LABEL("NAN", "pr");
    GP.LABEL("%");
    GP.BREAK();
    GP.BREAK();
    GP.BUTTON("btn", "Сохранить настройки");

    M_SPOILER(
      "Сброс",
      GP.BUTTON_MINI("btnS", "Сброс настроек"););

  } else if (ui.uri("/hpasettings")) {
    M_BOX(GP.LABEL("Плата управл."); GP.SELECT("pcb", "Кастомная,M4,AK,M4 ME Arcturus,KAC Stoner LMG", Settings.PCB, false, false, true); GP.BREAK(););
    M_BOX(GP.LABEL("ВВД система"); GP.SELECT("hpa", "односоленоидная,двухсоленоидная,двухствольная,многоствольный пулемет", Settings.HPAsystemType, false, false, true); GP.BREAK(););
    if (Settings.HPAsystemType == NONSINGLESYSTEM) {
      M_BOX(GP.LABEL("Количество стволов"); GP.SELECT("nbur", "2 ствола,3ствола", Settings.TwoThreeBurrelMG, false, false, true); GP.BREAK(););
    }
    if (Settings.HPAsystemType == DSOL) {
      M_BOX(GP.SWITCH("OpBo", Settings.openBolt); GP.LABEL("Норм.откр.нозл"););
      M_BOX(GP.SWITCH("InNoz", Settings.reverseSolenoid2); GP.LABEL("инверт. нозл"););
      if(Settings.openBolt == true && Settings.reverseSolenoid2 == false || Settings.openBolt == false && Settings.reverseSolenoid2 == true){
        Settings.otkat = false;
      }else{
      M_BOX(GP.SWITCH("Otk", Settings.otkat); GP.LABEL("откат в начале"););
      }
    }
    switch (Settings.HPAsystemType) {
      case SSOL:
        TwoLoads();
        break;
      case DSOL:
        OneLoad();
        break;
      case DOUBLEBURREL:
        OneLoad();
        break;
      case NONSINGLESYSTEM:
        if (Settings.TwoThreeBurrelMG == 0) {
          OneLoad();
        }
        break;
    }

    GP.BUTTON("btn", "Сохранить настройки");


  } else if (ui.uri("/settings")) {
    M_BOX(GP.LABEL("Аккумулятор"); GP.SELECT("bt", "2S-8.4В,3S-12.6В", Settings.batType); GP.BREAK(););
    M_BOX(GP.SWITCH("BS", Settings.isBatSafe); GP.LABEL("отключение при разряде"););
    M_BLOCK_THIN_TAB(
      "Настройка делителя",
      M_BOX(GP.LABEL("R1"); GP.NUMBER_F("R1", "", Settings.divR1););
      M_BOX(GP.LABEL("R2"); GP.NUMBER_F("R2", "", Settings.divR2););
      M_BOX(GP.LABEL("Опорное"); GP.NUMBER_F("OV", "", Settings.voltCorr););
      GP.BUTTON("btn", "Сохранить настройки");

      GP.LABEL("Напряжение: ");
      GP.LABEL("", "vol");
      GP.LABEL("В");
      GP.BREAK();
      GP.LABEL("Процент заряда: ");
      GP.LABEL("NAN", "pr");
      GP.LABEL("%");
      GP.BREAK(););


    GP.TEXT("ssd", "text", WF.WF_SSID);
    GP.BREAK();
    GP.TEXT("pass", "pass", WF.WF_PASS);
    GP.BREAK();
    GP.BUTTON("WFbtn", "Сохранить Wi-Fi");
    GP.BREAK();
    GP.BUTTON_MINI("WFRbtn", "Сброс Wi-fi");



  } else if (ui.uri("/info")) {
    GP.LABEL("Прошивка v3.1");
    GP.BREAK();
    GP.BREAK();
    GP.LABEL("Магазин");
    GP.BREAK();
    GP.UI_LINK("https://vk.com/ztv_airsoft", "VK магазин");
    GP.BREAK();
    GP.BREAK();
    GP.LABEL("Дополнительные материалы");
    GP.BREAK();
    GP.UI_LINK("https://boosty.to/ztv", "Boosty");
    GP.BREAK();
    GP.BREAK();
    GP.LABEL("Последние прошивки");
    GP.BREAK();
    GP.UI_LINK("https://github.com/ZTVairsoft/HPA_V3", "GitHub");
    GP.BREAK();
    GP.BREAK();
    GP.LABEL("Последние релизы");
    GP.BREAK();
    GP.UI_LINK("https://github.com/ZTVairsoft/HPA_V3/releases", "Релизы BIN");

  } else if (ui.uri("/ota_update")) {
  }

  GP.UI_END();

  GP.BUILD_END();
}

void OneLoad() {
  GP.LABEL("Работа нагрузки");
  GP.BREAK();
  M_BOX(GP.LABEL("3 нагрузка"); GP.SELECT("load3", "выключена,включена,выкл.сек.,выкл.мин.", Settings.Load3Select); GP.BREAK(););
}

void TwoLoads() {
  GP.LABEL("Работа нагрузок");
  GP.BREAK();
  M_BOX(GP.LABEL("2 нагрузка"); GP.SELECT("load2", "выключена,включена,выкл.сек.,выкл.мин.", Settings.Load2Select); GP.BREAK(););
  M_BOX(GP.LABEL("3 нагрузка"); GP.SELECT("load3", "выключена,включена,выкл.сек.,выкл.мин.", Settings.Load3Select); GP.BREAK(););
}

void TypeLoads() {
}

void action() {
  int ROF;
  switch (Settings.HPAsystemType) {
    case SSOL:
      if (Settings.Mode == 1) {
        ROF = (1000 / (Settings.shotTime + Settings.shotWait + Settings.shotDelay)) * 60;
      } else {
        ROF = (1000 / (Settings.shotTime + Settings.shotWait)) * 60;
      }
      break;
    case DSOL:
      if (Settings.Mode == 1) {
        ROF = (1000 / (Settings.shotTime + Settings.shotWait + Settings.shotDelay + Settings.del2SolBegin + Settings.del2SolEnd)) * 60;
      } else {
        ROF = (1000 / (Settings.shotTime + Settings.shotWait + Settings.del2SolBegin + Settings.del2SolEnd)) * 60;
      }
      break;
    case DOUBLEBURREL:
      break;
    case NONSINGLESYSTEM:
      int result = (Settings.TwoThreeBurrelMG == 0) ? 2 : 3;
      ROF = (1000 / (Settings.shotTime + Settings.shotWait)) * 60 * result;
      break;
  }
  // if (Settings.Mode == 1) {
  //   ROF = (1000 / (Settings.shotTime + Settings.shotWait + Settings.shotDelay)) * 60;
  // } else if (Settings.Mode == 3 && Settings.HPAsystemType == NONSINGLESYSTEM){
  //   ROF = (1000 / (Settings.shotTime + Settings.shotWait)) * 60 * int result = (Settings.TwoThreeBurrelMG == 0) ? 2 : 3;;
  // } else {
  //   ROF = (1000 / (Settings.shotTime + Settings.shotWait)) * 60;
  // }

  if (ui.click()) {

    if (ui.clickInt("vo", Settings.shotTime)) {
    }

    if (ui.clickInt("nozBeg", Settings.del2SolBegin)) {
    }

    if (ui.clickInt("nozEnd", Settings.del2SolEnd)) {
    }

    if (ui.clickInt("vc", Settings.shotWait)) {
    }

    if (ui.clickInt("vd", Settings.shotDelay)) {
    }

    if (ui.clickInt("sf", Settings.numOfShotsSemi)) {
    }

    if (ui.clickInt("sa", Settings.numOfShotsAuto)) {
    }

    if (ui.clickInt("na", Settings.timeLastShot)) {
    }

    if (ui.clickInt("vi", Settings.Mode)) {
    }

    if (ui.clickInt("pcb", Settings.PCB)) {
    }
    if (ui.clickInt("hpa", Settings.HPAsystemType)) {
      switch (Settings.HPAsystemType) {
        case DSOL:
          Settings.Load2Select = NOPE;
          if (isLD2) digitalWrite(SOLPIN2, LOW);
          break;
        case DOUBLEBURREL:
          Settings.Load2Select = NOPE;
          break;
        case NONSINGLESYSTEM:
          if (Settings.TwoThreeBurrelMG == TWOBURREL) {
            Settings.Load3Select = NOPE;
          } else if (Settings.TwoThreeBurrelMG == THREEBURREL) {
            Settings.Load2Select = NOPE;
            Settings.Load3Select = NOPE;
          }
          Settings.Mode = MACHINEGUNMODE;
          solenoidCount = (Settings.TwoThreeBurrelMG == 0) ? 2 : 3;
          break;
      }
    }
    if (ui.clickInt("nbur", Settings.TwoThreeBurrelMG)) {
      solenoidCount = (Settings.TwoThreeBurrelMG == 0) ? 2 : 3;
    }
    if (ui.clickBool("OpBo", Settings.openBolt)) {
    }
    if (ui.clickBool("InNoz", Settings.reverseSolenoid2)) {
    }
    if (ui.clickBool("Otk", Settings.otkat)) {
    }
    if (ui.clickInt("load2", Settings.Load2Select)) {
      if (Settings.Load2Select == JUSTON) {
        digitalWrite(SOLPIN2, HIGH);
        isLD2 = 1;
      } else if (Settings.Load2Select > JUSTON || Settings.Load2Select == NOPE) {
        digitalWrite(SOLPIN2, LOW);
        isLD2 = 0;
      }
    }
    if (ui.clickInt("load3", Settings.Load3Select)) {
      if (Settings.Load3Select == JUSTON) {
        digitalWrite(TRACER, HIGH);
        isLSTr = 1;
      } else if (Settings.Load3Select > JUSTON || Settings.Load3Select == NOPE) {
        digitalWrite(TRACER, LOW);
        isLSTr = 0;
      }
    }

    if (ui.clickInt("bt", Settings.batType)) {
    }

    if (ui.clickBool("TrSw", Settings.isTracer)) {
      if (!Settings.isTracer) digitalWrite(TRACER, LOW);
    }
    if (ui.clickBool("Ld2Sw", Settings.isLoad2)) {
      if (!Settings.isLoad2) digitalWrite(SOLPIN2, LOW);
    }

    if (ui.clickBool("DSTr", Settings.isDeepSleep)) {
    }

    if (ui.clickBool("CvTr", Settings.convTrig)) {
    }

    if (ui.clickBool("CvFM", Settings.convSel)) {
    }

    if (ui.clickBool("CvSf", Settings.isConvSafe)) {
    }

    if (ui.clickBool("SOPS", Settings.isSWProgSafe)) {
    }

    if (ui.clickBool("WFTr", isWiFiFlag)) {
    }

    if (ui.clickBool("TWF", Settings.isDoubleShot)) {
    }

    if (ui.clickBool("BS", Settings.isBatSafe)) {
    }

    if (ui.clickInt("TrSl", Settings.tracTime)) {
    }

    if (ui.clickInt("LD2Sl", Settings.Load2Time)) {
    }

    if (ui.clickInt("DSSl", Settings.deepSleepMin)) {
    }

    if (ui.clickInt("volcor", Settings.voltCorr)) {
    }

    if (ui.click("btn")) {
      WR.writeCount++;
      mem1.updateNow();  // обновить сейчас
      mem2.updateNow();
      Serial.println("Изменения сохранены");
    }
    if (ui.click("WFbtn")) {
      WR.writeCount++;
      Serial.println(WF.WF_SSID);
      Serial.println(WF.WF_PASS);
      Serial.println(WF.apply);
      WF.apply = 1;

      mem3.updateNow();  // обновить сейчас
      Serial.println("Изменения сохранены");

      ESP.restart();
    }

    if (ui.click("WFRbtn")) {
      WR.writeCount++;
      mem2.updateNow();  // обновить сейчас
      Serial.println("Сброс Wi-Fi");
      mem3.reset();  // Сброс настроек
      ESP.restart();
    }

    if (ui.click("btnS")) {
      mem1.reset();  // Сброс настроек
      Serial.println("Сброс");
      ESP.restart();
    }

    if (ui.click("bkwO")) Settings.shotTime--;
    if (ui.click("bkwC")) Settings.shotWait--;
    if (ui.click("bkwS")) Settings.numOfShotsSemi--;
    if (ui.click("bkwA")) Settings.numOfShotsAuto--;
    if (ui.click("bkwT")) Settings.timeLastShot--;
    if (ui.click("bkwD")) Settings.shotDelay--;

    if (ui.click("frwO")) Settings.shotTime++;
    if (ui.click("frwC")) Settings.shotWait++;
    if (ui.click("frwS")) Settings.numOfShotsSemi++;
    if (ui.click("frwA")) Settings.numOfShotsAuto++;
    if (ui.click("frwT")) Settings.timeLastShot++;
    if (ui.click("frwD")) Settings.shotDelay++;

    if (ui.clickFloat("R1", Settings.divR1)) {}
    if (ui.clickFloat("R2", Settings.divR2)) {}
    if (ui.clickFloat("OV", Settings.voltCorr)) {}

    if (ui.clickString("ssd", WF.WF_SSID)) {}
    if (ui.clickString("pass", WF.WF_PASS)) {}
  }
  if (ui.update()) {

    if (ui.update("rof")) ui.answer(ROF);

    if (ui.update("cof")) ui.answer(WR.shotCount);

    if (ui.update("cow")) ui.answer(WR.writeCount);

    if (ui.update("vol")) ui.answer(resVoltValue, 2);

    if (ui.update("pr")) ui.answer(procVol);

    if (ui.update("vo")) ui.answer(Settings.shotTime);

    if (ui.update("vc")) ui.answer(Settings.shotWait);

    if (ui.update("sf")) ui.answer(Settings.numOfShotsSemi);

    if (ui.update("sa")) ui.answer(Settings.numOfShotsAuto);

    if (ui.update("na")) ui.answer(Settings.timeLastShot);

    if (ui.update("led1")) ui.answer(safetyState);

    if (ui.update("led2")) ui.answer(autoModeState);

    if (ui.update("led3")) ui.answer(triggerState);
  }
}