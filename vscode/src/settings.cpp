#include <M5Unified.h>
#include "SPIFFS.h"
#include "config.h"
#include "settings.h"

const char ConfigPath[] = "/config.txt";

extern void setup_settings();
extern void print_settings();
extern bool setup_setting(int num);
extern int load_settings();
extern void save_settings();
extern void clear_settings();

void init_settings()
{
  Serial.println("Mount Filesystem");
  if (!SPIFFS.begin(true)) {
    Serial.println("SPIFFS Mount Failed.");
    while (1)
      ;
  }
  Serial.print("Check settings\r\n");
  int ok = load_settings();
  M5.update();
  if (M5.BtnA.isPressed()) {
    ok = 0;
  }
  if (ok) {
    return;
  }
  setup_settings();
}

void setup_settings()
{
  bool changed = false;

  while (Serial.available()) {
    Serial.read();
  }
  while (1) {
    print_settings();
    Serial.printf("Select 1-%d settings or 9 (clear) or 0(done) : ", NumSettings);
    while (!Serial.available())
      ;
    int c = Serial.read();
    if (c == '0') {
      if (changed) {
        save_settings();
      }
      break;
    } else if (c >= '1' && c <= ('0' + NumSettings)) {
      Serial.printf("%c\r\n", c);
      if (setup_setting(c - '0')) {
        changed = true;
      }
    } else if (c == '9') {
      Serial.printf("%c\r\n", c);
      clear_settings();
    }
    Serial.print("\r\n");
  }
  Serial.print("\r\n");
}

#define is_letter(c) ((c)>='!' && (c)<='~')

void print_settings()
{
  Serial.print("Current settings:\r\n");
  for (int i = 0; i < NumSettings; i ++) {
    Serial.printf("%d. %s=[%s]\r\n", i + 1, AppSettings[i].name, AppSettings[i].var);
  }
}

int get_line(char *buf, int max)
{
  int len = 0;

  while (1) {
    while (!Serial.available())
      ;
    int c = Serial.read();
    if (c == 'H' - '@') {
      if (len) {
        len --;
        buf[len] = 0;
        Serial.printf("%c", c);
      }
    } else if (c == 13) {
      Serial.print("\r\n");
      break;
    } else if (is_letter(c)) {
      if (len < max - 1) {
        buf[len++] = c;
        buf[len] = 0;
        Serial.printf("%c", c);
      }
    }
  }
  return len;
}

bool setup_setting(int num)
{
  char buf[20];
  int len;

  Serial.print("Enter value : ");
  if (!get_line(buf, sizeof buf)) {
    return false;
  }
  strcpy(AppSettings[num - 1].var, buf);
  return true;
}

int read_keyAndValue(File &file, char *key, char *value)
{
  key[0] = value[0] = 0;
  char *dst = key;
  while (1) {
    int c = file.read();
    if (c < 0) {
      *dst = 0;
      return 0;
    } else if (c == 0x3D) {
      *dst = 0;
      dst = value;
    } else if (c == 0xD || c == 0x0A) {
      *dst = 0;
      return 1;
    } else if (c > 32) {
      *dst++ = c;
    }
  }
  return 1;
}

bool check_settingsSatisfied()
{
  for (int i = 0; i < NumSettings; i ++) {
    if (AppSettings[i].required && !AppSettings[i].var[0]) {
      return false;
    }
  }
  return true;
}

int load_settings()
{
  File file = SPIFFS.open(ConfigPath, FILE_READ);
  if (!file) {
    Serial.printf("Can't open %s\n", ConfigPath);
    return 0;
  }
  int ret;
  do {
    char key[SETTING_LENGTH], value[SETTING_LENGTH];
    ret = read_keyAndValue(file, key, value);
    if (*key && *value) {
      Serial.printf("key=[%s] value=[%s]\r\n", key, value);
      for (int i = 0; i < NumSettings; i ++) {
        if (strcmp(key, AppSettings[i].key)) {
          continue;
        }
        strcpy(AppSettings[i].var, value);
      }
    }
  } while (ret);
  file.close();
  return check_settingsSatisfied();
}

void save_settings()
{
  File file = SPIFFS.open(ConfigPath, FILE_WRITE);
  if (!file) {
    Serial.printf("Can't open %s\n", ConfigPath);
    return;
  }
  for (int i = 0; i < NumSettings; i ++) {
    const char *key = AppSettings[i].key;
    char *var = AppSettings[i].var;
    file.printf("%s=%s\r\n", key, var);
  }
  file.close();
  Serial.print("setting saved\r\n");
}

void clear_settings()
{
  SPIFFS.remove(ConfigPath);
  for (int i = 0; i < NumSettings; i ++) {
    AppSettings[i].var[0] = 0;
  }
  Serial.print("setting cleared\r\n");
}
