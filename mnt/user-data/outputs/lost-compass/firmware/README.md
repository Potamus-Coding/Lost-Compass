# Firmware

The main compass firmware and a standalone calibration sketch.

## `lost_compass/`

Flash this for normal use. Requires `html_page.h` and `config_page.h` in the same folder.

## `compass_cal/`

A standalone calibration sketch used to empirically measure the servo's degrees-per-millisecond at each speed and direction. Flash this temporarily when calibrating a replacement servo. See [`docs/calibration.md`](../docs/calibration.md) for instructions.
