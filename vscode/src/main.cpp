#include <M5Unified.h>
#include "config.h"
#include "settings.h"
#include "servo.h"
#include "avatar.h"
#include "wifi.h"
#include "mqtt.h"

char WiFi_SSID[SETTING_LENGTH] = "";
char WiFi_PASSWORD[SETTING_LENGTH] = "";
char MQTT_BrokerHost[SETTING_LENGTH] = "";
char MQTT_Topic[SETTING_LENGTH] = DEFAULT_MQTT_TOPIC;

int NumSettings = 4;
Settings AppSettings[] = {
  {"ssid", WiFi_SSID, "WiFi SSID", true},
  {"password", WiFi_PASSWORD, "WiFi Password", true},
  {"broker", MQTT_BrokerHost, "MQTT Broker", true},
  {"topic", MQTT_Topic, "MQTT Topic", true},
};

String MQTT_TopicAll = "";
String MQTT_TopicSay = "";
String MQTT_TopicLed = "";

void setup()
{
  M5.begin();
  Serial.begin(115200);
  Serial.println("Waiting 3 seconds...");
  delay(3000);
  init_settings();

  M5.setLogDisplayIndex(0);
  M5.Log.setLogLevel(m5::log_target_display, ESP_LOG_INFO);
  M5.Log.setLogLevel(m5::log_target_serial, ESP_LOG_INFO);
  M5.Log.setEnableColor(m5::log_target_serial, false);
  M5_LOGI("Initializing ...");

  init_wifi();
  connect_wifi();
  init_mqtt();
  init_servo();

  MQTT_TopicAll.concat(MQTT_Topic);
  MQTT_TopicAll.concat("/#");
  MQTT_TopicSay.concat(MQTT_Topic);
  MQTT_TopicSay.concat("/say");
  MQTT_TopicLed.concat(MQTT_Topic);
  MQTT_TopicLed.concat("/led");
  mqtt_subscribe(MQTT_TopicAll.c_str());

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

int nbtoi(const char *buf, int length)
{
  int result = 0;
  char c;

  while (c = *buf++, is_binary(c) && length --) {
    result *= 2;
    result += c - '0';
  }
  return result;
}

void mqtt_callback(const char *topic, byte *payload, unsigned int length)
{
  M5_LOGD("MQTT: Topic=%s, Payload=%s", topic, payload);
  if (!strcmp(topic, MQTT_TopicSay.c_str())) {
    M5_LOGI("say received");
    if (!strncmp((const char *) payload, "*s1=", 4)) {
      int degrees = natoi((const char *) &payload[4], length - 4);
      servo_set(1, degrees);
      return;
    }
    if (!strncmp((const char *) payload, "*s2=", 4)) {
      int degrees = natoi((const char *) &payload[4], length - 4);
      servo_set(2, degrees);
      return;
    }
    String text;
    for (int i = 0; i < length; i ++) {
      text += (char) payload[i];
    }
    text += '\0';
    avatar_say(text.c_str());
  } else if (!strcmp(topic, MQTT_TopicLed.c_str())) {
    int value = nbtoi((const char *) payload, length);
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

void loop()
{
  M5.update();
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
  servo_update();
  mqtt_loop();
  delay(10);
}
