# Bill of Materials — The Lost Compass

All components used in the final build. Items discussed during development but not present in the finished build are noted at the bottom.

---

## Electronics

| Component | Description | Qty | Notes |
|---|---|---|---|
| Seeed XIAO ESP32-C3 | Microcontroller. WiFi AP, I2C host, ADC, servo PWM | 1 | USB-C charging onboard. LiPo connects to BAT+ JST |
| QMC5883L / QMC5883P | Magnetometer module | 1 | Either variant works. Connected via default I2C (SDA=D4, SCL=D5), powered from 3V3 |
| TS90D | Continuous rotation servo. Drives compass needle | 1 | Stop point at 1450µs (not standard 1500µs) — see calibration notes |
| LiPo battery | Main power supply | 1 | **Capacity: [confirm mAh]** |
| Slide switch (SPST) | Power switch. Interrupts battery positive wire | 1 | Sits on the positive line between LiPo+ and XIAO BAT+ |
| 3-pin JST connector | Battery harness connector | 1 | Carries: switched battery+, GND, and voltage divider midpoint. **[Confirm pitch: PH 2.0mm / XH 2.54mm?]** |
| Resistor 200kΩ | Voltage divider for battery ADC, upper leg | 1 | Battery+ → resistor → D1 node |
| Resistor 200kΩ | Voltage divider for battery ADC, lower leg | 1 | D1 node → GND |
| Tactile button | Lid-actuated sleep/wake trigger | 1 | Pressed by lid closing. Uses internal pull-down on XIAO |

---

## Pin Assignments

| Function | XIAO Pin | GPIO | Notes |
|---|---|---|---|
| Servo signal | D2 | GPIO4 | PWM output via ESP32Servo library |
| Sleep/wake button | D3 | GPIO5 | Active HIGH (lid closed). Internal pull-down enabled |
| Battery ADC | D1 | GPIO3 | Reads voltage divider midpoint. Multiply reading × 2 for actual battery voltage |
| I2C SDA (magnetometer) | D4 | GPIO6 | Default I2C bus |
| I2C SCL (magnetometer) | D5 | GPIO7 | Default I2C bus |

---

## Firmware Files

| File | Purpose |
|---|---|
| `lost_compass.ino` | Main sketch. WiFi AP, web server, mode logic, servo control |
| `html_page.h` | Main control page HTML/CSS/JS, stored in PROGMEM |
| `config_page.h` | Calibration/config page HTML/CSS/JS, stored in PROGMEM |
| `compass_cal.ino` | Standalone calibration sketch. Runs servo laps to measure empirical deg/ms constants |

---

## Libraries (Arduino IDE)

| Library | Purpose |
|---|---|
| ESP32Servo | Servo PWM control on ESP32 |
| Arduino Preferences | NVS flash storage for heading offset, bias values |
| Wire (built-in) | I2C communication with magnetometer (direct register access, no mag library) |
| WiFi / WebServer (built-in) | AP mode and HTTP server |

---

## Not Used in Final Build

These components were considered during design but are not present in the finished hardware:

- **1N5819 Schottky diode** — proposed for a battery charging topology that would allow charging with the MCU powered down. Wiring was simplified to not require it.
- **TP4056 charger module** — proposed as a standalone charging circuit before settling on the XIAO's onboard USB-C LiPo charger.

---

## Items to Confirm Before Publishing

- [ ] LiPo battery capacity (mAh) and physical form factor
- [ ] JST connector pitch (2.0mm PH or 2.54mm XH)
- [ ] Confirm servo pin is D2 — sketch may have been updated since conversations
- [ ] Confirm sleep/wake pin is D3
- [ ] Any link to the 3D printed enclosure (Printables / Thingiverse / local STL)
