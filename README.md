# vounce - MIDI sensors architecture

## Project structure

- `vounce.ino`: sketch wiring, sensor instantiation, and startup sequence
- `src/conf/`: board profile selection and feature flags
- `src/core/`: common interfaces, EEPROM config store, and sensor orchestration
- `src/led/`: status LED driver
- `src/menu/`: serial configuration menu
- `src/midi/`: MIDI output transport
- `src/sensors/`: concrete sensor implementations
- `sensor-configurator.html`: standalone Web Serial configurator for the same menu protocol

## Core idea

Every sensor implements `IMidiSensor` and can emit a `MidiMessage` with:

- `channel`
- `control`
- `value`

`SensorManager` updates all registered sensors and sends pending messages through `MidiOut`.

Sensors that also implement `IConfigurableSensor` expose runtime-editable parameters. Those values are persisted through `SensorConfigEeprom` and restored at startup.

## Build-time configuration

Board selection now lives in `src/conf/pinout.h`:

```cpp
//#define PINOUT_UNO_NANO
#define PINOUT_PRO_MICRO
```

Per-board pins are split into:

- `src/conf/pinout_nano.h`
- `src/conf/pinout_pro.h`

Active sensor types are selected in `src/conf/conf.h`:

```cpp
//#define VOUNCE_ENCODER_SENSOR
//#define VOUNCE_POTENTIOMETER_SENSOR
#define VOUNCE_ULTRASONIC_SENSOR
```

The repository currently ships with only the ultrasonic sensor enabled by default. Enable or disable sensor families there before flashing.

## Runtime configuration menu

The runtime menu is now serial-driven. There is no separate physical menu button or rotary-driven menu state machine in the current firmware.

See [USER_GUIDE.md](USER_GUIDE.md) for end-user setup and troubleshooting.
See [USER_GUIDE_FR.md](USER_GUIDE_FR.md) for the French user guide.
See [PINOUTS.md](PINOUTS.md) for board-specific wiring details.
See [sensor-configurator.html](sensor-configurator.html) for a browser UI that drives the same protocol through Web Serial.

Available commands:

- `ls`: list sensors
- `s <index>`: select a sensor by 1-based index
- `p <index>`: select a parameter by 1-based index
- `v`: show the currently selected parameter value and range
- `v <value>`: set the selected parameter value
- `h`: print command help

The menu is intentionally index-driven for quick edits from the Serial Monitor. Runtime menu values show MIDI channels as `1..16`, while constructor arguments in code remain zero-based as usual for MIDI status bytes.

If you prefer a visual UI, serve `sensor-configurator.html` from `localhost` and open it in Edge or Chrome. The page reads the live sensor list, strips logger prefixes such as `[INFO]` and `[WARN]`, and writes back values through the same `s`, `p`, and `v` commands.

When a parameter is edited successfully, it is saved immediately to EEPROM.

## EEPROM persistence

`SensorConfigEeprom` stores all configurable sensor parameters in EEPROM using a small layout header.

At startup:

- If the header matches the current sensor layout, values are loaded from EEPROM.
- If the layout changed, defaults from the sketch are written to EEPROM.
- If EEPROM access fails, the sketch keeps running with in-memory defaults.

With logger output enabled, serial messages include EEPROM lifecycle lines such as:

```text
[INFO]   EEPROM: start (sensors=2)
[INFO]   EEPROM: values loaded
```

or:

```text
[INFO]   EEPROM: layout mismatch, writing defaults
[INFO]   EEPROM: defaults initialized
```

## LED behavior

The old dual-LED manager has been removed. The current sketch drives a single `Led` instance on `PIN_LED_STATUS`:

- ON briefly during startup
- OFF during normal runtime
- BLINK for about 1 second after a parameter save succeeds

That blink is triggered from the config menu after `SensorConfigEeprom::saveParameter(...)` succeeds.

## MIDI and serial behavior

- On supported Pro Micro / Leonardo / Micro targets, `src/conf/pinout_pro.h` enables `VOUNCE_USE_MIDIUSB` automatically and CC messages are sent through `MIDIUSB`.
- On non-USB targets, CC messages are also written as standard serial MIDI bytes.
- The current `MidiOut` implementation prints a human-readable CC trace to the serial stream for every message, for example `Channel: 14, Control: 10, Value: 64`.
- `ENABLE_SERIAL_LOGGING` in `vounce.ino` controls `Logger` output such as menu and EEPROM messages. It does not disable the CC trace printed by `MidiOut`.

This matters on non-USB boards: the serial port currently carries readable trace text, optional logger messages, and serial MIDI bytes on the same connection.

## Runtime protections

- The encoder sensor ignores movement while the encoder switch is held.
- After an encoder button release, movement is suppressed briefly to reject false deltas caused by bounce.
- The toggle switch sensor toggles on a valid release only, which filters short glitches during encoder movement.

## Add a new sensor

1. Include its header in `vounce.ino`.
2. Instantiate it with the required MIDI mapping and pin arguments.
3. Register it with `sensorManager.addSensor(...)`.
4. If it is configurable, also register it with `configMenu.addSensor(...)`.
5. If it is configurable, also register it with `sensorConfigStore.addSensor(...)`.

Example:

```cpp
PotentiometerSensor potSensor(PIN_POT, 12, 7, 0, 1023, 0, 127, 1);
sensorManager.addSensor(potSensor);
configMenu.addSensor(potSensor);
sensorConfigStore.addSensor(potSensor);
```

In constructor code, `channel = 12` still means MIDI channel 13. In the runtime menu, the same channel is displayed as `13`.

## Existing sensors

- `EncoderSensor`
- `ToggleSwitchSensor`
- `PotentiometerSensor`
- `UltrasonicSensor`
