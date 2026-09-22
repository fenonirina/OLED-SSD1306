#ifndef SSD1306_H
#define SSD1306_H

#include <stdint.h>
#include <stdbool.h>
#include "driver/i2c.h"
#include "esp_err.h"

// ---- Configuration matérielle (à adapter à ton câblage) ----
#define SSD1306_I2C_PORT        I2C_NUM_0
#define SSD1306_SDA_GPIO        21
#define SSD1306_SCL_GPIO        22
#define SSD1306_I2C_FREQ_HZ     400000
#define SSD1306_I2C_ADDR        0x3C   // 0x3D sur certains modules

#define SSD1306_WIDTH           128
#define SSD1306_HEIGHT          64     // Mets 32 si ton écran est un 128x32

esp_err_t ssd1306_init(void);
void ssd1306_clear(void);
void ssd1306_set_pixel(int x, int y, bool on);
void ssd1306_draw_string(int x, int y, const char *str);
esp_err_t ssd1306_display(void);

#endif // SSD1306_H
