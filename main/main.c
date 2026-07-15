#include <stdio.h>
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "nvs_flash.h"

#include "wifi.h"
#include "mqtt.h"
#include "general_definitions.h"
#include "iterations.h"

//esp_err_t é um tipo inteiro que está relacionado a macros de erros, cada uma com seu código específico
//esp_wifi_init(WIFI_INIT_CONFIG_DEFAULT); ===> ERRADO


void app_main(void)
{
    iterations* retorno = initialize_iterations(); //CUIDADO COM CÓPIA ==> ENDEREÇOS DE MEMÓRIA DIFERENTES

            printf("%s", "Wi-Fi_state_RETORNO: ");
            printf("%p \n", &retorno->connecting_wifi);

            printf("%s", "Mqtt_state_RETORNO: ");
            printf("%p \n", &retorno->connecting_mqtt);

            printf("%s", "OTA_state_RETORNO: ");
            printf("%p \n", &retorno->starting_ota);

    vTaskDelay(2000 / portTICK_PERIOD_MS);

    initialize_wifi_nvs(retorno);
    //initialize_mqtt(&retorno);
}