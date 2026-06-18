#include "ConfigMenu.h"

ConfigMenu::ConfigMenu(
    Logger& logger,
    Stream& commandStream,
    Led& ledStatus,
    SensorConfigEeprom* configStore)
    : logger(logger),
      commandStream(commandStream),
      ledStatus(ledStatus),
      configStore(configStore),
      sensors{nullptr},
      sensorCount(0),
      mode(SelectSensor),
      selectedSensor(0),
      selectedParam(0),
      serialCommandBuffer{0},
      serialCommandLength(0) {}

void ConfigMenu::begin() {
    printSerialHelp();
}

bool ConfigMenu::addSensor(IConfigurableSensor& sensor) {
    if (sensorCount >= MaxSensors) {
        return false;
    }

    sensors[sensorCount] = &sensor;
    sensorCount++;
    return true;
}

void ConfigMenu::update() {
    processSerialInput();
}

void ConfigMenu::processSerialInput() {
    while (commandStream.available() > 0) {
        char c = (char)commandStream.read();

        if (c == '\n' || c == '\r') {
            if (serialCommandLength == 0) {
                continue;
            }

            serialCommandBuffer[serialCommandLength] = '\0';
            executeSerialCommand(String(serialCommandBuffer));
            serialCommandLength = 0;
            serialCommandBuffer[0] = '\0';
            continue;
        }

        if (serialCommandLength < SerialCommandMaxLen) {
            serialCommandBuffer[serialCommandLength++] = c;
        }
    }
}

void ConfigMenu::executeSerialCommand(const String& command) {
    String cmd = command;
    cmd.trim();
    cmd.toLowerCase();

    logger.info(String("Received: ") + cmd );

    if (cmd.length() == 0) {
        return;
    }

    if (cmd == "h") {
        printSerialHelp();
        printState();
        return;
    }

    if (cmd == "ls") {
        mode = SelectSensor;
        
        logger.info(String("type \"s x\" with x being the sensor index"));
        printState();
        return;
    }

    if (cmd.startsWith("s ")) {
        String arg = cmd.substring(1);
        arg.trim();
        if (arg.length() == 0) {
            logger.info("Usage: s sensor_index");
            return;
        }

        int index = arg.toInt();
        if (index < 1 || index > sensorCount) {
            logger.info(String("Invalid sensor index: ") + arg);
            logger.info(String("Valid range: 1..") + String(sensorCount));
            return;
        }

        logger.info(String("type \"p x\" with x being the parameter index"));
        selectedSensor = (uint8_t)(index - 1);
        selectedParam = 0;
        mode = SelectParam;
        printState();
        return;
    }

    if (cmd.startsWith("p ")) {
        if (sensorCount == 0) {
            logger.info("No sensor available");
            return;
        }

        IConfigurableSensor* sensor = sensors[selectedSensor];
        uint8_t parameterCount = sensor->getParameterCount();
        String arg = cmd.substring(1);
        arg.trim();
        if (arg.length() == 0) {
            logger.info("Usage: p param_index");
            return;
        }

        int index = arg.toInt();
        if (index < 1 || index > parameterCount) {
            logger.info(String("Invalid param index: ") + arg);
            logger.info(String("Valid range: 1..") + String(parameterCount));
            return;
        }

        logger.info(String("type \"v x\" with x being the parameter value"));
        selectedParam = (uint8_t)(index - 1);
        mode = EditParam;
        printState();
        return;
    }

    if (cmd == "v") {
        if (sensorCount == 0) {
            logger.info("No sensor available");
            return;
        }

        IConfigurableSensor* sensor = sensors[selectedSensor];
        SensorParameterMeta meta;
        if (!sensor->getParameterMeta(selectedParam, meta)) {
            logger.info("Selected parameter is invalid");
            return;
        }

        int value = sensor->getParameterValue(selectedParam);
        logger.info(String(sensor->getSensorName()) + "." + String(meta.name) +
                    " = " + String(value) +
                    " (range " + String(meta.minValue) + ".." + String(meta.maxValue) + ")");
        logger.info("Usage: v value");
        return;
    }

    if (cmd.startsWith("v ")) {
        if (sensorCount == 0) {
            logger.info("No sensor available");
            return;
        }

        IConfigurableSensor* sensor = sensors[selectedSensor];
        String arg = cmd.substring(1);
        arg.trim();
        if (arg.length() == 0) {
            logger.info("Usage: v <value>");
            return;
        }

        SensorParameterMeta meta;
        if (!sensor->getParameterMeta(selectedParam, meta)) {
            logger.info("Selected parameter is invalid");
            return;
        }

        int value = arg.toInt();
        int next = constrain(value, meta.minValue, meta.maxValue);
        sensor->setParameterValue(selectedParam, next);
        if (configStore != nullptr) {
            if (configStore->saveParameter(selectedSensor, selectedParam)) {
                ledStatus.blink(1000);
            } else {
                logger.warning("Failed to save parameter to EEPROM");
            }
        }
        mode = SelectParam;
        printState();
        return;
    }

    logger.info(String("Unknown menu command: ") + cmd);
    printSerialHelp();
}

void ConfigMenu::printSerialHelp() {
    logger.info("Menu serial commands:");
    logger.info("  ls    -> list sensors");
    logger.info("  s [i] -> list sensors or select sensor index (1-based)");
    logger.info("  p [i] -> list params or select parameter index (1-based)");
    logger.info("  v [n] -> show current value or set value");
    logger.info("  h     -> print this help");
}

void ConfigMenu::printState() {
    if (sensorCount == 0) {
        logger.info("No sensor available");
        return;
    }

    if (mode == SelectSensor) {
        for (uint8_t i = 0; i < sensorCount; ++i) {
            IConfigurableSensor* sensor = sensors[i];
            String msg = "["+String(i + 1)+"] => " + String("Sensor: ") + sensor->getSensorName() + 
                        " (" + String((int)i + 1) + "/" + String((int)sensorCount) + ")";
            logger.info(msg);
        }
        if(sensorCount > 1) {
            return;
        }
    }

    IConfigurableSensor* sensor = sensors[selectedSensor];
    if (mode == SelectParam) {
        for (uint8_t i = 0; i < sensor->getParameterCount(); ++i) {
            SensorParameterMeta meta;
            if (!sensor->getParameterMeta(i, meta)) {
                continue;
            }
            int value = sensor->getParameterValue(i);
            String msg = "["+String(i + 1)+"] => " + String(sensor->getSensorName()) + "." + String(meta.name) + 
                        " = " + String(value);
            logger.info(msg);
        }
    } else if (mode == EditParam){
        SensorParameterMeta meta;
        if (!sensor->getParameterMeta(selectedParam, meta)) {
            return;
        }

        int value = sensor->getParameterValue(selectedParam);
        String msg = String(sensor->getSensorName()) + "." + String(meta.name) + " = " + String(value);
        logger.info(msg);
    }
}
