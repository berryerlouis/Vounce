# Vounce MIDI Device User Guide

This guide is for musicians and makers who want to use Vounce as a MIDI controller.

## What This Device Does

Vounce turns physical controls into MIDI Control Change (CC) messages:
- Rotary encoder
- Push/toggle input
- Optional sensors (potentiometer, ultrasonic)

You can change sensor behavior directly on the device using the built-in menu, without reflashing firmware.

## Before You Start

You need:
- Your board flashed with Vounce firmware
- USB cable connected to your computer
- A DAW or MIDI monitor

Connection notes:
- On Pro Micro USB builds, Vounce can appear as a native USB MIDI device.
- On non-USB builds, you need a serial-to-MIDI bridge on the host side.

## LED Meanings

The 2 LEDs indicate operating mode:

- Menu LED (`PIN_LED_MENU`)
  - Off: menu closed
  - On: menu open
  - Blinking: editing a setting

- Idle LED (`PIN_LED_IDLE`)
  - On: menu closed (normal play mode)
  - Off: menu open

Implementation reference: [src/led/LedIndicator.cpp](src/led/LedIndicator.cpp)

## Basic Operation

### Normal Play Mode

When the menu is closed:
- Sensor changes are sent as MIDI CC
- Idle LED is ON

Main runtime flow: [vounce.ino](vounce.ino)

### Open and Use the Menu

Controls:
- Long press: open menu, go back, or close menu
- Short press: confirm / enter / exit edit mode
- Rotate encoder: navigate items or change value

Menu states:
- SelectSensor: choose which sensor to edit
- SelectParam: choose a parameter
- EditParam: change parameter value

Detailed transitions: [MENU_FLOW.md](MENU_FLOW.md)

## Important: MIDI Mode vs Debug Log Mode

Vounce `MidiOut` supports two output styles:

- `logEnabled = true` (default)
  - Prints readable lines like `Channel: 10, Control: 1, Value: 64`
  - Best for testing and debugging
  - MIDI is still transmitted at the same time
  - Menu state can also be observed in readable text through the logger

- `logEnabled = false`
  - Hides debug text output
  - Still sends MIDI normally
  - Menu state sent as MIDI CC 120 on channel 11 (button channel) for remote debugging

Reference: [src/midi/MidiOut.h](src/midi/MidiOut.h), [src/midi/MidiOut.cpp](src/midi/MidiOut.cpp)

If you want real MIDI behavior, instantiate `MidiOut` with `false` in [vounce.ino](vounce.ino).

On Pro Micro targets, USB MIDI is enabled automatically when the selected Arduino board profile exposes a supported USB macro. On other targets, Vounce falls back to serial MIDI bytes.

### Menu State Output via MIDI

When `logEnabled = false`, the menu system outputs its state as MIDI CC 120 messages on channel 11 (the button/menu sensor channel). This allows external MIDI devices or software to track menu navigation even when serial logging is disabled.

**Menu state encoding:**
- Menu closed: value = 0
- SelectSensor mode: value = (selected sensor index + 1) → range 1–N (where N is number of sensors)
- SelectParam mode: value = (selected parameter index + 100) → range 100+
- EditParam mode: value = (constrained parameter value) → range 0–127

This feature is useful for:
- Monitoring menu state from a DAW or MIDI monitor
- Mapping menu navigation to external hardware
- Debugging controller behavior in production environments

## First-Time Setup Checklist

1. Flash firmware to your board.
2. Connect USB.
3. Verify serial speed is 115200.
4. Start in debug mode (`logEnabled = true`) to confirm activity.
5. Move controls and confirm output appears.
6. Open menu and verify LEDs react as expected.
7. Switch to raw MIDI mode (`logEnabled = false`) for production use.
8. Map incoming CC in your DAW.

## Typical Live Workflow

1. Power up device.
2. Keep menu closed for normal MIDI control.
3. If response needs adjustment, long press to open menu.
4. Edit sensor parameters (channel/CC/range/sensitivity).
5. Exit menu to resume MIDI streaming.

## Encoder and Switch Behavior

- Pressing the encoder switch does not change the encoder MIDI value.
- Encoder movement is ignored while the encoder switch is held.
- The menu rotary decoder stays synchronized while the switch is held, so edit mode remains accurate after release.
- The switch input filters very short presses to reject false toggles caused by mechanical noise during rotation.

## Sensor Parameters Reference

Each sensor can be configured through the menu with different parameters. This section lists all editable parameters for each sensor type.

### Encoder Sensor

The rotary encoder control.

| Parameter | Min | Max | Step | Description |
|-----------|-----|-----|------|-------------|
| midi_channel | 0 | 15 | 1 | MIDI channel (0-15 = channels 1-16) |
| midi_control | 0 | 127 | 1 | MIDI CC number for output |
| min | 0 | 127 | 1 | Minimum MIDI value (lower bound) |
| max | 0 | 127 | 1 | Maximum MIDI value (upper bound) |
| sensitivity | 1 | 16 | 1 | Amount to change per encoder step (1-16) |

