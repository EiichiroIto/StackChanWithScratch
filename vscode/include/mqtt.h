#ifndef __MQTT__
#define __MQTT__

#define MQTT_CLIENT_ID_PREFIX "esp32-client-"

extern char MQTT_BrokerHost[];

extern void init_mqtt();
extern void mqtt_subscribe(const char *topic);
extern void mqtt_publish(const char *topic, const char *payload);
extern void mqtt_loop();
extern void mqtt_callback(const char *topic, byte *payload, unsigned int length);

#endif
