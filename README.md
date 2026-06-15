# vounce - MIDI sensors architecture

## Project structure

- `vounce.ino`: wiring and configuration only
- `src/core/`: common interfaces and orchestration
- `src/midi/`: MIDI output transport
- `src/sensors/`: concrete sensor implementations

## Core idea

Every sensor derives from `IMidiSensor` and can emit a `MidiMessage`:

- `channel`
- `control`
- `value`

`SensorManager` updates all sensors and sends any pending messages through `MidiOut`.

Sensors can also derive from `IConfigurableSensor` to expose runtime-editable parameters in the software menu.

## Runtime software menu

See [MENU_FLOW.md](MENU_FLOW.md) for detailed state machine diagrams and all possible transitions.

- Long press on menu button (`PIN_MENU`) opens or closes the menu.
- Rotate encoder to navigate between sensors or parameters.
- Short press to enter selection and edit mode.
- Long press to go back one level.

When the menu is open, MIDI streaming is paused so encoder movement edits values instead of sending CC.

## Add a new sensor on a new MIDI channel

1. Include its header in `vounce.ino`.
2. Instantiate it with your MIDI mapping (`channel`, `control`).
3. Register it with `sensorManager.addSensor(...)`.

Example:

```cpp
PotentiometerSensor potSensor(A0, 12, 7, 0, 1023, 0, 127, 1);
sensorManager.addSensor(potSensor);
```

This sends the potentiometer values on channel 12, CC 7.

## Existing sensors

- `EncoderSensor`
- `ToggleSwitchSensor`
- `PotentiometerSensor`
- `UltrasonicSensor`
