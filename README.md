# OLED SSD1306 sur ESP32

Petit projet PlatformIO/ESP-IDF permettant d'afficher du texte, un bitmap et un compteur sur un écran OLED SSD1306 128x64 via le bus I2C.

## Description

Ce projet utilise un module OLED SSD1306 connecté à un ESP32. L'application affiche :

- un message d'accueil,
- le texte "SSD1306 OLED",
- un petit cœur 16x16,
- puis un compteur qui s'incrémente toutes les secondes.

Le code est organisé autour du driver `ssd1306.c` et de son interface `ssd1306.h`.

## Matériel requis

- ESP32 DevKit (ou carte compatible)
- Écran OLED SSD1306 128x64
- Fils de câblage
- Alimentation 3.3V ou via la carte ESP32

## Branchement I2C

Le projet est configuré pour utiliser les broches suivantes :

- SDA : GPIO 21
- SCL : GPIO 22
- VCC : 3.3V
- GND : GND

Adresse I2C par défaut : `0x3C`

> Sur certains modules, l'adresse peut être `0x3D`. Si votre écran reste noir ou ne répond pas, vérifiez l'adresse du module.

## Configuration logicielle

Le fichier `platformio.ini` configure le projet pour :

- plateforme : `espressif32`
- carte : `esp32dev`
- framework : `espidf`
- vitesse de moniteur : `115200`

## Démarrage rapide

1. Ouvrir le projet dans VS Code avec PlatformIO.
2. Vérifier que le bon port série est détecté.
3. Compiler le projet :

```bash
pio run
```

4. Télécharger le firmware sur l'ESP32 :

```bash
pio run -t upload
```

5. Ouvrir le moniteur série :

```bash
pio device monitor
```

## Structure du projet

```text
OLED-SSD1306/
├── src/
│   ├── main.c
│   ├── ssd1306.c
│   ├── ssd1306.h
│   └── font5x7.h
├── include/
├── lib/
├── CMakeLists.txt
├── platformio.ini
├── sdkconfig
├── README.md
└── ...
```

## Points importants

- La définition de la taille de l'écran est dans `src/ssd1306.h`.
- Pour un écran 128x32 au lieu de 128x64, il faut modifier la ligne :

```c
#define SSD1306_HEIGHT 64
```

en :

```c
#define SSD1306_HEIGHT 32
```

## Dépannage

### L'écran ne s'affiche pas

- Vérifier la polarité et les connexions SDA/SCL.
- Vérifier l'adresse I2C (`0x3C` ou `0x3D`).
- Vérifier que la broche `SCL` est bien branchée sur GPIO 22.
- Vérifier l'alimentation du module.

### Problème de compilation

- Relancer la commande :

```bash
pio run
```

- Vérifier que les dépendances PlatformIO et l'ESP-IDF sont bien installées.

## Licence

Projet personnel / expérimental, à adapter selon vos besoins.
