void setup() {
  Serial.begin(115200);
  gpio_init();
  EEPROM.begin(1024);

  mem1.begin(0, 'b');
  mem2.begin(mem1.nextAddr(), 'b');
  mem3.begin(mem2.nextAddr(), 'b');

  if (digitalRead(TRIGPIN) == Settings.convTrig && WF.apply == 0) {  //если переключатель режимов нажат то запускается wifi
    WiFi.mode(WIFI_STA);
    WiFi.begin(AP_SSID, AP_PASS);
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }
    Serial.println(WiFi.localIP());
    Serial.println(WF.WF_SSID);
    Serial.println(WF.WF_PASS);
    Serial.println(WF.apply);
  } else if (digitalRead(TRIGPIN) == Settings.convTrig) {
    isWiFiFlag = false;
    isSetupMode = true;
    WiFi.mode(WIFI_AP);
    WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
    WiFi.softAP(WF.WF_SSID, WF.WF_PASS);
    //WiFi.softAP(WF.WF_SSID);
    Serial.print("AP: ");
    Serial.println(WiFi.softAPIP());
  } else {
    WiFi.mode(WIFI_OFF);
    WiFi.forceSleepBegin();
    isWiFiFlag = true;
    isSetupMode = false;
    delay(10);
  }

  // подключаем конструктор и запускаем
  ui.attachBuild(build);
  ui.attach(action);
  ui.start();
  ui.enableOTA();

  if (!LittleFS.begin()) Serial.println("FS Error");
  ui.downloadAuto(true);

  pinMode(SOLPIN, OUTPUT);
  pinMode(SOLPIN2, OUTPUT);
  pinMode(TRIGPIN, INPUT);
  pinMode(FIREMODESW, INPUT);
  pinMode(TRACER, OUTPUT);

  digitalWrite(SOLPIN, LOW);
  digitalWrite(SOLPIN2, LOW);
  digitalWrite(TRACER, LOW);

  if (Settings.reverseSolenoid2 == true && Settings.HPAsystemType == DSOL) {
    digitalWrite(SOLPIN2, HIGH);  // Нормально выдвинут - подаем HIGH для задвижения
    solenoid2Active = false;
  } else {
    digitalWrite(SOLPIN2, LOW);  // Нормально задвинут
    solenoid2Active = false;
  }

  if (Settings.HPAsystemType == NONSINGLESYSTEM) {
    cycleTime = Settings.shotTime + Settings.shotWait;
  }

  if (Settings.isSWProgSafe) {
    pinMode(PROGSAFE, INPUT);
  }

  if (WR.writeCount == 0) {
    digitalWrite(SOLPIN, HIGH);
    digitalWrite(SOLPIN2, HIGH);
    digitalWrite(TRACER, HIGH);
    delay(100);
    digitalWrite(SOLPIN, LOW);
    digitalWrite(SOLPIN2, LOW);
    digitalWrite(TRACER, LOW);
  }
  if (isWiFiFlag == true && Settings.isSoundModule == true) {
    Serial.begin(9600);
    mp3.reset();
    mp3.setVolume(Settings.SMVolume);
    mp3.setLoopMode(MP3_LOOP_NONE);
  }
}