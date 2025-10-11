#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <LittleFS.h>

#define AP_SSID "ZTV"     //логин
#define AP_PASS "12345678"  //пароль от wifi готовой точки доступа

#include <GyverPortal.h>
GyverPortal ui(&LittleFS);

#include <EEManager.h>  // подключаем либу

#define PROGSAFE D3     //Пин программного предохранителя (ошибка если будет нажат при старте)
#define FIREMODESW D4  //Переключатель режимов огня (ошибка если будет нажат при старте)
#define SOLPIN D5      //пин соленоида основного
#define SOLPIN2 D6      //пин соленоида 2
#define TRIGPIN D7     //пин спускового крючка
#define TRACER D8    //Пин для подключения дополнительной нагрузки (трассерка или бубен)
//#define analogPin A0   //разъем для замера напряжения на батарее

#define AUTOMODE 0  //РЕЖИМ АВТОМАТА
#define SNIPERMODE 1 //РЕЖИМ СНАЙПЕРСКИЙ
#define CQBMODE 2 //CQB РЕЖИМ
#define MACHINEGUNMODE 3  //РЕЖИМ ПУЛЕМЕТА

#define SEMI 1  //одиночка
#define AUTO 2  //очередь

//выбор платы
#define CUSTOMGUN 0 //кастоный привод с включенными всеми настройками
#define M4 1  //Универсальная плата для M4 (Gearbox V2)
#define AK 2  //Универсальная плата для AK (Gearbox V3)
#define M4A 3 //Arcturus M4 серии ME (Gearbox V2)
#define KAC 4 //KAC Stoner LMG Classic Army

//Выбор системы
#define SSOL 0  //Односолиноидная система
#define DSOL 1  //Двухсоленоидная система
#define DOUBLEBURREL 2 //Двойной ствол, стрельба ведется из одного или другого по переключению переключателя огня
#define NONSINGLESYSTEM 3 //многоствольная система

//Тип работы нагрузки
#define NOPE  0  //ничего не делаем
#define JUSTON 1 //тупо включается и работает всё время
#define DELAFTERSHOTSEC 2 //включается и выключается с задержкой после выстрела в секундах
#define DELAFTERSHOTMIN 3 //включается и выключается с задержкой после выстрела в минутах

#define TWOBURREL 0 //2 ствола на пулемете
#define THREEBURREL 1 //3 ствола на пулемете

#define SINUS 0 //стрельба последовательная как синусоида в 3-х фазной цепи
#define TOGETHER 1 //стрельба из всех стволов одновременно

int autoMode = 0;               //режим стрельбы 1-одиночка, 2-очередь
int voltValue = 0;           //Значение с аналогового пина
unsigned long voltTime = 0;  //Время с последней проверки напряжения
bool isResFl; //
float resVoltValue = 0.0;
float procVol = 0.0;
unsigned long lastShot = 0;  //время последнего выстрела
bool isLS;                     //флаг для отсчета времени до сохранения настрела
bool isLSTr;                  //флаг для трассерки
bool isLD2;                   //флаг для второй нагрузки типа бубен
unsigned long tracLastShot = 0; //время последнего выстрела для трассерки
unsigned long load2Lastshot = 0;  //время последнего выстрела для 2 нагрузки
bool isFlagSafe;  //флаг предохранителя
bool isFireMode;  //флаг переключателя режимов огна
bool isTrig;      //флаг спускового крючка
unsigned long deepSleepTime = 0;  //миллисекунд до ухода в сон
bool isBatSafeFlag = false; //флаг для отключения в случае разряда аккумулятора
bool isWiFiFlag = true;
unsigned long cycleStartTime = 0; //начало цикла для многоствольного пулемета
int cycleTime = 0;  //
int phaseShift = 0; //фазный сдвиг для многоствольного пулемета
int solenoidCount = 3;

//состояние переключателей
bool triggerState;
bool safetyState;
bool autoModeState;

// Состояния управления
bool solenoid1Active = false;
bool solenoid2Active = false;

int spupd = 1000;
unsigned long lastspupd;

struct {                    //структура в которой хранятся настройки
  int shotTime = 25;       //1. время в милисекундах открытия клапана
  int shotWait = 25;       //2. время в милисекундах закрытия клапана
  int convSel = false;          //3. настройка конвертации для нормально замкнутых (1) и нормально открытых контактов (0) для переключателя режимов огня
  int numOfShotsSemi = 5;   //4. Количество выстрелов при одиночном режиме
  int numOfShotsAuto = 55;  //5. Количество выстрелов при автоматическом режиме стрельбы
  int weightBall = 0.28;    //6. Вес шара
  int Mode = 0;            //7. Тип стрельбы
  /*подрежим стрельбы
0-обычный режим, автомат
1-режим снайперский
2-режим CQB
3-режим пулемета
*/
  int delForErr = 1;     //8. Задержка от дребезга
  int timeLastShot = 1;  //9. время после последнего выстрела для сохранения количества совершенных выстрелов в eeprom
  int writeCount = 0;    //15. Кол-во записей в eeprom. Заявлено что eeprom esp поддерживает около 10.000 перезаписей после чего сделать переход к следующей ячейке.
  int shotDelay = 0;  //задержка между выстрелами
  int convTrig = false;  //конвертация спускового крючка, 0 - нормально замкнутый, 1 - нормально разомкнутый
  bool isDoubleShot = false;   //включение двойного выстрела
  bool isConvSafe = false;      //конвертация предохранителя
  bool isTracer = 0;  //включение трассерки
  int tracTime = 1; //секунд до отключения трассерной подсветки
  float voltCorr = 1.0;  //напряжение для рассчета напряжения на аккумуляторе
  float divR1 = 120.0;  //сопротивление R1 на делителе
  float divR2 = 10.0;   //сопротивление R2 на делителе
  int batType = 0;  //тип литиевого аккумулятора, 0-2s, 1-3s
  bool isDeepSleep = false; //переключатель глубокого сна
  int deepSleepMin = 120; //минут до ухода в глубокий сон
  bool isBatSafe = 0; //отключение в случае разряда аккумулятора
  bool isSWProgSafe = false;  //отключение програмного предохранителя (если включен то нельзя воспользоваться I2C)
  int del2SolBegin = 2; //задержка на выдвижение нозла перед выстрелом
  int del2SolEnd = 2; //Задержка на задвигание после выстрела
  int PCB = 100;  //Платапо умолчанию АК
  int HPAsystemType = 100;  //Тип ВВД системы по умолчанию
  int Load2Select = 0;  //Тип работы нагрузки
  int Load3Select = 0;  //Тип работы нагрузки
  bool reverseSolenoid2 = false;
  bool openBolt = true;
  int TwoThreeBurrelMG = 1; //Количество стволов в пулемете 0 - 2 ствола, 1 - 3 ствола
  bool isLoad2 = 0; //Включение 2 нагрузки
  int Load2Time = 1;  //время включения нагрузки
  int FireType = 0; //режим стрельбы 0 - синус, 1 - одновременная
  float zap7;
  float zap8;
  float zap9;
} Settings;              //Название структуры к которой обращаемся

struct {                         //структура в которой храним настрел
  unsigned long shotCount = 0;  //кол-во выстрелов
  int writeCount = 0;            //кол-во записей в eeprom
} WR;

struct {
  String WF_SSID = "AP_ZTV";
  String WF_PASS = "12345678";
  bool apply = 1;
} WF;

IPAddress apIP(63, 63, 63, 63);

EEManager mem1(Settings);  // передаём нашу переменную (фактически её адрес)
EEManager mem2(WR);
EEManager mem3(WF);