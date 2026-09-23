/*
* @file ssd1306.h
* @brief Interface pour le contrôleur SSD1306 via I2C
* @author Fenonirina
* @date 2024-06-15
*/

#ifndef SSD1306_H
#define SSD1306_H

#include <stdint.h>
#include <stdbool.h>
#include "driver/i2c_master.h"
#include "esp_err.h"

// ---- Configuration matérielle ----
#define SSD1306_I2C_PORT        I2C_NUM_0
#define SSD1306_SDA_GPIO        21
#define SSD1306_SCL_GPIO        22
#define SSD1306_I2C_FREQ_HZ     400000
#define SSD1306_I2C_ADDR        0x3C   // 0x3D sur certains modules

#define SSD1306_WIDTH           128
#define SSD1306_HEIGHT          64     // Mets 32 si ton écran est un 128x32

/*
 * @brief Initialise le contrôleur SSD1306
 * @return ESP_OK en cas de succès
 */
esp_err_t ssd1306_init(void);

/**
 * @brief Efface l'écran
 */
void ssd1306_clear(void);

/**
 * @brief Définit un pixel sur l'écran
 * @param x Position horizontale
 * @param y Position verticale
 * @param on true = pixel allumé, false = pixel éteint
 */
void ssd1306_set_pixel(int x, int y, bool on);

/**
 * @brief Dessine une chaîne de caractères sur l'écran
 * @param x Position horizontale
 * @param y Position verticale
 * @param str Pointeur vers la chaîne de caractères
 */
void ssd1306_draw_string(int x, int y, const char *str);

/**
 * @brief Affiche le contenu de la mémoire tampon sur l'écran
 * @return ESP_OK en cas de succès
 */
esp_err_t ssd1306_display(void);

/*
 * @brief Dessine un bitmap sur l'écran
 * @param x Position horizontale
 * @param y Position verticale
 * @param bitmap Pointeur vers le tableau de données du bitmap
 * @param w Largeur du bitmap
 * @param h Hauteur du bitmap
 */
void ssd1306_draw_bitmap(int x, int y, const uint8_t *bitmap, int w, int h);

/**
 * @brief Définit le contraste de l'écran
 * @param level Niveau de contraste (0x00 - 0xFF)
 * @return ESP_OK en cas de succès
 */
esp_err_t ssd1306_set_contrast(uint8_t level);

/**
 * @brief Inverse le mode vidéo
 * @param inverted true = fond blanc / pixels noirs
 * @return ESP_OK en cas de succès
 */
esp_err_t ssd1306_invert(bool inverted);

#endif