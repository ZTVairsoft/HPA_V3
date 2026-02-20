void enterDeepSleep() {
  ESP.deepSleep(0);
}

void wakeUp() {
  // Обработчик прерывания для пробуждения
  // Ничего не делаем, просто выходим из сна
}

void light_sleep() {
  digitalWrite(SOLPIN, LOW);
  digitalWrite(SOLPIN2, LOW);
  digitalWrite(TRACER, LOW);
  wifi_station_disconnect();
  wifi_fpm_set_sleep_type(LIGHT_SLEEP_T);
  wifi_set_opmode(NULL_MODE);
  wifi_set_sleep_type(MODEM_SLEEP_T);
  wifi_fpm_open();
  if (Settings.convSel == false) {
    gpio_pin_wakeup_enable(TRIGPIN, GPIO_PIN_INTR_LOLEVEL);
  } else {
    gpio_pin_wakeup_enable(TRIGPIN, GPIO_PIN_INTR_HILEVEL);
  }
  //wifi_fpm_set_wakeup_cb(wakeupCallback);  // Set wakeup callback (optional)
  wifi_fpm_do_sleep(0xFFFFFFF);  // only 0xFFFFFFF, any other value and it won't disconnect the RTC timer
}
