#include "ota.h"
#include <stdio.h>
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_event.h"
#include "esp_crt_bundle.h"

#include "esp_ota_ops.h"
#include "esp_https_ota.h"
#include "general_definitions.h"
#include "iterations.h"


TaskHandle_t* ota_return_task;

bool* ota_return_bool;



static void ota_task(void *param)
{
    ESP_LOGI("ota_debug", "Iniciando a atualização segura de troca de partição (Safe Swap)...");

    esp_http_client_config_t config_client = 
    {
        .url = "https://github.com/Rangel-f/Ciclus_esp32c3/releases/download/v1.0/ciclus_esp32c3.bin",
        .crt_bundle_attach = esp_crt_bundle_attach,
        .keep_alive_enable = true,
        .buffer_size = 8192,
        .buffer_size_tx = 4096,
    };

    esp_https_ota_config_t config_ota = 
    {
        .http_config = &config_client,
    };


    esp_err_t ret = esp_https_ota(&config_ota);

    if(ret == ESP_OK)
    {
        *ota_return_bool = 0;

        vTaskSuspend(*ota_return_task);

        ESP_LOGI("ota_state", "Envio OTA realizado com sucesso! Reiniciando o ESP...");
        esp_restart();
    }
    else
    {
        ESP_LOGE("ota_state", "Falha na atualização OTA (Erro: %s). Abortado.", esp_err_to_name(ret));
    }

    vTaskDelete(NULL);
}



void ota_validation_checker(void)
{
    const esp_partition_t *running = esp_ota_get_running_partition();
    esp_ota_img_states_t ota_state;
    
    if (esp_ota_get_state_partition(running, &ota_state) == ESP_OK) {
        if (ota_state == ESP_OTA_IMG_PENDING_VERIFY) 
        {
            ESP_LOGI("ota_debug", "Firmware estável. Confirmando validade!");
            esp_ota_mark_app_valid_cancel_rollback();
        }
    }
}



void ota_start(TaskHandle_t *task, bool *state_connecting)
{
    ota_return_task = task;

    ota_return_bool = state_connecting;


    *ota_return_bool = 1;

    vTaskResume(*ota_return_task);

    xTaskCreate(&ota_task, "atualizacao_ota", 8192, NULL, 5, NULL);
}