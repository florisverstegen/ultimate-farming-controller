# Ultimate Farming Controller

DIY game controller / button box voor Farming Simulator 25. Doel: ~80+ knoppen,
een aantal rotary encoders en 4 analoge assen, modulair opgebouwd uit losse
paneeltjes die met stekkers aan de hoofdunit hangen.

## Hardware

- **Microcontroller:** Raspberry Pi Pico (RP2040); Pico 2 (RP2350) wordt ook ondersteund
- **Knoppen:** keten van 74HC165-schuifregisters (parallel-in, serial-out)
  - Huidige testopstelling: 2 losse 74HC165 (DIP) op een breadboard = 16 ingangen
  - Eindsituatie: 74HC165-borden van 24 ingangen (3 chips) met PH2.0-connectoren,
    1 bord per module, doorgelust met 5-polige PH2.0-kabels
  - Knoppen schakelen naar GND, 10k pull-up naar 3,3 V -> ingedrukt = LOW
- **Analoge assen (later):** ADS1115 (16-bit, I2C) met B10K-potmeters
- **Encoders (later):** EC11, A/B rechtstreeks op Pico-GPIO (uitlezen via PIO),
  drukknop via schuifregister
- **Verlichting:** knoppen met 12 V-led, apart 12 V-circuit met ledstripdimmer.
  Staat los van de Pico; 12 V mag NOOIT op een Pico- of schuifregisterpin komen.

### Belangrijk

- Alles op de logicakant draait op **3,3 V** (Pico 3V3-pin). De Pico is niet 5 V-tolerant.
- De 74HC165-borden labelen hun voedingspin als "5V": daar gaat 3,3 V op.

## Pinverdeling

| Pico pin | Functie | 74HC165 pin |
|---|---|---|
| GP2 | LOAD (SH/LD) | 1, alle chips |
| GP3 | CLOCK (CLK) | 2, alle chips |
| GP4 | DATA (QH van chip 1) | 9 van chip 1 |
| 3V3 | voeding | 16 |
| GND | massa | 8, en 15 (CLK INH) |

Ketenvolgorde: chip 1 QH -> Pico, chip 1 SER (pin 10) <- chip 2 QH, laatste chip SER -> GND.

Bitvolgorde in de code: bit 0..7 = chip 1 ingang H..A, bit 8..15 = chip 2 ingang H..A.

## Commando's

```bash
pio run                    # compileren (Pico)
pio run -e pico2           # compileren voor Pico 2
pio run -t upload          # compileren + uploaden
pio device monitor         # seriele monitor (115200 baud)
pio run -t upload && pio device monitor   # alles in een keer
```

Eerste upload: BOOTSEL ingedrukt houden tijdens het aansluiten van de Pico.

## Werkwijze / afspraken

- Stapsgewijs bouwen, elke stap eerst valideren via de seriele monitor.
- Commit per werkende stap.
- Roadmap:
  1. [ ] Schuifregisterketen uitlezen en printen (huidige stap, `src/main.cpp`)
  2. [ ] Encoders via PIO
  3. [ ] ADS1115 + potmeters
  4. [ ] USB HID: meerdere virtuele gamecontrollers (max ~32 knoppen per stuk
     i.v.m. games die niet meer lezen), via Adafruit TinyUSB
  5. [ ] Configuratie: mapping van fysieke ingangen naar knopnummers per module
- Code-commentaar mag Nederlands of Engels; houd het consistent per bestand.
