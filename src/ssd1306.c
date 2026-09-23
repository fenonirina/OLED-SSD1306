#include <string.h>
#include "ssd1306.h"
#include "font5x7.h"
#include "esp_log.h"

static const char *TAG = "SSD1306";

// Handles du bus I2C et du périphérique, gérés en interne par ce module.
static i2c_master_bus_handle_t s_bus_handle = NULL;
static i2c_master_dev_handle_t s_dev_handle = NULL;

// Framebuffer : 1 bit par pixel, organisé en pages de 8 lignes (format natif SSD1306)
#define SSD1306_PAGES (SSD1306_HEIGHT / 8)
static uint8_t s_buffer[SSD1306_WIDTH * SSD1306_PAGES];

// ---- Bas niveau I2C ----

static esp_err_t ssd1306_write_cmd(uint8_t cmd)
{
    uint8_t data[2] = {0x00, cmd}; // Co=0, D/C#=0 -> commande
    return i2c_master_transmit(s_dev_handle, data, sizeof(data), 100);
}

static esp_err_t ssd1306_write_data(const uint8_t *data, size_t len)
{
    // Le buffer envoyé doit être préfixé par l'octet de contrôle 0x40 (D/C#=1)
    uint8_t *buf = malloc(len + 1);
    if (!buf) return ESP_ERR_NO_MEM;
    buf[0] = 0x40;
    memcpy(buf + 1, data, len);
    esp_err_t err = i2c_master_transmit(s_dev_handle, buf, len + 1, 200);
    free(buf);
    return err;
}

static esp_err_t i2c_bus_init(void)
{
    i2c_master_bus_config_t bus_config = {
        .i2c_port = SSD1306_I2C_PORT,
        .sda_io_num = SSD1306_SDA_GPIO,
        .scl_io_num = SSD1306_SCL_GPIO,
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .glitch_ignore_cnt = 7,
        .flags.enable_internal_pullup = true,
    };
    
    esp_err_t err = i2c_new_master_bus(&bus_config, &s_bus_handle);
    if (err != ESP_OK) return err;

    i2c_device_config_t dev_config = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = SSD1306_I2C_ADDR,
        .scl_speed_hz = SSD1306_I2C_FREQ_HZ,
    };
    return i2c_master_bus_add_device(s_bus_handle, &dev_config, &s_dev_handle);
}

// ---- Initialisation du contrôleur SSD1306 ----

esp_err_t ssd1306_init(void)
{
    esp_err_t err = i2c_bus_init();
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Erreur init bus I2C: %s", esp_err_to_name(err));
        return err;
    }

    const uint8_t init_seq[] = {
        0xAE,             // Display OFF
        0xD5, 0x80,       // Clock divide ratio / oscillator freq
        0xA8, SSD1306_HEIGHT - 1, // Multiplex ratio
        0xD3, 0x00,       // Display offset
        0x40,             // Start line = 0
        0x8D, 0x14,       // Charge pump ON
        0x20, 0x00,       // Memory addressing mode = horizontal
        0xA1,             // Segment remap (miroir horizontal)
        0xC8,             // COM scan direction (miroir vertical)
        0xDA, (SSD1306_HEIGHT == 64) ? 0x12 : 0x02, // COM pins config
        0x81, 0x7F,       // Contraste
        0xD9, 0xF1,       // Pre-charge period
        0xDB, 0x40,       // VCOMH deselect level
        0xA4,             // Affiche le contenu de la RAM (pas tout allumé)
        0xA6,             // Mode normal (pas inversé)
        0xAF              // Display ON
    };

    for (size_t i = 0; i < sizeof(init_seq); i++) {
        err = ssd1306_write_cmd(init_seq[i]);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "Erreur commande init 0x%02X: %s", init_seq[i], esp_err_to_name(err));
            return err;
        }
    }

    ssd1306_clear();
    return ssd1306_display();
}

void ssd1306_clear(void)
{
    memset(s_buffer, 0x00, sizeof(s_buffer));
}

void ssd1306_set_pixel(int x, int y, bool on)
{
    if (x < 0 || x >= SSD1306_WIDTH || y < 0 || y >= SSD1306_HEIGHT) return;

    uint16_t idx = x + (y / 8) * SSD1306_WIDTH;
    uint8_t bit = y % 8;

    if (on) {
        s_buffer[idx] |= (1 << bit);
    } else {
        s_buffer[idx] &= ~(1 << bit);
    }
}

static void draw_char(int x, int y, char c)
{
    if (c < FONT5X7_FIRST_CHAR || c > FONT5X7_LAST_CHAR) c = '?';
    const uint8_t *glyph = font5x7[c - FONT5X7_FIRST_CHAR];

    for (int col = 0; col < 5; col++) {
        uint8_t line = glyph[col];
        for (int row = 0; row < 8; row++) {
            ssd1306_set_pixel(x + col, y + row, (line >> row) & 0x01);
        }
    }
}

void ssd1306_draw_string(int x, int y, const char *str)
{
    int cursor_x = x;
    while (*str) {
        draw_char(cursor_x, y, *str);
        cursor_x += 6; // 5 px de glyphe + 1 px d'espacement
        if (cursor_x + 5 >= SSD1306_WIDTH) break; // pas de retour à la ligne automatique
        str++;
    }
}

void ssd1306_draw_bitmap(int x, int y, const uint8_t *bitmap, int w, int h)
{
    int byte_width = (w + 7) / 8; // nombre d'octets par ligne, arrondi au supérieur

    for (int row = 0; row < h; row++) {
        for (int col = 0; col < w; col++) {
            uint8_t byte = bitmap[row * byte_width + (col / 8)];
            bool on = (byte >> (7 - (col % 8))) & 0x01;
            ssd1306_set_pixel(x + col, y + row, on);
        }
    }
}

esp_err_t ssd1306_set_contrast(uint8_t level)
{
    esp_err_t err = ssd1306_write_cmd(0x81); // Set Contrast Control
    if (err != ESP_OK) return err;
    return ssd1306_write_cmd(level);
}

esp_err_t ssd1306_invert(bool inverted)
{
    return ssd1306_write_cmd(inverted ? 0xA7 : 0xA6); // Inverse / Normal display
}

esp_err_t ssd1306_display(void)
{
    // Définit la fenêtre d'adressage complète (colonnes + pages)
    esp_err_t err;
    err = ssd1306_write_cmd(0x21); if (err != ESP_OK) return err; // Column address
    err = ssd1306_write_cmd(0);    if (err != ESP_OK) return err;
    err = ssd1306_write_cmd(SSD1306_WIDTH - 1); if (err != ESP_OK) return err;
    err = ssd1306_write_cmd(0x22); if (err != ESP_OK) return err; // Page address
    err = ssd1306_write_cmd(0);    if (err != ESP_OK) return err;
    err = ssd1306_write_cmd(SSD1306_PAGES - 1); if (err != ESP_OK) return err;

    return ssd1306_write_data(s_buffer, sizeof(s_buffer));
}