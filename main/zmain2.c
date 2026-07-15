#include <stdio.h>
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

// == Com base nas prioridades de cada task, o LED permanece acesso ou apagado ==
#define TASK1_PRIORITY 1
#define TASK2_PRIORITY 2

void Task1(void *param)
{
    while(1)
    {
        ESP_LOGW("Task1","TASK 1 rodando...");
        gpio_set_level(GPIO_NUM_8, 0);
        vTaskDelay(5000 / portTICK_PERIOD_MS);
    }
}

void Task2(void *param)
{
    while(1)
    {
        ESP_LOGE("Task2", "TASK 2 rodando......");
        gpio_set_level(GPIO_NUM_8, 1);
        vTaskDelay(5000 / portTICK_PERIOD_MS);
        //ESP_LOGE("Task2", "2.0");
        //vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}




void app_main(void)
{
    ESP_LOGI("App_main", "Secão principal iniciada!");

    

    xTaskCreatePinnedToCore(Task1, "tarefa1_core1", 2048, NULL, TASK1_PRIORITY, NULL, 0);
    xTaskCreatePinnedToCore(Task2, "tarefa1_core2", 2048, NULL, TASK2_PRIORITY, NULL, 0);

    gpio_reset_pin(GPIO_NUM_8);
    gpio_set_direction(GPIO_NUM_8, GPIO_MODE_OUTPUT);
}
