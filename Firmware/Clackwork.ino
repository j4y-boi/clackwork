#include <Arduino_GFX_Library.h> // we usin arduinio_gfx instead of Adafruit_ST7789.h
#include <SPI.h> 

#include <WiFi.h>
#include <time.h>
#include <HijelHID_BLEKeyboard.h>
#include <Keypad.h>
#include <Preferences.h>

#include "FreeMono12pt7b.h" //fonts :))))
#include "Picopixel.h" // small font

#define TFT_SCLK 9
#define TFT_MOSI 10
#define TFT_DC 4
#define TFT_CS 5

#define BUZZER 6

const byte ROWS = 2;
const byte COLS = 4;

char keys[ROWS][COLS] = { // note to self: "" for string '' for characters
  {'1', '2', '3', '4'},
  {'5', '6', '7', '8'}
};

byte rowPins[ROWS] = {7, 8};
byte colPins[COLS] = {0, 1, 2, 3};

Keypad customKeypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);
HijelHID_BLEKeyboard keyboard("Clackwork Macropad", "j4y_boi", 100); //sets bt macropad up

Arduino_DataBus *bus = new Arduino_ESP32SPI(TFT_DC, TFT_CS, TFT_SCLK, TFT_MOSI, GFX_NOT_DEFINED); // GFX)NOT_DEFINED cuz rst is hardwired to v3.3
Arduino_GFX *gfx = new Arduino_ST7789(bus, GFX_NOT_DEFINED, 1, false, 76, 284, 82, 18, 82, 18);

// CONFIG (cuz idk how to do properly, its only like my third time working in c++)
const char* WIFI_SSID = "SSID";
const char* WIFI_PASSWORD = "Password";
const int WIFI_CONNECTION_TIMEOUT = 15000; // in ms, 15000 default (15s)
const char* TIME_ZONE = "CET-1CEST,M3.5.0,M10.5.0/3"; // i got ts from https://github.com/nayarsystems/posix_tz_db/blob/master/zones.csv
const uint8_t CLIP_KEYBIND = KEY_F7; // my nvidia instant replay keybind is F7 c:
const int ALARM_BEEP_TIME= 500; // how fast the beeps are
const int AUTOMATIC_ALARM_STOP = 30000; // when the alarm automatically stops beeping
// end of config wow

//bitmaps
static const unsigned char PROGMEM image__31_bits[] = {0x00,0x00,0x00,0x00,0x00,0x00,0xfc,0x00,0x1e,0x3c,0x84,0x78,0x1e,0x3c,0x84,0x78,0x1e,0x3c,0x84,0x78,0x1e,0x3c,0x84,0x78,0x00,0x00,0xfc,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x1e,0x3c,0x78,0x78,0x1e,0x3c,0x78,0x78,0x1e,0x3c,0x78,0x78,0x1e,0x3c,0x78,0x78,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};

static const unsigned char PROGMEM image__32_bits[] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x1e,0x3c,0x78,0x78,0x1e,0x3c,0x78,0x78,0x1e,0x3c,0x78,0x78,0x1e,0x3c,0x78,0x78,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xfc,0x00,0x1e,0x3c,0x84,0x78,0x1e,0x3c,0x84,0x78,0x1e,0x3c,0x84,0x78,0x1e,0x3c,0x84,0x78,0x00,0x00,0xfc,0x00,0x00,0x00,0x00,0x00};

static const unsigned char PROGMEM image__41_bits[] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xfc,0x1e,0x3c,0x78,0x84,0x1e,0x3c,0x78,0x84,0x1e,0x3c,0x78,0x84,0x1e,0x3c,0x78,0x84,0x00,0x00,0x00,0xfc,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x1e,0x3c,0x78,0x78,0x1e,0x3c,0x78,0x78,0x1e,0x3c,0x78,0x78,0x1e,0x3c,0x78,0x78,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};

static const unsigned char PROGMEM image__42_bits[] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x1e,0x3c,0x78,0x78,0x1e,0x3c,0x78,0x78,0x1e,0x3c,0x78,0x78,0x1e,0x3c,0x78,0x78,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xfc,0x1e,0x3c,0x78,0x84,0x1e,0x3c,0x78,0x84,0x1e,0x3c,0x78,0x84,0x1e,0x3c,0x78,0x84,0x00,0x00,0x00,0xfc,0x00,0x00,0x00,0x00};

