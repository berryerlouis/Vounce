# Vounce MIDI Device User Guide

This guide is for musicians and makers who want to use Vounce as a MIDI controller.

## What This Device Does

Vounce turns the enabled physical controls into MIDI Control Change (CC) messages. Depending on which sensor families are enabled in `src/conf/conf.h`, the build can include:
- Rotary encoder
- Push/toggle input
- Potentiometer
- Ultrasonic distance sensor

You can change supported sensor parameters at runtime over serial, without reflashing firmware.

## Before You Start

You need:
- Your board flashed with Vounce firmware
- USB cable connected to your computer
- A serial monitor for configuration
- A DAW or MIDI monitor for testing MIDI output

Connection notes:
- On Pro Micro USB builds, Vounce can appear as a native USB MIDI device.
- On non-USB builds, read the serial-output note below before planning a serial-to-MIDI bridge.

Before flashing, choose:
- The board profile in `src/conf/pinout.h`
- The active sensor families in `src/conf/conf.h`

## Status LED

The current firmware uses one status LED on `PIN_LED_STATUS`.

- Briefly ON during startup
- OFF during normal operation
- Blinks for about 1 second after a parameter is saved to EEPROM

There is no longer a separate idle LED or always-on menu LED state.

## Basic Operation

### Normal Play Mode

During normal runtime:
- Enabled sensors are updated continuously
- Pending changes are sent as MIDI CC messages
- The status LED stays off unless a save just occurred

Main runtime flow: [vounce.ino](vounce.ino)

### Configure Over Serial

Controls:
- `ls`: list sensors
- `s <index>`: select sensor by index (1-based)
- `p <index>`: select parameter by index (1-based)
- `v`: show current parameter value and allowed range
- `v <value>`: set selected parameter value directly
- `h`: show command help

Menu states:
- SelectSensor: choose which sensor to edit
- SelectParam: choose a parameter
- EditParam: change parameter value

Notes:
- Indexes are 1-based in the menu.
- Runtime channel values are displayed as `1..16`.
- The current firmware does not use a physical long-press/short-press menu workflow.
- When you send `v <value>`, the change is applied immediately and saved to EEPROM.

### Use the Browser Configurator

If you want a visual editor instead of typing commands manually, use [sensor-configurator.html](sensor-configurator.html).

- Serve the repository from `localhost` with a small static server.
- Open the page in Edge or Chrome.
- Click `Connect`, choose the Vounce serial port, then use the generated sensor cards and parameter controls.

The page uses the same runtime protocol as the serial menu, strips logger prefixes such as `[INFO]` and `[WARN]`, and still saves values to EEPROM through the normal `v <value>` path.

## Important: Current MIDI and Serial Output Behavior

Current firmware behavior is:

- Every sent CC is printed to serial as readable text such as `Channel: 14, Control: 10, Value: 64`
- On supported Pro Micro / Leonardo / Micro builds, the same CC is also sent over native USB MIDI
- On non-USB builds, the same CC is also written as raw serial MIDI bytes

- `ENABLE_SERIAL_LOGGING` only controls extra logger output such as `[INFO]` menu and EEPROM messages
- It does not disable the CC trace printed by `MidiOut`

Reference: [src/midi/MidiOut.h](src/midi/MidiOut.h), [src/midi/MidiOut.cpp](src/midi/MidiOut.cpp)

Practical implication:
- Pro Micro USB builds are the cleanest setup for simultaneous MIDI plus serial configuration.
- On non-USB boards, one serial connection currently carries readable trace text, optional logger lines, and raw MIDI bytes together.

## First-Time Setup Checklist

1. Flash firmware to your board.
2. Select the correct board profile in `src/conf/pinout.h`.
3. Enable the sensors you want in `src/conf/conf.h`.
4. Connect USB.
5. Verify serial speed is 115200.
6. Open Serial Monitor with line ending set to Newline or Both NL & CR.
7. Send `h` and verify the menu commands are listed.
8. Send `ls` and confirm your compiled sensors appear.
9. Move controls and confirm readable CC output appears.
10. If using a USB-capable Pro Micro build, confirm the DAW also sees a MIDI device.

## Typical Live Workflow

1. Power up device.
2. Keep normal play running and send serial commands only when needed.
3. If response needs adjustment, use `ls` then `s <index>` to select a sensor.
4. Use `p <index>` then `v` to inspect the current value and range.
5. Send `v <value>` to apply and save a new setting.
6. Keep playing; changes are applied immediately.

## Encoder and Switch Behavior

- Pressing the encoder switch does not change the encoder MIDI value.
- Encoder movement is ignored while the encoder switch is held.
- After a release, encoder movement is suppressed briefly to reject false deltas caused by button bounce.
- Menu editing is done through serial commands.
- The switch input toggles only after a valid release, which rejects very short false presses caused by mechanical noise during rotation.

## Sensor Parameters Reference

Each sensor can be configured through the menu with different parameters. This section lists all editable parameters for each sensor type.

### Encoder Sensor

The rotary encoder control.

| Parameter | Min | Max | Step | Description |
|-----------|-----|-----|------|-------------|
| midi_channel | 1 | 16 | 1 | MIDI channel shown in the menu |
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
| midi_channel | 1 | 16 | 1 | MIDI channel shown in the menu |
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
| midi_channel | 1 | 16 | 1 | MIDI channel shown in the menu |
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
| midi_channel | 1 | 16 | 1 | MIDI channel shown in the menu |
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
- Check whether your board is using USB MIDI or serial MIDI transport.
- On USB-capable Pro Micro builds, verify the board profile exposes the USB macros used by the firmware.
- On non-USB boards, remember that the current serial stream mixes text output and serial MIDI bytes.

Board powers on, but no USB MIDI device appears:
- Confirm you selected a USB-capable Pro Micro / Leonardo / Micro board profile in the Arduino toolchain.
- Reflash and reconnect USB so the host re-enumerates the device.
- If your toolchain does not build the USB MIDI path, Vounce will fall back to serial MIDI output.

Menu commands are not responding:
- Verify Serial Monitor uses 115200 baud and line ending set to Newline or Both NL & CR.
- Send `h` to print available menu commands.
- Remember that valid selection commands are `s <index>` and `p <index>`.

Cannot select sensor/parameter:
- Use `ls` first to view sensor list and indexes.
- Use `s <index>` and `p <index>` with 1-based indexes.

LED behavior looks wrong:
- Verify pin wiring for `PIN_LED_STATUS`.
- Ensure LED polarity and resistor are correct.
- A blink only occurs after a successful save, not while browsing parameters.

MIDI stops while editing:
- Verify your sketch wiring in [vounce.ino](vounce.ino).
- In the current example wiring, sensor streaming remains enabled while editing.

## Where to Customize

- Wiring and pin assignments: [vounce.ino](vounce.ino)
- Board profile selection: [src/conf/pinout.h](src/conf/pinout.h)
- Sensor feature flags: [src/conf/conf.h](src/conf/conf.h)
- Menu logic: [src/menu/ConfigMenu.cpp](src/menu/ConfigMenu.cpp)
- MIDI transport mode: [src/midi/MidiOut.cpp](src/midi/MidiOut.cpp)
- LED behavior: [src/led/Led.cpp](src/led/Led.cpp)
- Non-MIDI logs: [src/utils/Logger.cpp](src/utils/Logger.cpp)

---