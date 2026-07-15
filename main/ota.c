#include "ota.h"
#include <stdio.h>
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_event.h"

#include "esp_ota_ops.h"
#include "esp_https_ota.h"
#include "general_definitions.h"
#include "iterations.h"


TaskHandle_t* ota_return_task;

bool* ota_return_bool;



static void ota_task(void *param)
{
    ESP_LOGI("ota_debug", "Iniciando a atualização segura de troca de partição (Safe Swap)...");
/*
    esp_http_client_config_t config_client = 
    {
        .url
    }
*/
    while(1)
    {
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
    
}



void ota_validation_checker(void)
{

}



void ota_start(TaskHandle_t *task, bool *state_connecting)
{
    ota_return_task = task;

    ota_return_bool = state_connecting;


    *ota_return_bool = 1;

    vTaskResume(*ota_return_task);

    xTaskCreate(&ota_task, "atualizacao_ota", 8192, NULL, 5, NULL);
}