#ifndef WIFI_H
#define WIFI_H

#include "esp_event.h"
#include "iterations.h"

void initialize_wifi_nvs(iterations *object);
static void wifi_task_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data);

// ==> Rede privada local <==
#define WIFI_SSID "brisa-1355292"
#define WIFI_PASSWORD "st0xcpmq"

// ==> Rede móvel <==
/*
#define WIFI_SSID "george"
#define WIFI_PASSWORD ""
*/

// ==> Rede Wi-Fi IFRN <==
/*
#define WIFI_SSID
#define WIFI_PASSWORD
*/


#endif