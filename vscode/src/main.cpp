#include <M5Unified.h>
#include "config.h"
#include "servo.h"
#include "avatar.h"
#include "scratchlink.h"

void setup()
{
  M5.begin();
  Serial.begin(115200);
  Serial.println("Waiting 3 seconds...");
  delay(3000);

  M5.setLogDisplayIndex(0);
  M5.Log.setLogLevel(m5::log_target_display, ESP_LOG_INFO);
  M5.Log.setLogLevel(m5::log_target_serial, ESP_LOG_INFO);
  M5.Log.setEnableColor(m5::log_target_serial, false);
  M5_LOGI("Initializing ...");

  init_scratchlink("M5-stackchan");

  M5_LOGI("Starting ...");
  M5.Log.setLogLevel(m5::log_target_display, ESP_LOG_NONE);
  delay(2000);

  init_avatar();
}

#define is_digit(c) ((c)>='0' && (c)<='9')
#define is_binary(c) ((c)=='0' || (c)=='1')

int natoi(const char *buf, int length)
{
  int result = 0;
  char c;

  while (c = *buf++, is_digit(c) && length --) {
    result *= 10;
    result += c - '0';
  }
  return result;
}

void scratchlink_callback(byte command, byte *payload, unsigned int length)
{
  M5_LOGD("ScratchLink: Command=%d, Payload=%s", command, payload);
  if (command == ScratchLink_Say) {
    M5_LOGI("say received");
    if (!strncmp((const char *) payload, "1=", 2)) {
      int degrees = natoi((const char *) &payload[2], length - 2);
      servo_set(1, degrees);
      return;
    }
    if (!strncmp((const char *) payload, "2=", 2)) {
      int degrees = natoi((const char *) &payload[2], length - 2);
      servo_set(2, degrees);
      return;
    }
    String text;
    for (int i = 0; i < length; i ++) {
      text += (char) payload[i];
    }
    text += '\0';
    avatar_say(text.c_str());
  } else if (command == ScratchLink_Led) {
    int value = 0;
    value |= (int) payload[4];
    value |= (int) payload[3] << 5;
    value |= (int) payload[2] << 10;
    value |= (int) payload[1] << 15;
    value |= (int) payload[0] << 20;
    M5_LOGI("led received=%X", value);
    int face = Expression_Neutral;
    if (value == 0x11503F5) {
      face = Expression_Angry;
    }
    if (value == 0xD81C0) {
      face = Expression_Sleepy;
    }
    if (value == 0x5022E) {
      face = Expression_Happy;
    }
    if (value == 0x501D1) {
      face = Expression_Sad;
    }
    if (value == 0x50155) {
      face = Expression_Doubt;
    }
    avatar_expression(face);
  }
}

int msec = 0;

void loop()
{
  M5.update();
  if (!scratchlink_isconnecting()) {
    if (M5.BtnA.wasClicked()) {
      servo_set(1, 10);
      servo_set(2, 30);
    }
    if (M5.BtnB.wasClicked()) {
      servo_set(1, 90);
      servo_set(2, 50);
    }
    if (M5.BtnC.wasClicked()) {
      servo_set(1, 170);
      servo_set(2, 70);
    }
  }
  servo_update();
  if (millis() - msec > 1000) {
    msec = millis();
    scratchlink_update(0, 0, M5.BtnA.isHolding(), M5.BtnB.isHolding(), false, false, false, 0);
  }
  delay(10);
}
