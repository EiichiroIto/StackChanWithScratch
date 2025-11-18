#include <M5Unified.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include "config.h"
#include "mqtt.h"

extern WiFiClient wifiClient;

PubSubClient mqttClient(wifiClient);

void init_mqtt()
{
  M5_LOGI("Connecting MQTT Broker(%s)", MQTT_BrokerHost);
  mqttClient.setServer(MQTT_BrokerHost, MQTT_PORT);
  mqttClient.setCallback(mqtt_callback);
  while (!mqttClient.connected()) {
    String client_id = MQTT_CLIENT_ID_PREFIX;
    client_id += String(WiFi.macAddress());
    if (mqttClient.connect(client_id.c_str(), MQTT_USER, MQTT_PASSWORD)) {
      break;
    }
    M5_LOGI("MQTT: failed(%02X)", mqttClient.state());
    delay(2000);
  }
  M5_LOGI("Conneced");
}

void mqtt_subscribe(const char *topic)
{
  M5_LOGI("Subscribing Topic %s", topic);
  mqttClient.subscribe(topic);
}

void mqtt_publish(const char *topic, const char *payload)
{
  mqttClient.publish(topic, payload);
}

void mqtt_loop()
{
  mqttClient.loop();
}
