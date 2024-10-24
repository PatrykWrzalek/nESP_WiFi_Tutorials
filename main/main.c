#include <main.h> // dla kontenera na Linux ("/") separato jest inny niż dla Windows'a ("\")
#include <init_fun.h>

void system_init(void *arg);
void workStatus(void *arg);

/******************************************************************************
 * FunctionName : app_main
 * Description  : entry of app application, init app function here.
 * Users can use tasks with priorities from 1 to 9
 * (priority of the freeRTOS timer is 2).
 * Parameters   : none
 * Returns      : none
 *******************************************************************************/
void app_main(void)
{
    xTaskCreate(system_init, "Init_system", 2048, NULL, 9, NULL);

    xTaskCreate(workStatus, "Status", 1024, NULL, 1, NULL);
}

/******************************************************************************
 * FunctionName : system_init
 * Description  : main initializes task.
 * Parameters   : none
 * Returns      : none
 *******************************************************************************/
void system_init(void *arg)
{
    gpio_init();

    vTaskDelay(5000 / portTICK_RATE_MS);
    esp_AP_init();

    uint16_t siz_dat = uxTaskGetStackHighWaterMark(NULL); // Info for debug
    ESP_LOGI(TAG, "Task2 stack: %d\r\n", siz_dat);
    vTaskDelete(NULL);
}

/******************************************************************************
 * FunctionName : workStatus
 * Description  : on board LED blinking every 1s for 1s.
 * Parameters   : none
 * Returns      : none
 *******************************************************************************/
void workStatus(void *arg)
{
    while (true)
    {
        gpio_set_level(2, 0);
        vTaskDelay(1000 / portTICK_RATE_MS);
        gpio_set_level(2, 1);
        vTaskDelay(1000 / portTICK_RATE_MS);
    }
    vTaskDelete(NULL);
}