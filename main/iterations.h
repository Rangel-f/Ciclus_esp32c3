#ifndef ITERATIONS_H
#define ITERATIONS_H

#include "mqtt_client.h"


typedef struct
{
    //SemaphoreHandle_t wifi;
    //SemaphoreHandle_t mqtt;

    TaskHandle_t wifi_mqtt_handle;

    esp_mqtt_client_handle_t mqtt_client;

    bool connecting_wifi;

    bool connecting_mqtt;

    bool starting_ota;

} iterations;
// ==> Struct é apenas um modelo de variável, não a variável em si

void fase_de_conexao(void *param);
iterations* initialize_iterations();

#endif