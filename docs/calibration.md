# Calibration

Technical reference for calibrating The Lost Compass. Covers both the first-time setup procedure and the individual parameters that can be tuned.

---

## Parameters

### `SENSOR_ROTATION` (compile-time constant)

Defined in `lost_compass.ino`. Corrects for the physical angle at which the QMC5883 is mounted inside the housing relative to the compass face. This is a fixed hardware offset — it does not change unless the sensor is remounted.

Set this once after assembly by measuring how far the raw magnetic heading deviates from the compass face's North mark, then encoding that as a constant. It is separate from `headingOffset` intentionally: this corrects hardware, `headingOffset` is user-tunable at runtime.

### `headingOffset` (runtime, persisted to NVS)

User-adjustable heading trim. Exposed via the config page (`/config`) as ±5° and ±15° buttons. Stored in ESP32 NVS flash via the `Preferences` library — survives power cycles.

Use this to fine-tune the virtual needle to read 0° when the device is facing magnetic North. Adjust the software needle on the config page compass rose; do not move the physical needle during this step.

### `cwBias` / `ccwBias` (runtime, persisted to NVS)

Float multipliers applied in `trackMove()` to correct the dead-reckoning model for the servo's asymmetric clockwise and counter-clockwise behaviour. Default 1.0. Exposed via the `/setbias` endpoint and config page controls.

These correct the *model*, not the physical servo movement. If the firmware's estimated needle position drifts away from the physical position over time, adjust these values.

### `stopUs` (servo stop pulse width)

The TS90D's deadband centre sits at **1450µs**, not the standard 1500µs. This is encoded in the firmware constants. If the servo creeps when it should be stationary, this is the value to adjust.

### Speed constants

Empirically measured via `compass_cal.ino`. Asymmetric between directions due to the 1450µs stop point.

| Constant | Value | Direction |
|---|---|---|
| `DEG_PER_MS_FAST_CW` | 0.255 | Clockwise fast |
| `DEG_PER_MS_FAST_CCW` | 0.215 | Counter-clockwise fast |
| `DEG_PER_MS_MED_CW` | 0.148 | Clockwise medium |
| `DEG_PER_MS_MED_CCW` | 0.115 | Counter-clockwise medium |
| `DEG_PER_MS_SLOW_CW` | 0.073 | Clockwise slow |
| `DEG_PER_MS_SLOW_CCW` | 0.060 | Counter-clockwise slow |

These are baked into the firmware. Re-run `compass_cal.ino` if the servo is replaced.

---

## First Commission Procedure

Run this on first assembly, or after any disassembly that disturbs the sensor or needle.

**1. Power on**
Open the sliding base lid and move the power switch to On. Place the compass on a flat surface and open the top display lid.

**2. Connect**
Join the `CompassControl` network. Open `http://192.168.4.1/config`.

**3. Face North**
Using a phone compass as reference, orient the prop to magnetic North. The hinges and dome of the lid should face North.

**4. Tune heading offset** *(optional — skip if previously calibrated)*
The config page shows a live compass rose. Use the ±5° and ±15° buttons to bring the virtual needle to 0°. Adjust the software needle only. The offset is stored in flash and does not need repeating on subsequent power cycles.

**5. Home the physical needle**
Rotate the physical dial needle by hand to the N mark on the compass face.

**6. Initialise**
Press **Initialise Dial** on the config page. This sets `dialBearing = 0.0` in the dead-reckoning model, telling the firmware the physical needle is at North. The instrument is now commissioned.

**7. Set operating mode**
Navigate to the main page (`http://192.168.4.1`) and select the starting mode. The compass defaults to Fixed Bearing on power-up.

---

## Repeat Use (after sleep or power cycle)

The heading offset is stored in flash — no need to repeat step 4.

After a full power cycle, repeat steps 5 and 6 only: home the physical needle by hand, then press Initialise Dial.

After light sleep (lid closed), the needle position is preserved in RAM. On wake, allow a few seconds for the WiFi to restart before reconnecting.

---

## Using `compass_cal.ino`

This is a standalone sketch (separate from the main firmware) that helps measure empirical deg/ms values for the servo.

Flash it to the device, connect to its AP, and use the lap-timing web interface to measure how many degrees the servo turns per millisecond at each speed setting. Record the results and update the speed constants in `lost_compass.ino`.

Run this if the servo is replaced or if dead-reckoning accuracy degrades significantly.
