#include "src/conf/pinout.h"
#include "src/core/SensorManager.h"
#include "src/core/SensorConfigEeprom.h"
#include "src/input/ButtonInput.h"
#include "src/input/RotaryInput.h"
#include "src/menu/ConfigMenu.h"
#include "src/midi/MidiOut.h"
#include "src/led/LedIndicator.h"
#include "src/utils/Logger.h"
#include "src/sensors/EncoderSensor.h"
#include "src/sensors/PotentiometerSensor.h"
#include "src/sensors/ToggleSwitchSensor.h"
#include "src/sensors/UltrasonicSensor.h"


#define ENABLE_SERIAL_LOGGING   true

// ============================================================

MidiOut midiOut(&Serial, ENABLE_SERIAL_LOGGING);
SensorManager sensorManager;
ButtonInput menuButton(PIN_MENU, 30, 800);
RotaryInput menuRotary(PIN_CLK, PIN_DT);
Logger logger(Serial, Logger::INFO, ENABLE_SERIAL_LOGGING);
SensorConfigEeprom sensorConfigStore(&logger);
ConfigMenu configMenu(menuButton, menuRotary, logger, &sensorConfigStore, &midiOut, 11);
LedManager ledManager(PIN_LED_MENU, PIN_LED_IDLE, configMenu);

// Active sensors
EncoderSensor encoderSensor(PIN_CLK, PIN_DT, 10, 0, 64, 10, 0, 127);
ToggleSwitchSensor switchSensor(PIN_SW, 11, 1, 0, 127, 30);

// Optional examples: uncomment one or both lines below to add more sensors.
// PotentiometerSensor potSensor(PIN_POT, 12, 7, 0, 1023, 0, 127, 1);
// UltrasonicSensor distanceSensor(PIN_TRIG, PIN_ECHO, 13, 10, 5, 80, 0, 127, 50, 1);

void setup() {
    midiOut.begin(115200);
    encoderSensor.setLockWhilePinLow(PIN_SW);
    menuRotary.setLockWhilePinLow(PIN_SW);

    sensorManager.addSensor(encoderSensor);
    sensorManager.addSensor(switchSensor);
    // sensorManager.addSensor(potSensor);
    // sensorManager.addSensor(distanceSensor);

    configMenu.addSensor(encoderSensor);
    configMenu.addSensor(switchSensor);
    // configMenu.addSensor(potSensor);
    // configMenu.addSensor(distanceSensor);

    sensorConfigStore.addSensor(encoderSensor);
    sensorConfigStore.addSensor(switchSensor);
    // sensorConfigStore.addSensor(potSensor);
    // sensorConfigStore.addSensor(distanceSensor);

    sensorConfigStore.begin();
    sensorManager.begin();
    configMenu.begin();
    ledManager.begin();
}

void loop() {
    configMenu.update();
    ledManager.update();
    sensorManager.setEnabled(!configMenu.isOpen());
    sensorManager.updateAndSend(midiOut);
}
