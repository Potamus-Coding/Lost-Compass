# Known Issues

Documented hardware quirks, firmware gotchas, and development decisions worth remembering. Recorded here so they don't have to be rediscovered.

---

## Hardware

### CCW jitter asymmetry
The TS90D servo exhibits more jitter in the counter-clockwise direction than clockwise. This is a hardware characteristic of the specific unit and is not correctable in software. It is accepted as a known artefact. The `ccwBias` dead-reckoning multiplier compensates the *model* for the asymmetry but does not reduce the physical jitter.

### Servo stop point at 1450µs
The TS90D's deadband centre is at 1450µs rather than the standard 1500µs. Commands at 1500µs will cause slow creep. All speed constants in the firmware are defined relative to the 1450µs stop point.

### Servo direction inversion
The physical CW/CCW direction of this servo is inverted relative to the expected convention. The CW and CCW pulse width labels in the firmware are swapped accordingly. If the servo is replaced, verify direction and update labels if necessary.

---

## Firmware

### Non-ASCII characters break JavaScript in PROGMEM strings
The Arduino IDE preprocessor scans inside PROGMEM raw string literals. Non-ASCII characters embedded in HTML/JS — including em-dashes (`—`), ellipses (`…`), and Unicode symbols (`▶`) — cause silent JavaScript failures at runtime. All HTML and JS must use only ASCII characters and HTML entities (`&mdash;`, `&hellip;`, etc.).

**Resolution:** HTML and JavaScript are stored in separate `.h` header files (`html_page.h`, `config_page.h`) and all non-ASCII characters replaced with HTML entities throughout.

### Arduino IDE misidentifies JS `function` declarations as C++ prototypes
When HTML/JS is embedded as a PROGMEM raw string literal directly in the `.ino` file, the Arduino IDE preprocessor scans the string content and misidentifies JavaScript `function` keyword occurrences as C++ function prototype declarations, producing `'function' does not name a type` compile errors.

**Resolution:** All HTML/JS moved to `.h` header files. Do not put substantial HTML or JavaScript back into the `.ino` file as raw string literals.

### `Serial.println()` blocks when no USB host is connected
`Serial.println()` blocks indefinitely when the USB serial buffer fills with no reader (Serial monitor closed / USB disconnected). This causes the sketch to hang.

**Resolution:** Add `Serial.setTxTimeoutMs(0)` immediately after `Serial.begin()` in `setup()`.

### WiFi requires staged teardown and reinitialisation after light sleep
On the ESP32-C3, calling `WiFi.softAP()` directly after waking from light sleep fails silently — the AP appears to start but does not serve connections. A full teardown and staged restart with delays between each step is required.

**Required wake sequence:**
```cpp
WiFi.disconnect(true);
WiFi.mode(WIFI_OFF);
delay(100);
WiFi.mode(WIFI_AP);
delay(100);
WiFi.softAP(SSID, PASSWORD);
delay(100);
server.begin();
```

### iOS Safari requires specific handling
Several standard approaches fail silently on iOS Safari:

| Issue | Resolution |
|---|---|
| `fetch()` AJAX calls fail when mobile data is active | Replace with `XMLHttpRequest`; use absolute URLs via `window.location.hostname` |
| SVG `transform-origin` ignored | Add `transform-box: fill-box; transform-origin: center` to SVG element CSS |
| CSS custom properties set via JavaScript don't resolve | Use hex colour values directly instead of `var(--token)` references |

---

## Dead-reckoning model

### `dialBearing` drift
The dead-reckoning model accumulates error over time, particularly across many mode transitions or after extended Erratic Spin sessions. The **Initialise Dial** button on the config page resets `dialBearing` to 0.0 — use this whenever the physical needle and model position visibly diverge. The frequency of this depends on `cwBias`/`ccwBias` tuning.

### Free Needle wobble and drift
Early firmware implemented a random wobble in Free Needle mode to give the needle a more organic feel. This caused dead-reckoning drift because `dialBearing` was reset to `currentHeading` after each wobble, preventing corrections from accumulating. The random wobble was replaced with a deterministic alternating wobble (`static bool wobbleCW`) that guarantees zero net drift across pairs of movements.
