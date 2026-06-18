#include "src/conf/pinout.h"
#include "src/conf/conf.h"
#include "src/core/SensorManager.h"
#include "src/core/SensorConfigEeprom.h"
#include "src/menu/ConfigMenu.h"
#include "src/midi/MidiOut.h"
#include "src/led/Led.h"
#include "src/utils/Logger.h"
#include "src/sensors/EncoderSensor.h"
#include "src/sensors/PotentiometerSensor.h"
#include "src/sensors/ToggleSwitchSensor.h"
#include "src/sensors/UltrasonicSensor.h"


#define ENABLE_SERIAL_LOGGING   true

// ============================================================

MidiOut midiOut(&Serial, ENABLE_SERIAL_LOGGING);
SensorManager sensorManager;
Logger logger(Serial, Logger::INFO, ENABLE_SERIAL_LOGGING);
SensorConfigEeprom sensorConfigStore(&logger);
Led ledStatus(PIN_LED_STATUS);
ConfigMenu configMenu(logger, Serial, ledStatus, &sensorConfigStore);

// Active sensors
#ifdef VOUNCE_ENCODER_SENSOR
EncoderSensor encoderSensor(PIN_CLK, PIN_DT, 10, 0, 64, 1, 0, 127);
ToggleSwitchSensor switchSensor(PIN_SW, 11, 1, 0, 127, 100);
#endif 

#ifdef VOUNCE_ULTRASONIC_SENSOR
UltrasonicSensor distanceSensor(PIN_TRIG, PIN_ECHO, 13, 10, 5, 50, 0, 127, 50, 1);
#endif
    
#ifdef VOUNCE_POTENTIOMETER_SENSOR
PotentiometerSensor potSensor(PIN_POT, 12, 7, 0, 1023, 0, 127, 1);
#endif


void setup() {
    midiOut.begin(115200);
    ledStatus.begin();
    ledStatus.setMode(Led::On);
    
#ifdef VOUNCE_ENCODER_SENSOR
    encoderSensor.setLockWhilePinLow(PIN_SW);
    sensorManager.addSensor(encoderSensor);
    sensorManager.addSensor(switchSensor);
    configMenu.addSensor(encoderSensor);
    configMenu.addSensor(switchSensor);
    sensorConfigStore.addSensor(encoderSensor);
    sensorConfigStore.addSensor(switchSensor);
#endif

#ifdef VOUNCE_POTENTIOMETER_SENSOR
    sensorManager.addSensor(potSensor);
    configMenu.addSensor(potSensor);
    sensorConfigStore.addSensor(potSensor);
#endif

#ifdef VOUNCE_ULTRASONIC_SENSOR
    sensorManager.addSensor(distanceSensor);
    configMenu.addSensor(distanceSensor);
    sensorConfigStore.addSensor(distanceSensor);
#endif


    sensorConfigStore.begin();
    sensorManager.begin();
    configMenu.begin();
    
    sensorManager.setEnabled(true);
    ledStatus.setMode(Led::Off);
}

void loop() {
    configMenu.update();
    ledStatus.update();
    sensorManager.updateAndSend(midiOut);
}