static const unsigned char PROGMEM image_arrow_curved_left_up_bits[] = {0x22,0x6c,0xf8,0x60,0x20};

static const unsigned char PROGMEM image_bluetooth_bits[] = {0x01,0x00,0x02,0x80,0x02,0x40,0x22,0x20,0x12,0x20,0x0a,0x40,0x06,0x80,0x03,0x00,0x06,0x80,0x0a,0x40,0x12,0x20,0x22,0x20,0x02,0x40,0x02,0x80,0x01,0x00,0x00,0x00};

static const unsigned char PROGMEM image_bluetooth_not_connected_bits[] = {0x02,0x00,0x83,0x00,0x42,0x80,0x22,0x40,0x12,0x20,0x08,0x20,0x04,0x40,0x02,0x00,0x05,0x00,0x0a,0x80,0x12,0x40,0x22,0x20,0x02,0x50,0x02,0x88,0x01,0x04,0x00,0x00};

static const unsigned char PROGMEM image_music_next_bits[] = {0x00,0x00,0xe0,0x0f,0x90,0x09,0x8c,0x09,0x82,0x09,0x81,0x89,0x80,0x49,0x80,0x39,0x80,0x39,0x80,0x49,0x81,0x89,0x82,0x09,0x8c,0x09,0x90,0x09,0xe0,0x0f,0x00,0x00};

static const unsigned char PROGMEM image_music_next_button_bits[] = {0x07,0xc0,0x18,0x30,0x20,0x08,0x50,0x04,0x58,0x74,0x96,0x52,0x93,0x52,0x91,0xd2,0x93,0x52,0x96,0x52,0x58,0x74,0x50,0x04,0x20,0x08,0x18,0x30,0x07,0xc0,0x00,0x00};

static const unsigned char PROGMEM image_music_previous_bits[] = {0x00,0x00,0xf0,0x07,0x90,0x09,0x90,0x31,0x90,0x41,0x91,0x81,0x92,0x01,0x9c,0x01,0x9c,0x01,0x92,0x01,0x91,0x81,0x90,0x41,0x90,0x31,0x90,0x09,0xf0,0x07,0x00,0x00};

static const unsigned char PROGMEM image_music_record_button_black_bits[] = {0x07,0xc0,0x18,0x30,0x20,0x08,0x43,0x84,0x4f,0xe4,0x8f,0xe2,0x9f,0xf2,0x9f,0xf2,0x9f,0xf2,0x8f,0xe2,0x4f,0xe4,0x43,0x84,0x20,0x08,0x18,0x30,0x07,0xc0,0x00,0x00};

static const unsigned char PROGMEM image_operation_undo_bits[] = {0x00,0x00,0x00,0x00,0x00,0x00,0x8f,0x80,0xd0,0x40,0xe0,0x20,0xf0,0x10,0xf8,0x10,0x00,0x10,0x00,0x10,0x00,0x10,0x20,0x20,0x10,0x40,0x0f,0x80,0x00,0x00,0x00,0x00};

static const unsigned char PROGMEM image_volume_low_bits[] = {0x01,0xc0,0x00,0x02,0x40,0x00,0x04,0x40,0x00,0x08,0x40,0x00,0xf0,0x50,0x00,0x80,0x48,0x00,0x80,0x44,0x00,0x80,0x44,0x00,0x80,0x44,0x00,0x80,0x48,0x00,0xf0,0x50,0x00,0x08,0x40,0x00,0x04,0x40,0x00,0x02,0x40,0x00,0x01,0xc0,0x00,0x00,0x00,0x00};

static const unsigned char PROGMEM image_volume_normal_bits[] = {0x01,0xc0,0x00,0x02,0x40,0x00,0x04,0x42,0x00,0x08,0x41,0x00,0xf0,0x50,0x80,0x80,0x48,0x80,0x80,0x44,0x40,0x80,0x44,0x40,0x80,0x44,0x40,0x80,0x48,0x80,0xf0,0x50,0x80,0x08,0x41,0x00,0x04,0x42,0x00,0x02,0x40,0x00,0x01,0xc0,0x00,0x00,0x00,0x00};

enum ScreenMode { // this makes it possible to switch betweens creens with only one function later
  CLOCK_MODE,
  MACROPAD_MODE,
  SET_ALARM_MODE
};

ScreenMode currentScreen = CLOCK_MODE;

Preferences prefs;

