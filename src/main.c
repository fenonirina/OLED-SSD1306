#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "ssd1306.h"

static const char *TAG = "MAIN";

void app_main(void)
{
    esp_err_t err = ssd1306_init();
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Echec init SSD1306: %s", esp_err_to_name(err));
        return;
    }

    ssd1306_clear();
    ssd1306_draw_string(0, 0, "HELLO ESP32!");
    ssd1306_draw_string(0, 16, "SSD1306 OLED");
    ssd1306_draw_string(0, 32, "I2C PLATFORMIO");
    ssd1306_display();

	vTaskDelay(pdMS_TO_TICKS(5000));

    int counter = 0;
    char line[32];
    while (1) {
        ssd1306_clear();
        ssd1306_draw_string(0, 0, "COMPTEUR:");
        snprintf(line, sizeof(line), "%d", counter++);
        ssd1306_draw_string(0, 16, line);
        ssd1306_display();

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
