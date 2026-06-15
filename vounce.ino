#include "src/core/SensorManager.h"
#include "src/input/ButtonInput.h"
#include "src/input/RotaryInput.h"
#include "src/menu/ConfigMenu.h"
#include "src/midi/MidiOut.h"
#include "src/sensors/EncoderSensor.h"
#include "src/sensors/PotentiometerSensor.h"
#include "src/sensors/ToggleSwitchSensor.h"
#include "src/sensors/UltrasonicSensor.h"

// KY-040
#define PIN_SW    4
#define PIN_CLK   3
#define PIN_DT    2
#define PIN_MENU  8

// Optional sensor pins
#define PIN_POT   A0
#define PIN_TRIG  6
#define PIN_ECHO  5

MidiOut midiOut(Serial);
SensorManager sensorManager;
ButtonInput menuButton(PIN_MENU, 30, 800);
RotaryInput menuRotary(PIN_CLK, PIN_DT);
ConfigMenu configMenu(menuButton, menuRotary, Serial);

// Active sensors
EncoderSensor encoderSensor(PIN_CLK, PIN_DT, 10, 0, 64, 1, 0, 127);
ToggleSwitchSensor switchSensor(PIN_SW, 11, 1, 0, 127, 30);

// Optional examples: uncomment one or both lines below to add more sensors.
// PotentiometerSensor potSensor(PIN_POT, 12, 7, 0, 1023, 0, 127, 1);
// UltrasonicSensor distanceSensor(PIN_TRIG, PIN_ECHO, 13, 10, 5, 80, 0, 127, 50, 1);

void setup() {
    midiOut.begin(115200);

    sensorManager.addSensor(encoderSensor);
    sensorManager.addSensor(switchSensor);
    // sensorManager.addSensor(potSensor);
    // sensorManager.addSensor(distanceSensor);

    configMenu.addSensor(encoderSensor);
    configMenu.addSensor(switchSensor);
    // configMenu.addSensor(potSensor);
    // configMenu.addSensor(distanceSensor);

    sensorManager.begin();
    configMenu.begin();
}

void loop() {
    configMenu.update();
    sensorManager.setEnabled(!configMenu.isOpen());
    sensorManager.updateAndSend(midiOut);
}