int8_t lastBluetooth = -1; // -1 so we know that to update on draw
bool alarmOn = true;

bool editingHour = true;
uint8_t alarmHour = 7;
uint8_t alarmMinute = 30;
bool ringing = false;
bool alarmAlreadyFired = false;
unsigned long lastAlarmCheck = 0; // uhhhh its an unsigned long so it can store 4bil ms otherwise it will probably blow itself up
unsigned long ringStart = 0;

static char lastTime[8] = "";


void drawCurrentScreen(void){
  lastTime[0] = '\0'; //because this is apparently how you do string = "" in c++
  lastBluetooth = -1; // reset bluetooth icon status
  switch (currentScreen) {
    case CLOCK_MODE:
      drawClock_mode();
      break;
    case MACROPAD_MODE:
      drawMacropad_Mode();
      break;
    case SET_ALARM_MODE:
      drawSet_alarm();
      break; 
  }
}

void changeScreen(ScreenMode newScreen) { //so u can spare one line lmao
  currentScreen = newScreen;
  drawCurrentScreen();
}

void drawClock_mode(void) { 
    char timeText[8];
    char dateText[24];

    getCurrentTime(timeText, sizeof(timeText));
    getCurrentDate(dateText, sizeof(dateText));

    gfx->fillScreen(0x0);

    // Time
    gfx->setTextColor(0xFFFF);
    gfx->setTextWrap(false);
    gfx->setFont(&FreeMono12pt7b);
    gfx->setCursor(107, 30);
    gfx->println(timeText);
    // Date
    gfx->setCursor(51, 54);
    gfx->println(dateText);
    // Mode 
    gfx->setFont(&Picopixel);
    gfx->setCursor(3, 7);
    gfx->println("Clock Mode");
    // 42
    gfx->drawBitmap(251, 59, image__42_bits, 32, 16, 0xFFFF);
    // 41
    gfx->drawBitmap(251, 1, image__41_bits, 32, 16, 0xFFFF);
    // string 31
    gfx->setFont();
    gfx->setCursor(203, 5);
    gfx->println("Macropad");
    // Back
    gfx->setCursor(197, 64);
    gfx->println("Set Alarm");
    // 32
    gfx->drawBitmap(1, 59, image__32_bits, 32, 16, 0xFFFF);
    // Down
    gfx->setCursor(34, 64);
    if (alarmOn) {
      gfx->println("Turn off alarm");
    } else {
      gfx->println("Turn on alarm");
    }
}

void drawMacropad_Mode(void) {
    gfx->fillScreen(0x0);

    // title
    gfx->setTextColor(0xFFFF);
    gfx->setTextWrap(false);
    gfx->setCursor(104, 8);
    gfx->setFont(); // idk why lopaka didnt include this
    gfx->println("Macropad Mode");
    // 8
    gfx->drawRect(182, 48, 35, 21, 0xFFFF);
    // 7
    gfx->drawRect(144, 48, 35, 21, 0xFFFF);
    // 6
    gfx->drawRect(106, 48, 35, 21, 0xFFFF);
    // 5
    gfx->drawRect(68, 48, 35, 21, 0xFFFF);
    // 4
    gfx->drawRect(182, 24, 35, 21, 0xFFFF);
    // 3
    gfx->drawRect(144, 24, 35, 21, 0xFFFF);
    // 2
    gfx->drawRect(106, 24, 35, 21, 0xFFFF);
    // 1
    gfx->drawRect(68, 24, 35, 21, 0xFFFF);
    // music_next_button
    gfx->drawBitmap(78, 51, image_music_next_button_bits, 15, 16, 0xFFFF);
    // volume_normal
    gfx->drawBitmap(114, 27, image_volume_normal_bits, 18, 16, 0xFFFF);
    // volume_low
    gfx->drawBitmap(153, 27, image_volume_low_bits, 18, 16, 0xFFFF);
    // music_previous
    gfx->drawBitmap(114, 51, image_music_previous_bits, 16, 16, 0xFFFF);
    // music_next
    gfx->drawBitmap(152, 51, image_music_next_bits, 16, 16, 0xFFFF);
    // music_record_button_black
    gfx->drawBitmap(192, 27, image_music_record_button_black_bits, 15, 16, 0xFFFF);
    // operation_undo
    gfx->drawBitmap(79, 26, image_operation_undo_bits, 12, 16, 0xFFFF);
    // arrow_curved_left_up
    gfx->drawBitmap(206, 61, image_arrow_curved_left_up_bits, 7, 5, 0xFFFF);
}

