# ESP32-Quad-nRF24L01-Plus-Jammer-Multi-Band-2-4GHz-RF-Controller

This project allows an ESP32 board to control **four nRF24L01+ modules** for RF jamming (Bluetooth jammer, BLE jammer, WiFi jammer, RC jammer, ...) or channel sweeping across multiple 2.4GHz bands. Each module can be independently controlled and jam on its own channel. Modes are selectable via serial commands.

---

## Hardware Schematic (ASCII Art)

```
         +------------------ ESP32 Dev Board ------------------+
         |                                                     |
         |   14 SCK (HSPI) -----------------------------+      |
         |   13 MOSI (HSPI) --------------------------+ |      |
         |   12 MISO (HSPI) <-----------------------+ | |      |
         |                                          | | |      |
         |   26 CE (Mod1) ------+                   | | |      |
         |   15 CSN (Mod1) ----|--- nRF24L01+ #1    | | |      |
         |                     +-- nRF24L01+ #2     | | |      |
         |   27 CE (Mod2) ------+                   | | |      |
         |   25 CSN (Mod2) ----|                    | | |      |
         |                                          | | |      |
         |   18 SCK (VSPI) ----------------------------+       |
         |   23 MOSI (VSPI) -------------------------+ |       |
         |   19 MISO (VSPI) <----------------------+ | |       |
         |                                         | | |       |
         |   4  CE (Mod3) ------+                  | | |       |
         |   2  CSN (Mod3) ----|--- nRF24L01+ #3   | | |       |
         |                      +-- nRF24L01+ #4   | | |       |
         |   16 CE (Mod4) ------+                  | | |       |
         |   17 CSN (Mod4) ----|                   | | |       |
         |                                                     |
         +-----------------------------------------------------+

     All modules: VCC to 3.3V, GND to GND (do NOT use 5V!)
```

---

## Pin Mapping

| ESP32 Pin | nRF24L01+ Module | Function | Bus  |
|-----------|------------------|----------|------|
| GPIO14    | 1,2              | SCK      | HSPI |
| GPIO13    | 1,2              | MOSI     | HSPI |
| GPIO12    | 1,2              | MISO     | HSPI |
| GPIO26    | 1                | CE       | HSPI |
| GPIO15    | 1                | CSN      | HSPI |
| GPIO27    | 2                | CE       | HSPI |
| GPIO25    | 2                | CSN      | HSPI |
| GPIO18    | 3,4              | SCK      | VSPI |
| GPIO23    | 3,4              | MOSI     | VSPI |
| GPIO19    | 3,4              | MISO     | VSPI |
| GPIO4     | 3                | CE       | VSPI |
| GPIO2     | 3                | CSN      | VSPI |
| GPIO16    | 4                | CE       | VSPI |
| GPIO17    | 4                | CSN      | VSPI |

---

## Serial Commands

- `start` – Initialize and activate all 4 jammers
- `stop` – Deactivate all jammers
- `r` – Random Bluetooth hopping mode (odd/even channels)
- `w` – WiFi channels mode
- `b` – BLE advertising channels mode
- `u` – USB wireless channels mode
- `v` – Video streaming channels mode
- `c` – RC toys/drones channels mode
- `f` – Full spectrum mode (all channels)
- `help` or `?` – Show help

---

## Usage

1. Wire up the ESP32 and 4x nRF24L01+ modules as shown above.
2. Flash the provided firmware to your ESP32.
3. Open a serial terminal at 115200 baud.
4. Use the commands above to control the jammers.

---

## Warnings

- **FOR EDUCATIONAL AND TESTING PURPOSES ONLY.** Jamming radio frequencies may be illegal in your country. Always comply with local laws.
- Always power nRF24L01+ modules with **3.3V** (never 5V).
- For best stability, use capacitor filtering for each nRF24L01+ module’s VCC/GND.

---

## License

See [LICENSE.md](LICENSE.md) for license information.
