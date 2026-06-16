# vounce - MIDI sensors architecture

## Project structure

- `vounce.ino`: wiring and configuration only
- `src/conf/`: board-specific pin configuration
- `src/core/`: common interfaces and orchestration
- `src/midi/`: MIDI output transport
- `src/sensors/`: concrete sensor implementations

## Core idea

Every sensor derives from `IMidiSensor` and can emit a `MidiMessage`:

- `channel`
- `control`
- `value`

`SensorManager` updates all sensors and sends pending messages through `MidiOut`.

To reduce mixed bursts when multiple inputs change nearly at the same time, the manager emits messages from one updated sensor per loop tick.

Sensors can also derive from `IConfigurableSensor` to expose runtime-editable parameters in the software menu.

Configurable sensor parameters are persisted through `SensorConfigEeprom` and restored at startup.

## Runtime software menu

See [MENU_FLOW.md](MENU_FLOW.md) for detailed state machine diagrams and all possible transitions.
See [USER_GUIDE.md](USER_GUIDE.md) for setup, wiring, LED behavior, and troubleshooting.
See [PINOUTS.md](PINOUTS.md) for board-specific pin configurations (Uno/Nano vs Pro Micro).

- Long press on menu button (`PIN_MENU`) opens or closes the menu.
- Rotate encoder to navigate between sensors or parameters.
- Short press to enter selection and edit mode.
- Long press to go back one level.

When the menu is open, MIDI streaming is paused so encoder movement edits values instead of sending CC.

When a parameter is edited in the menu, the new value is saved immediately to EEPROM.

When serial logging is disabled, menu state is exported as MIDI CC 120 on the configured menu channel so you can monitor navigation from a DAW or MIDI monitor.

## EEPROM persistence

`SensorConfigEeprom` stores all configurable sensor parameters in EEPROM using a small header:

- `magic`
- `version`
- `sensorCount`
- `parameterCount`

At startup:

- If the header matches the current sensor layout, values are loaded from EEPROM.
- If the layout changed, defaults from the sketch are written to EEPROM.
- If EEPROM access fails, the sketch keeps running with in-memory defaults.

When logging is enabled, serial output includes EEPROM lifecycle messages such as:

```text
[INFO]   EEPROM: start (sensors=2)
[INFO]   EEPROM: values loaded
```

or:

```text
[INFO]   EEPROM: layout mismatch, writing defaults
[INFO]   EEPROM: defaults initialized
```

## Pinout and logging configuration

Board pin selection is defined in `src/conf/pinout.h`.

For Pro Micro targets, USB MIDI is enabled automatically when the Arduino core exposes a supported USB board macro (`USBCON`, `ARDUINO_AVR_PROMICRO`, `ARDUINO_AVR_LEONARDO`, or `ARDUINO_AVR_MICRO`).

Logging is controlled from `vounce.ino`:

```cpp
#define ENABLE_SERIAL_LOGGING false
```

`MidiOut` always transmits MIDI. Serial logging is optional and does not disable MIDI transmission.

## Runtime protections

- The encoder sensor ignores movement while the encoder switch is held.
- The menu rotary input uses the same lock-and-sync approach, so edit mode remains accurate after pressing the switch.
- The toggle switch sensor validates press duration before toggling, which filters short glitches caused by encoder movement.

## MIDI behavior

- On USB-capable Pro Micro builds, CC messages are sent through the `MIDIUSB` transport.
- On non-USB targets, CC messages are sent as standard serial MIDI bytes.
- Menu debug output uses CC 120 on the menu channel.
- Sensor MIDI channels remain zero-based in code: `0..15` maps to MIDI channels `1..16`.

## Add a new sensor on a new MIDI channel

1. Include its header in `vounce.ino`.
2. Instantiate it with your MIDI mapping (`channel`, `control`).
3. Register it with `sensorManager.addSensor(...)`.
4. If it is configurable, also register it with `configMenu.addSensor(...)`.
5. If it is configurable, also register it with `sensorConfigStore.addSensor(...)`.

Example:

```cpp
PotentiometerSensor potSensor(A0, 12, 7, 0, 1023, 0, 127, 1);
sensorManager.addSensor(potSensor);
configMenu.addSensor(potSensor);
sensorConfigStore.addSensor(potSensor);
```

This sends the potentiometer values on channel 12, CC 7.

## Existing sensors

- `EncoderSensor`
- `ToggleSwitchSensor`
- `PotentiometerSensor`
- `UltrasonicSensor`
