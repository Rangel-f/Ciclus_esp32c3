#include <stdio.h>
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "nvs_flash.h"

#include "general_definitions.h"
#include "iterations.h"


//static SemaphoreHandle_t semaphore_conexao_wifi;
//static SemaphoreHandle_t semaphore_conexao_mqtt;
static iterations itr; //=>(variável CÓPIA)<= da estrutura

/*
void fase_de_conexao(void *param)
{
    while(xSemaphoreTake(itr.wifi, 0) != pdTRUE)
    {
        ESP_LOGW("wifi_state", "Aguardando conexão Wi-Fi...");
        //ESP_LOGW("wifi_state", ".");
        //printf("%s", ".");
        gpio_set_level(BLINK_GPIO, 1);
        vTaskDelay(250 / portTICK_PERIOD_MS);
        gpio_set_level(BLINK_GPIO, 0);
        vTaskDelay(250 / portTICK_PERIOD_MS);

    }

    while(xSemaphoreTake(itr.mqtt, 0) != pdTRUE)
    {
        ESP_LOGW("mqtt_state", "Aguardando conexão MQTT...");
        //ESP_LOGW("mqtt_state", ".");
        //printf("%s", ".");
        gpio_set_level(BLINK_GPIO, 1);
        vTaskDelay(750 / portTICK_PERIOD_MS);
        gpio_set_level(BLINK_GPIO, 0);
        vTaskDelay(750 / portTICK_PERIOD_MS);
    }
}
*/





// ==> MÁQUINA DE ESTADOS <==

void fase_de_conexao(void *param)
{
    while(1)
    {
        bool* wifi_state = &itr.connecting_wifi;
        bool* mqtt_state = &itr.connecting_mqtt;
        bool* ota_state = &itr.starting_ota;

        if(!WIFI_ITERATION && !MQTT_ITERATION && !OTA_ITERATION) vTaskSuspend(itr.wifi_mqtt_handle); //suspende a task caso não haja iterações habilitadas

        else if(WIFI_ITERATION && *wifi_state)
        {
            ESP_LOGW("wifi_state", "Aguardando conexão Wi-Fi...");
            //ESP_LOGW("wifi_state", ".");
            //printf("%s", ".");
            gpio_set_level(BLINK_GPIO, 1);
            vTaskDelay(250 / portTICK_PERIOD_MS);
            gpio_set_level(BLINK_GPIO, 0);
            vTaskDelay(250 / portTICK_PERIOD_MS);
        }

        else if(MQTT_ITERATION && *mqtt_state)
        {
            ESP_LOGW("mqtt_state", "Aguardando conexão MQTT...");
            //ESP_LOGW("mqtt_state", ".");
            //printf("%s", ".");
            gpio_set_level(BLINK_GPIO, 1);
            vTaskDelay(750 / portTICK_PERIOD_MS);
            gpio_set_level(BLINK_GPIO, 0);
            vTaskDelay(750 / portTICK_PERIOD_MS);
        }

        else if(OTA_ITERATION && *ota_state)
        {
            ESP_LOGW("ota_state", "Fazendo atualização OTA...");
            //ESP_LOGW("ota_state", ".");
            //printf("%s", ".");
            gpio_set_level(BLINK_GPIO, 0);
            vTaskDelay(100 / portTICK_PERIOD_MS);
            gpio_set_level(BLINK_GPIO, 1);
            vTaskDelay(1000 / portTICK_PERIOD_MS);
        } 

        else
        {
            ESP_LOGI("iteration_debug", "ELSE");

            printf("%s", "Wi-Fi_state: ");
            printf("%p \n", wifi_state);

            printf("%s", "Mqtt_state: ");
            printf("%p \n", mqtt_state);

            printf("%s", "OTA_state: ");
            printf("%p \n", ota_state);

            vTaskDelay(1000 / portTICK_PERIOD_MS); //para a task não estourar o watchdogs caso não esteja com nenhuma iteração
        } 
    }
}


iterations* initialize_iterations()
{
    gpio_reset_pin(BLINK_GPIO);
    gpio_set_direction(BLINK_GPIO, GPIO_MODE_OUTPUT);
    gpio_set_level(BLINK_GPIO, 0);

/*
    if(wifi_param) 
    {
        itr.wifi = xSemaphoreCreateBinary();
    }
    if(mqtt_param) 
    {
        itr.mqtt = xSemaphoreCreateBinary();
    }
*/
    //valores de iteração começam zerados
    itr.wifi_mqtt_handle = NULL;
    itr.mqtt_client = NULL;

    itr.connecting_wifi = 0;
    itr.connecting_mqtt = 0;
    itr.starting_ota = 0;

    //static volatile bool *vetor_from_address[2] = {&itr.connecting_wifi, &itr.connecting_mqtt};

    //criação de uma task de blink para indicar que o esp está na fase de conexão dos componentes (wifi e mqtt)
    xTaskCreate(fase_de_conexao, "blink_de_conexao", 1000, NULL, 1, &itr.wifi_mqtt_handle);

    
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    vTaskSuspend(itr.wifi_mqtt_handle); //Task inicialmente suspensa

    return &itr;
}