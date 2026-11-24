# Ultimate Farming Controller

Controller for Farming Simulator and in the future Ultimate Farmer.

Modulaire DIY button box voor Farming Simulator 25, gebouwd rond een Raspberry Pi Pico
met 74HC165-schuifregisters, rotary encoders en analoge hendels.

## Snel starten

1. Installeer [VS Code](https://code.visualstudio.com/) met de extensie **PlatformIO IDE**.
2. Clone deze repo en open de map in VS Code.
3. Sluit de Pico aan (eerste keer met BOOTSEL ingedrukt).
4. Upload en open de seriele monitor:

   ```bash
   pio run -t upload && pio device monitor
   ```

Zie [CLAUDE.md](CLAUDE.md) voor hardware, pinverdeling en roadmap.