void drawSet_alarm(void) {
    gfx->fillScreen(0x0);

    char alarmTime[6];
    snprintf(alarmTime, sizeof(alarmTime), "%02u:%02u", alarmHour, alarmMinute);

    gfx->setTextColor(0xFFFF);
    gfx->setTextWrap(false);
    gfx->setFont(&FreeMono12pt7b);
    gfx->setCursor(107, 45);
    gfx->println(alarmTime);
    // Time
    gfx->setFont();
    gfx->setCursor(38, 7);
    gfx->println("What time should your alarm go off?");
    // Mode 
    gfx->setFont(&Picopixel);
    gfx->setCursor(3, 72);
    gfx->println("Set alarm");
    // 31
    gfx->drawBitmap(20, 22, image__31_bits, 32, 16, 0xFFFF);
    // 32
    gfx->drawBitmap(20, 40, image__32_bits, 32, 16, 0xFFFF);
    // 42
    gfx->drawBitmap(232, 41, image__42_bits, 32, 16, 0xFFFF);
    // 41
    gfx->drawBitmap(232, 20, image__41_bits, 32, 16, 0xFFFF);
    // string 31
    gfx->setFont();
    gfx->setCursor(195, 25);
    gfx->println("Switch");
    // Back
    gfx->setCursor(207, 46);
    gfx->println("Back");
    // Up
    gfx->setCursor(53, 27);
    gfx->println("Up");
    // Down
    gfx->setCursor(53, 45);
    gfx->println("Down");
    // cursor
    if (editingHour) {
      gfx->drawLine(110, 49, 131, 49, 0xBDF7);
    } else {
      gfx->drawLine(152, 49, 173, 49, 0xBDF7);
    }
}

void updateBluetoothIndicator(void) {
  bool bluetoothStatus = keyboard.isConnected();

  if ((int8_t)bluetoothStatus != lastBluetooth){ // the fact that bools can be converted to an int is incredible, tysm tutorialspoint ong

    gfx->fillRect(2, 2, 14, 16, 0x0000);  //draw black rect that removes old bitmap draw
    if (bluetoothStatus) {
      gfx->drawBitmap(2, 2, image_bluetooth_bits, 14, 16, 0xFFFF);
    } else {
      gfx->drawBitmap(2, 2, image_bluetooth_not_connected_bits, 14, 16, 0xFFFF);
    }

    lastBluetooth = bluetoothStatus;
  }
}

void updateTopRightClock(void) {
  char timeText[8];
  getCurrentTime(timeText, sizeof(timeText));

  if (strcmp(timeText, lastTime) == 0) { // if its the same we do NOTHING
    return;
  }

  strcpy(lastTime, timeText);
  gfx->setFont(); //default font
  gfx->fillRect(252, 3, 30, 8, 0x0000);

  gfx->setTextColor(0xFFFF);
  gfx->setCursor(252, 3);
  gfx->print(timeText);
}

void setUpWifi() { //syncs time using ntp
  WiFi.mode(WIFI_STA); //station mode, it can connecc but it dont modem
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  Serial.println("Connecting to wifi");

  unsigned long startAttempt = millis(); //millis returns time since start script

  while (WiFi.status() != WL_CONNECTED && (millis() - startAttempt) < WIFI_CONNECTION_TIMEOUT) {
    Serial.print("."); //dotted line
    delay(250);
  }

  Serial.println();

  if (WiFi.status() == WL_CONNECTED) {
    configTzTime(
      TIME_ZONE,
      "pool.ntp.org",
      "time.google.com"
    );
  }
}

void getCurrentTime(char* timeText, size_t size) {
  struct tm timeInfo; //tysm https://randomnerdtutorials.com/esp32-date-time-ntp-client-server-arduino/

  if (getLocalTime(&timeInfo, 0)) {
    strftime(timeText, size, "%H:%M", &timeInfo);
  } else {
    snprintf(timeText, size, "--:--"); // if no time found (if no wifi for time sync for example)
  }
}

void getCurrentDate(char* dateText, size_t size) {
  struct tm timeInfo;

  if (getLocalTime(&timeInfo,0)) {
    strftime(dateText, size, "%a. %d/%m/%y", &timeInfo);
  } else {
    snprintf(dateText, size, "No date found");
  }
}

