#include "LedIndicator.h"
#include "../menu/ConfigMenu.h"

LedManager::LedManager(uint8_t ledMenuPin, uint8_t ledIdlePin, ConfigMenu& menu)
    : ledMenu(ledMenuPin),
      ledIdle(ledIdlePin),
      menu(menu),
      enabled(true) {}

void LedManager::begin() {
    ledMenu.begin();
    ledIdle.begin();
}

void LedManager::update() {
    if (!enabled) {
        ledMenu.setMode(Led::Off);
        ledIdle.setMode(Led::Off);
        ledMenu.update();
        ledIdle.update();
        return;
    }

    bool menuIsOpen = menu.isOpen();
    bool isEditing = menu.isEditing();

    // LED Menu: allumée si menu ouvert, clignote si on édite
    if (!menuIsOpen) {
        ledMenu.setMode(Led::Off);
    } else if (isEditing) {
        ledMenu.setMode(Led::Blink);
        ledMenu.setBlinkPeriod(300);
    } else {
        ledMenu.setMode(Led::On);
    }

    // LED Idle: allumée si menu fermé
    if (menuIsOpen) {
        ledIdle.setMode(Led::Off);
    } else {
        ledIdle.setMode(Led::On);
    }

    ledMenu.update();
    ledIdle.update();
}

void LedManager::setEnabled(bool value) {
    enabled = value;
}
