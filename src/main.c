#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "ssd1306.h"

static const char *TAG = "MAIN";

// Exemple d'icône 16x16 (coeur plein) au format "image2cpp" : MSB = pixel
// le plus à gauche, 2 octets par ligne ((16+7)/8 = 2).
static const uint8_t icon_heart_16x16[] = {
    0x03,0xC0, 0x0F,0xF0, 0x1F,0xF8, 0x3F,0xFC,
    0x7F,0xFE, 0x7F,0xFE, 0x7F,0xFE, 0x7F,0xFE,
    0x3F,0xFC, 0x3F,0xFC, 0x1F,0xF8, 0x0F,0xF0,
    0x07,0xE0, 0x03,0xC0, 0x01,0x80, 0x00,0x00
};

void app_main(void)
{
    esp_err_t err = ssd1306_init();
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Echec init SSD1306: %s", esp_err_to_name(err));
        return;
    }

    ssd1306_clear();
    ssd1306_draw_string(0, 0, "Hello esp32!");     // minuscules OK
    ssd1306_draw_string(0, 16, "SSD1306 OLED");
    ssd1306_draw_bitmap(100, 0, icon_heart_16x16, 16, 16); // image dans le coin
    ssd1306_display();

    vTaskDelay(pdMS_TO_TICKS(2000));
    ssd1306_invert(true);   // "inverse les couleurs" : fond blanc, pixels noirs
    vTaskDelay(pdMS_TO_TICKS(1000));
    ssd1306_invert(false);
    ssd1306_set_contrast(0x20); // écran plus sombre

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