void checkAlarmTime(void) {
  if (millis() - lastAlarmCheck < 1000) {
    return;
  }

  lastAlarmCheck = millis();

  struct tm t;
  if (!alarmOn || !getLocalTime(&t, 0)) {
    return;
  }

  if (t.tm_hour == alarmHour && t.tm_min == alarmMinute) {
    if (alarmAlreadyFired == false) {
      ringing = true;
      alarmAlreadyFired = true;
      ringStart = millis();
    }
  } else {
    alarmAlreadyFired = false;
  }
}

void refreshClockScreen(void) {
  char t[8];
  getCurrentTime(t, sizeof(t));
  if (strcmp(t, lastTime) == 0){ 
    return;
  }
  strcpy(lastTime, t);
  drawClock_mode();
}

void setup() {
  Serial.begin(115200);
  pinMode(BUZZER, OUTPUT);

  prefs.begin("clackwork", false);
  alarmHour = prefs.getUChar("hour",7);
  alarmMinute = prefs.getUChar("minute",30);
  alarmOn = prefs.getUChar("on",true);

  gfx->begin();
  drawCurrentScreen();

  keyboard.begin(); // set up bluetooth
  keyboard.end(); // not keyboarding stuff until macropad mode

  setUpWifi();
}

void loop() {
  char pressedKey = customKeypad.getKey();
  checkAlarmTime();

  if (ringing) {
    if ((millis()/ALARM_BEEP_TIME)%2) {
      tone(BUZZER, 2000);
    } else {
      noTone(BUZZER);
    }
    
    if (pressedKey || (millis() - ringStart > AUTOMATIC_ALARM_STOP)) {
      ringing = false;
      noTone(BUZZER);
      return;
    }
  }

  if (currentScreen != CLOCK_MODE) {  // manages the top right timer (shows up in macro mode and alarm mode)
    updateTopRightClock();
  } else {
    refreshClockScreen();
  }

  if (currentScreen == MACROPAD_MODE) { // macropad behaviour
    updateBluetoothIndicator();
    if (pressedKey) {
      switch(pressedKey) {
        case '1':
          keyboard.tap(KEY_Z, KEY_MOD_LCTRL);
          break;
        case '2':
          keyboard.tap(MEDIA_VOLUME_UP);
          break;
        case '3':
          keyboard.tap(MEDIA_VOLUME_DOWN);
          break;
        case '4':
          keyboard.tap(CLIP_KEYBIND); // my nvidia instant replay keybind is f7 lmao
          break;
        case '5':
          keyboard.tap(MEDIA_PLAY_PAUSE);
          break;
        case '6':
          keyboard.tap(MEDIA_PREV_TRACK);
          break;
        case '7':
          keyboard.tap(MEDIA_NEXT_TRACK);
          break;
        case '8':
          keyboard.end(); // disconnects and stops adverttising device
          changeScreen(CLOCK_MODE);
          break;
      }
    }
  } else if (currentScreen == CLOCK_MODE) {
    if (pressedKey) {
      switch(pressedKey) {
        case '4':
          keyboard.begin(); // this makes the macropad discoverable via bluetooth
          changeScreen(MACROPAD_MODE);
          break;
        case '7':
          alarmOn = !alarmOn;
          prefs.putUChar("on", alarmOn);
          drawCurrentScreen(); // refreshes entire screen (and with it the label)
          break;
        case '8':
          changeScreen(SET_ALARM_MODE);
          break;
      }
    }
  } else if (currentScreen == SET_ALARM_MODE) {
      switch(pressedKey) {
        case '3': // up key
          if (editingHour) {
            alarmHour = (alarmHour + 1) % 24; // wraparound yes
          } else {
            alarmMinute = (alarmMinute + 1) % 60;
          }
          drawCurrentScreen(); // refreshes entire screen (and with it the label)
          break;
        case '7': // down key
          if (editingHour) {
            alarmHour = (alarmHour + 23) % 24; //just noticed this lmao, so if i do plus 24-1 it should just wrap around hopefully
          } else {
            alarmMinute = (alarmMinute + 59) % 60;
          }
          drawCurrentScreen();
          break;
        case '4':
          editingHour = !editingHour;
          drawCurrentScreen();
          break;
        case '8':
          prefs.putUChar("minute",alarmMinute);
          prefs.putUChar("hour",alarmHour);
          changeScreen(CLOCK_MODE);
          editingHour = true;
          break;
      }
  }
}