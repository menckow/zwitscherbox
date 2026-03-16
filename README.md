# Zwitscherbox (Modular MP3 Player)

Ein modularer, ESP32-basierter MP3-Player, der primär für das Abspielen von Natur- und Umgebungsgeräuschen (wie Vogelgezwitscher) konzipiert ist. Die Wiedergabe wird durch Bewegung (PIR-Sensor) ausgelöst.

## Features

- **Bewegungssteuerung:** Automatisches Abspielen von Sounds bei erkannter Bewegung via PIR-Sensor.
- **Mehrere Sound-Sets:** Unterstützung für verschiedene Ordner auf der SD-Karte.
- **Einfache Bedienung:** 
  - **Taster:** Wechselt zum nächsten Sound-Ordner (spielt eine `intro.mp3` zur Bestätigung).
  - **Potentiometer:** Stufenlose Lautstärkeregelung.
- **Energiesparend:** Automatischer Wechsel in den **Deep Sleep** nach Inaktivität, um Batterielaufzeit zu maximieren.
- **Zustandsspeicherung:** Merkt sich den zuletzt gewählten Ordner und die Lautstärke über Neustarts hinweg (NVS).
- **Timeout-Schutz:** Maximale Wiedergabedauer pro Auslösung einstellbar.

## Hardware-Anforderungen

Das Projekt ist für das **YB-ESP32-S3-AMP** Board (Revision 2.x oder 3.x) optimiert.

- **MCU:** ESP32-S3
- **Audio:** I2S DAC/Verstärker
- **Sensoren:** 
  - PIR-Sensor (Motion Detection)
  - Potentiometer (Analog)
  - Push-Button (Digital)
- **Speicher:** Micro-SD Karte (SPI)

### Pin-Konfiguration (Standard)

| Komponente | Pin (ESP32-S3) |
|------------|----------------|
| POT_PIN    | GPIO 4         |
| PIR_PIN    | GPIO 18        |
| BUTTON_PIN | GPIO 17        |
| SD_CS      | Standard (SS)  |

*(Die I2S-Pins sind board-spezifisch vorkonfiguriert).*

## Software & Bibliotheken

Dieses Projekt nutzt [PlatformIO](https://platformio.org/).

Wichtige Bibliotheken:
- **ESP32-audioI2S:** Für die hochwertige Audiowiedergabe über I2S.
- **Preferences:** Zur dauerhaften Speicherung von Einstellungen.
- **SD:** Zugriff auf das Dateisystem der Speicherkarte.

## Installation

1. Installiere [PlatformIO](https://platformio.org/install) (z.B. als VS Code Extension).
2. Klone dieses Repository.
3. Wähle in der `platformio.ini` das passende Environment (`v2` oder `v3`), passend zu deiner Board-Revision.
4. Kompiliere und lade den Code auf den ESP32 hoch.

## SD-Karten Struktur

Die MP3-Dateien sollten in Ordnern auf der SD-Karte organisiert sein:

```text
SD-Karte/
├── Wald/
│   ├── intro.mp3 (optional, wird beim Ordnerwechsel abgespielt)
│   ├── vogel01.mp3
│   └── vogel02.mp3
├── Meer/
│   ├── intro.mp3
│   └── wellen.mp3
└── ...
```

- Der Player scannt beim Start alle Ordner nach MP3-Dateien.
- Pro Bewegungsauslösung wird eine zufällige Datei aus dem aktuellen Ordner abgespielt.

## Lizenz

Dieses Projekt ist unter der MIT-Lizenz lizenziert.
