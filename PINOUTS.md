# Vounce Pin Configuration Guide

This document describes available pin configurations for different microcontroller boards.

## Quick Switch

Select your board profile in [src/conf/pinout.h](src/conf/pinout.h):

```cpp
//#define PINOUT_UNO_NANO
#define PINOUT_PRO_MICRO
```

Then comment out all others.

The selected header pulls pins from:

- [src/conf/pinout_nano.h](src/conf/pinout_nano.h)
- [src/conf/pinout_pro.h](src/conf/pinout_pro.h)

Sensor families are enabled separately in [src/conf/conf.h](src/conf/conf.h).

## Configuration 1: Arduino Uno / Nano (Default)

**Board:** Arduino Uno, Arduino Nano, or compatible clones  
**Symbol:** `PINOUT_UNO_NANO`

| Function | Pin | Notes |
|----------|-----|-------|
| Encoder CLK | D3 | |
| Encoder DT | D2 | |
| Encoder Switch | D4 | |
| Status LED | D9 | Single LED used for startup and save feedback |
| Potentiometer | A0 | Optional |
| Ultrasonic Trigger | D6 | Optional |
| Ultrasonic Echo | D5 | Optional |

**Advantages:**
- Most Arduino pins available
- Wide compatibility
- Well-documented

**Typical Wiring:**
- Connect GND and 5V from all components
- Encoder: 3 wires (CLK, DT, SW) + GND
- Status LED: 1 pin with resistor (~220Ω)
- Potentiometer and ultrasonic pins are only needed if those sensor families are enabled in [src/conf/conf.h](src/conf/conf.h)

**Notes:**
- There is no dedicated menu button in the current firmware.
- Configuration is done over serial or through [sensor-configurator.html](sensor-configurator.html).
- The status LED is briefly on during startup and blinks after a successful EEPROM save.

## Configuration 2: Pro Micro 5V/16MHz

**Board:** SparkFun Pro Micro 5V/16MHz, clones  
**Symbol:** `PINOUT_PRO_MICRO`

| Function | Pin | Notes |
|----------|-----|-------|
| Encoder CLK | D2 | |
| Encoder DT | D3 | |
| Encoder Switch | D4 | |
| Status LED | D6 | Single LED used for startup and save feedback |
| Potentiometer | A0 | Optional |
| Ultrasonic Trigger | D14 (MISO) | Optional, careful with SPI |
| Ultrasonic Echo | D15 (MOSI) | Optional, careful with SPI |

**Pin Availability on Pro Micro:**
- Available: D2, D3, D4, D5, D6, D7, D14, D15, D16, A0, A1, A2, A3, A6, A7
- **Avoid:** D0/D1 (RX/TX), D8-D13 (not available)

**Advantages:**
- More compact form factor
- Built-in USB MIDI support is enabled automatically on supported board definitions
- Good for portable rigs

**Limitations:**
- Fewer total I/O pins than Uno/Nano
- Ultrasonic sensor uses MISO/MOSI pins (won't conflict if not using SPI)
- Some pins have different functions

**Typical Wiring:**
- Connect GND and RAW (5V) from all components
- Encoder: same 3 wires (CLK, DT, SW) + GND
- Status LED: D6 with resistor (~220Ω)
- Potentiometer and ultrasonic pins are only needed if those sensor families are enabled in [src/conf/conf.h](src/conf/conf.h)

**Notes:**
- There is no dedicated menu button in the current firmware.
- On supported Pro Micro, Leonardo, and Micro targets, [src/conf/pinout_pro.h](src/conf/pinout_pro.h) defines `VOUNCE_USE_MIDIUSB` automatically.
- Configuration is still done over serial or Web Serial using the same menu protocol.

## Adding More Boards

To add a new board configuration:

1. Add a new `#define PINOUT_BOARD_NAME` symbol
2. Add a new pin header in `src/conf/`
3. Include it from [src/conf/pinout.h](src/conf/pinout.h)
4. Document in this file
5. Update the quick switch comments

Example:

```cpp
// In src/conf/pinout.h
#ifdef PINOUT_STM32
#include "pinout_stm32.h"
#endif
```

## Pro Micro USB MIDI (Advanced)

Pro Micro-compatible boards can communicate directly via USB as MIDI devices when the selected Arduino core exposes one of the supported USB board macros.

Current behavior:

- [src/conf/pinout_pro.h](src/conf/pinout_pro.h) enables `VOUNCE_USE_MIDIUSB` automatically for supported Pro Micro / Leonardo / Micro targets.
- [src/midi/MidiOut.cpp](src/midi/MidiOut.cpp) sends CC messages through `MIDIUSB` on those targets.
- The firmware still uses serial for configuration commands and readable CC/log output.

## Troubleshooting Pin Issues

**Nothing responds on Pro Micro:**
- Verify you uncommented `PINOUT_PRO_MICRO`
- Check that unused config is commented
- Confirm board type in Arduino IDE: Tools → Board → SparkFun Pro Micro (5V/16MHz)

**Encoder not working:**
- Pro Micro: Verify D2, D3, D4 are not used by other shields
- Uno/Nano: Verify D2, D3, D4 are free
- Check encoder wiring + GND connection

**Status LED doesn't respond:**
- Verify resistors are installed (~220Ω for 5V)
- Check polarity (longer leg = +)
- Test with digitalWrite() directly to rule out brightness settings
- Remember this LED is normally off after startup and only blinks after a successful parameter save

**Ultrasonic doesn't trigger (Pro Micro only):**
- MISO/MOSI (D14/D15) can interfere with SPI if enabled
- Try moving to alternate pins if available on your board
- Ensure separate GND for sensor module
