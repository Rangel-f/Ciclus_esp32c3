#ifndef MQTT_H
#define MQTT_H

#include "wifi.h"
#include "esp_event.h"
#include "mqtt_client.h"

void initialize_mqtt(TaskHandle_t *task, esp_mqtt_client_handle_t *client, bool *state_connecting, bool *state_ota);
static void mqtt_task_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data);

// ==> Broker privado EMQX <==
#define MQTT_BROKER_URL "mqtts://n8ee66fe.ala.us-east-1.emqxsl.com:8883"  

// ==> Tópicos <==
#define MQTT_TOPIC_PUBLISH "esp32c3/mqtt_publish_esp32c3LOCAL"
#define MQTT_TOPIC_SUBSCRIBE "esp32c3/mqtt_subscribe_esp32c3LOCAL"

// ==> Usuário autenticado <==
#define MQTT_USER "ESP_LocalGeorge"
#define MQTT_PASSWORD "ESP_LocalGeorgeTesteSenha1234567!"

// ==> Client ID com permissão para publish e subscribe nos tópicos especificados <==
#define MQTT_CLIENT_ID "esp32c3SuperMini_old" 

#endif