**Tips:**
- Increase sensitivity for coarse control (large jumps per step)
- Decrease sensitivity for fine control (small increments)
- Set min/max to define the range of values sent

### Toggle Switch Sensor

The push button or momentary switch.

| Parameter | Min | Max | Step | Description |
|-----------|-----|-----|------|-------------|
| midi_channel | 0 | 15 | 1 | MIDI channel (0-15 = channels 1-16) |
| midi_control | 0 | 127 | 1 | MIDI CC number for output |
| value_off | 0 | 127 | 1 | MIDI value when switch is released |
| value_on | 0 | 127 | 1 | MIDI value when switch is pressed |
| debounce_ms | 5 | 500 | 5 | Bounce filtering delay (milliseconds) |

**Tips:**
- Set value_off to 0 and value_on to 127 for binary on/off behavior
- Use other pairs (e.g., 32/96) for different toggle points
- Increase debounce_ms if switch causes multiple rapid triggers

### Potentiometer Sensor (optional)

Analog slider or knob on pin A0.

| Parameter | Min | Max | Step | Description |
|-----------|-----|-----|------|-------------|
| midi_channel | 0 | 15 | 1 | MIDI channel (0-15 = channels 1-16) |
| midi_control | 0 | 127 | 1 | MIDI CC number for output |
| analog_min | 0 | 1023 | 1 | Raw ADC value at fully CCW (0-1023) |
| analog_max | 0 | 1023 | 1 | Raw ADC value at fully CW (0-1023) |
| midi_min | 0 | 127 | 1 | MIDI output when analog is at minimum |
| midi_max | 0 | 127 | 1 | MIDI output when analog is at maximum |
| threshold | 1 | 16 | 1 | Minimum change to send new MIDI value |

**Tips:**
- Calibrate analog_min/max by moving pot to extremes and noting raw values
- Use threshold to reduce MIDI traffic (higher = less frequent updates)
- Swap midi_min and midi_max to invert the pot response

### Ultrasonic Sensor (optional)

HC-SR04 distance sensor (pins 6=trigger, 5=echo).

| Parameter | Min | Max | Step | Description |
|-----------|-----|-----|------|-------------|
| midi_channel | 0 | 15 | 1 | MIDI channel (0-15 = channels 1-16) |
| midi_control | 0 | 127 | 1 | MIDI CC number for output |
| dist_min_cm | 1 | 400 | 1 | Closest distance to map (centimeters) |
| dist_max_cm | 1 | 400 | 1 | Farthest distance to map (centimeters) |
| midi_min | 0 | 127 | 1 | MIDI output when object is at dist_min |
| midi_max | 0 | 127 | 1 | MIDI output when object is at dist_max |
| sample_ms | 10 | 500 | 5 | Time between measurements (milliseconds) |
| threshold | 1 | 16 | 1 | Minimum change to send new MIDI value |

**Tips:**
- Typical max range is 200-300 cm (HC-SR04 limit ~4m)
- Shorter sample intervals = more responsive but higher CPU use
- Use threshold to ignore small fluctuations from reflections
- Swap midi_min/max to reverse behavior (far = high, close = low)

## Troubleshooting

No output at all:
- Check USB cable and board port.
- Confirm firmware is running.
- Confirm baud is 115200.

Readable text appears, but DAW receives no MIDI:
- On current firmware, logging does not disable MIDI output.
- If there is still no MIDI, check whether your board is using USB MIDI or serial MIDI transport.
- For non-USB boards, use a serial-to-MIDI bridge.

Board powers on, but no USB MIDI device appears:
- Confirm you selected a USB-capable Pro Micro / Leonardo / Micro board profile in the Arduino toolchain.
- Reflash and reconnect USB so the host re-enumerates the device.
- If your toolchain does not build the USB MIDI path, Vounce will fall back to serial MIDI output.

Menu cannot be opened:
- Verify button wiring on the pin used by `menuButton` in [vounce.ino](vounce.ino).
- Check long-press duration settings in `ButtonInput` initialization.

LED behavior looks wrong:
- Verify pin wiring for both LEDs.
- Ensure LED polarity and resistors are correct.

MIDI stops while editing:
- This is expected.
- Vounce pauses MIDI when menu is open, then resumes when closed.

## Where to Customize

- Wiring and pin assignments: [vounce.ino](vounce.ino)
- Menu logic: [src/menu/ConfigMenu.cpp](src/menu/ConfigMenu.cpp)
- MIDI transport mode: [src/midi/MidiOut.cpp](src/midi/MidiOut.cpp)
- LED behavior: [src/led/LedIndicator.cpp](src/led/LedIndicator.cpp)
- Non-MIDI logs: [src/utils/Logger.cpp](src/utils/Logger.cpp)

---