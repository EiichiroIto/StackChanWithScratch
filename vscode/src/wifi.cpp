#include <M5Unified.h>
#include <WiFi.h>
#include "config.h"
#include "wifi.h"

WiFiClient wifiClient;

void init_wifi()
{
  M5_LOGI("WiFi SSID=[%s]", WiFi_SSID);
  M5_LOGI("WiFi PASSWORD=[%s]", WiFi_PASSWORD);
  WiFi.begin(WiFi_SSID, WiFi_PASSWORD);
}

void connect_wifi()
{
  M5_LOGI("Connecting WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  M5_LOGI("WiFi Conneced");
}
