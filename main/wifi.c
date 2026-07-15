#include "wifi.h"
#include "mqtt.h"
#include <stdio.h>
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_event.h"

#include "esp_wifi.h"
#include "general_definitions.h"
#include "iterations.h"


static SemaphoreHandle_t wifi_semaphore; //Semáforo para sincronização da conexão Wi-fi

// POSSO CRIAR UMA STRUCT COM OS PONTEIROS !

TaskHandle_t* wifi_return_task;

esp_mqtt_client_handle_t* mqtt_client_extern;

bool* wifi_return_bool;

bool* mqtt_bool_extern;

bool* ota_bool_extern;

//SemaphoreHandle_t* wifi_return_semaphore;

//SemaphoreHandle_t* mqtt_semaphore_extern;

//função handle para tratamento dos eventos Wi-fi
static void wifi_task_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data)
{

    if(event_base == WIFI_EVENT)
    {
        if(event_id == WIFI_EVENT_STA_START)
        {
            ESP_LOGW("wifi_debug", "Tentativa de primeira conexão...");
            /*
            if(esp_wifi_connect() == ESP_OK)
            {
                vSemaphoreDelete(*wifi_return_semaphore); //não para a task sozinho            //MOVER PARA O HANDLE WIFI DE EVENTO DE DESCONEXÃO
                vTaskDelete(*wifi_return_task); //necessário deletar a task

            }; //estabelecendo a conexão inicial
            */
            esp_wifi_connect(); //ESP_OK retornado apenas por ter autorizado o chip de conexão
        }

        if(event_id == WIFI_EVENT_STA_DISCONNECTED)
        {
            ESP_LOGW("wifi_debug", "Conexão perdida! Tentando conectar...");

            if(*mqtt_client_extern != NULL) esp_mqtt_client_stop(*mqtt_client_extern); //Parando o MQTT para dar prioridade à conexão Wi-Fi

            *wifi_return_bool = 1; //volta com a iteração do wifi na máquina de estados

            vTaskResume(*wifi_return_task);
            /*
            if(esp_wifi_connect() == ESP_OK)
            {
                vSemaphoreDelete(*wifi_return_semaphore); //não para a task sozinho            
                vTaskDelete(*wifi_return_task); //necessário deletar a task

            }; //tentando reconectar em caso de desconexão
            */
            vTaskDelay(2000 / portTICK_PERIOD_MS); //atraso para que tudo esteja nos conformes

            esp_wifi_connect(); //ESP_OK retornado apenas por ter autorizado o chip de conexão
        }

        if(event_id == WIFI_EVENT_STA_CONNECTED)
        {
            ESP_LOGW("wifi_debug", "Conexão Wi-Fi estabelecida!");

            vTaskSuspend(*wifi_return_task);

            *wifi_return_bool = 0; //desativa a iteração wifi na máquina de estados

            //xSemaphoreGive(*wifi_return_semaphore);

            gpio_set_level(BLINK_GPIO, 0);

            //vTaskDelay(2000 / portTICK_PERIOD_MS);
        }
    }

    else if(event_base ==  IP_EVENT && event_id == IP_EVENT_STA_GOT_IP)
    {
        ESP_LOGI("ip_debug", "IP INICIADO!");

        initialize_mqtt(wifi_return_task, mqtt_client_extern, mqtt_bool_extern, ota_bool_extern); //MQTT só inicializa quando a conexão Wi-Fi e o recebimento do IP estão estabelecidos

        xSemaphoreGive(wifi_semaphore);
    } 

}



void initialize_wifi_nvs(iterations *object) //função só vai rodar uma vez
{
    wifi_return_task = &object->wifi_mqtt_handle;

    mqtt_client_extern = &object->mqtt_client;

    wifi_return_bool = &object->connecting_wifi;

    mqtt_bool_extern = &object->connecting_mqtt;

    ota_bool_extern = &object->starting_ota;

    printf("\n Wi-Fi na função WIFI: ");
    printf("%p \n", wifi_return_bool);

    printf("\n Mqtt na função WIFI: ");
    printf("%p \n\n", mqtt_bool_extern);

    //mqtt_semaphore_extern = &object->mqtt;

    //wifi_return_semaphore = &object->wifi;

    *wifi_return_bool = 1; //alterando a iteração do wifi para verdadeiro
    vTaskResume(*wifi_return_task);


    // ======= Inicialização o NVS =======

   esp_err_t nvs_error = nvs_flash_init();
    if(nvs_error != ESP_OK)
    {
        nvs_flash_erase();
        nvs_flash_init();
        ESP_LOGD("nvs_error", "O NVS passou por aqui");
    }


    // ======= Inicialização STA do WIFI =======

    //inicializa a pilha de rede e o loop de eventos
    wifi_semaphore = xSemaphoreCreateBinary();

    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_sta();

    //confugurando o wifi com parâmetros padronizados
    wifi_init_config_t wifi_init_config = WIFI_INIT_CONFIG_DEFAULT();
    esp_err_t wifi_error = esp_wifi_init(&wifi_init_config);

    if(wifi_error == ESP_OK) ESP_LOGI("wifi_error", "INICIALIZAÇÃO WI-FI REALIZADA COM SUCESSO");
    else ESP_LOGW("wifi_error", "FALHA NA INICIALIZAÇÃO WI-FI");

    //pode substituir por ESP_ERROR_CHECK(esp_wifi_init(&wifi_config))


    //registrando o handler para os eventos de IP e de Wi-fi
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_task_handler, NULL, NULL));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &wifi_task_handler, NULL, NULL));

    //configurando o Wi-fi e definindo o ID e Senha 
    wifi_config_t wifi_config = {
        .sta = {
            .ssid = WIFI_SSID,
            .password = WIFI_PASSWORD,
            .threshold.authmode = WIFI_AUTH_WPA2_PSK,
        },
    };

    //definindo o Wi-fi como modo de estação (STA) e aplicando a configuração da rede
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    xSemaphoreTake(wifi_semaphore, portMAX_DELAY);
}
