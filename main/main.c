#include <stdio.h>
#include <gpio.h>
#include <FreeRTOS.h>
#include <task.h> // dla kontenera na Linux ("/") separato jest inny niż dla Windows'a ("\")

void Task1(void *arg)
{
    while (1)
    {
        gpio_set_level(2, 0);
        vTaskDelay(1000 / portTICK_RATE_MS);
        gpio_set_level(2, 1);
        vTaskDelay(1000 / portTICK_RATE_MS);
    }
    vTaskDelete(NULL);
}

void app_main(void)
{
    gpio_config_t io_conf;
    io_conf.pin_bit_mask = ((1 << 2));
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io_conf.intr_type = GPIO_INTR_DISABLE;

    gpio_config(&io_conf);

    xTaskCreate(Task1, "Task1", 1024, NULL, 1, NULL);
}