#include "mqtt.h"
#include "ota.h"
#include <stdio.h>
#include <string.h>
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_event.h"

#include "general_definitions.h"
#include "iterations.h"



extern const uint8_t emqxsl_ca_crt_start[] asm("_binary_emqxsl_ca_crt_start");
extern const uint8_t emqxsl_ca_crt_end[]   asm("_binary_emqxsl_ca_crt_end");

//SemaphoreHandle_t* mqtt_return_semaphore;

TaskHandle_t* mqtt_return_task;

esp_mqtt_client_handle_t* mqtt_client_local;

bool* mqtt_return_bool;

bool* ota_extern_bool;


static void mqtt_task_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    esp_mqtt_event_handle_t received_data = event_data;

    switch ((esp_mqtt_event_id_t)event_id) //transformando o código em int do evento para o tipo esp_mqtt_event_id_t
    {
    case MQTT_EVENT_CONNECTED:
        
        vTaskSuspend(*mqtt_return_task);

        *mqtt_return_bool = 0;

        //xSemaphoreGive(*mqtt_return_semaphore);
        gpio_set_level(BLINK_GPIO, 1);

        ESP_LOGI("mqtt_debug", "Conectado com sucesso ao broker EMQX!");

        esp_mqtt_client_subscribe(*mqtt_client_local, MQTT_TOPIC_SUBSCRIBE, 1);

        ESP_LOGI("mqtt_debug", "Tópico assinado em: %s", MQTT_TOPIC_SUBSCRIBE);

        char mqtt_connected_payload[50] = "Cliente Ativado: ";
        strcat(mqtt_connected_payload, MQTT_CLIENT_ID);

        esp_mqtt_client_publish(*mqtt_client_local, MQTT_TOPIC_PUBLISH, mqtt_connected_payload, 0, 1, 0);
        break;
    
    case MQTT_EVENT_ERROR:

        ESP_LOGW("mqtt_debug", "Evento de erro!");

        *mqtt_return_bool = 1;

        //printf("%p \n", mqtt_return_bool);

        vTaskResume(*mqtt_return_task);
        
        break;
    
    case MQTT_EVENT_DATA:
        
        if(strncmp(received_data->topic, MQTT_TOPIC_SUBSCRIBE, received_data->topic_len) == 0)
        {
            if(strncmp(received_data->data, "true", received_data->data_len) == 0)
            {
                //ESP_LOGI("MQTT_DATA", "Mensagem recebida: %s", received_data->data);
                ESP_LOGI("esp_state", "Ligando o LED!");

                gpio_set_level(BLINK_GPIO, 0);
            }
            else if(strncmp(received_data->data, "false", received_data->data_len) == 0)
            {
                //ESP_LOGI("MQTT_DATA", "Mensagem recebida: %s", received_data->data);
                ESP_LOGI("esp_state", "Desligando o LED!");

                gpio_set_level(BLINK_GPIO, 1);
            }
            else if(strncmp(received_data->data, "Atualizar_OTA", received_data->data_len) == 0)
            {
                ESP_LOGI("esp_state", "Iniciando atualização OTA...");
                ota_start(mqtt_return_task, ota_extern_bool);
            }
        }

        break;

    default:
        break;
    }
}



void initialize_mqtt(TaskHandle_t *task, esp_mqtt_client_handle_t *client, bool *state_connecting, bool *state_ota)
{

    //mqtt_return_semaphore = &semaforo;
    mqtt_return_task = task;

    mqtt_client_local = client;

    mqtt_return_bool = state_connecting;

    ota_extern_bool = state_ota;

    *mqtt_return_bool = 1;

    printf("\n Mqtt na função MQTT: ");
    printf("%p \n\n", mqtt_return_bool);

    vTaskDelay(2000 / portTICK_PERIOD_MS); //atraso para que tudo fique nos conformes

    vTaskResume(*mqtt_return_task);

    ESP_LOGI("mqtt_debug", "Conectando ao broker EMQX com autenticação pelo ID: %s", MQTT_CLIENT_ID);

    esp_mqtt_client_config_t mqtt_config = 
    {
        .broker.address.uri = MQTT_BROKER_URL,
        .credentials.client_id = MQTT_CLIENT_ID,
        .credentials.username = MQTT_USER,
        .credentials.authentication.password = MQTT_PASSWORD,
        .broker.verification.certificate = (const char *)emqxsl_ca_crt_start,

    };

    // Atribuindo as configurações e iniciando a conexão MQTT
    *mqtt_client_local = esp_mqtt_client_init(&mqtt_config);
    esp_mqtt_client_register_event(*mqtt_client_local, MQTT_EVENT_ANY, mqtt_task_handler, NULL);

    esp_mqtt_client_start(*mqtt_client_local); 
}
