# Vounce Pin Configuration Guide

This document describes available pin configurations for different microcontroller boards.

## Quick Switch

In [vounce.ino](vounce.ino), uncomment your board type:

```cpp
#define PINOUT_UNO_NANO
// #define PINOUT_PRO_MICRO
```

Then comment out all others.

## Configuration 1: Arduino Uno / Nano (Default)

**Board:** Arduino Uno, Arduino Nano, or compatible clones  
**Symbol:** `PINOUT_UNO_NANO`

| Function | Pin | Notes |
|----------|-----|-------|
| Encoder CLK | D3 | |
| Encoder DT | D2 | |
| Encoder Switch | D4 | |
| Menu Button | D8 | |
| Menu LED | D9 | PWM capable |
| Idle LED | D10 | PWM capable |
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
- LEDs: 2 pins (each needs resistor ~220Ω)
- Button: 1 pin + GND (with pullup)

## Configuration 2: Pro Micro 5V/16MHz

**Board:** SparkFun Pro Micro 5V/16MHz, clones  
**Symbol:** `PINOUT_PRO_MICRO`

| Function | Pin | Notes |
|----------|-----|-------|
| Encoder CLK | D2 | |
| Encoder DT | D3 | |
| Encoder Switch | D4 | |
| Menu Button | D5 | |
| Menu LED | D6 | PWM capable |
| Idle LED | D7 | PWM capable |
| Potentiometer | A0 | Optional |
| Ultrasonic Trigger | D14 (MISO) | Optional, careful with SPI |
| Ultrasonic Echo | D15 (MOSI) | Optional, careful with SPI |

**Pin Availability on Pro Micro:**
- Available: D2, D3, D4, D5, D6, D7, D14, D15, D16, A0, A1, A2, A3, A6, A7
- **Avoid:** D0/D1 (RX/TX), D8-D13 (not available)

**Advantages:**
- More compact form factor
- Built-in USB MIDI support possible (with firmware modification)
- Good for portable rigs

**Limitations:**
- Fewer total I/O pins than Uno/Nano
- Ultrasonic sensor uses MISO/MOSI pins (won't conflict if not using SPI)
- Some pins have different functions

**Typical Wiring:**
- Connect GND and RAW (5V) from all components
- Encoder: same 3 wires (CLK, DT, SW) + GND
- LEDs: 2 pins (each needs resistor ~220Ω)
- Button: 1 pin + GND (with pullup)

## Adding More Boards

To add a new board configuration:

1. Add a new `#define PINOUT_BOARD_NAME` symbol
2. Add a new `#ifdef` block in [vounce.ino](vounce.ino) with pin definitions
3. Document in this file
4. Update the quick switch comments

Example:

```cpp
// In vounce.ino
#ifdef PINOUT_STM32
#define PIN_CLK   PA1
#define PIN_DT    PA2
// ... other pins
#endif
```

## Pro Micro USB MIDI (Advanced)

Pro Micro boards can communicate directly via USB as MIDI devices without serial/host bridge. This requires:

1. Different firmware (using `usbMIDI` library or equivalent)
2. Removing the serial logger dependency
3. Implementing `midiOut.sendCC()` directly to USB stack

Current implementation uses Serial output only.

## Troubleshooting Pin Issues

**Nothing responds on Pro Micro:**
- Verify you uncommented `PINOUT_PRO_MICRO`
- Check that unused config is commented
- Confirm board type in Arduino IDE: Tools → Board → SparkFun Pro Micro (5V/16MHz)

**Encoder not working:**
- Pro Micro: Verify D2, D3, D4 are not used by other shields
- Uno/Nano: Verify D2, D3, D4 are free
- Check encoder wiring + GND connection

**LEDs don't respond:**
- Verify resistors are installed (~220Ω for 5V)
- Check polarity (longer leg = +)
- Test with digitalWrite() directly to rule out brightness settings

**Ultrasonic doesn't trigger (Pro Micro only):**
- MISO/MOSI (D14/D15) can interfere with SPI if enabled
- Try moving to alternate pins if available on your board
- Ensure separate GND for sensor module
