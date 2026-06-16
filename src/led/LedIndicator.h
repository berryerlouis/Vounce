#pragma once
#include <Arduino.h>
#include "Led.h"

class ConfigMenu;

class LedManager {
  public:
    // ledMenuPin: s'allume quand menu ouvert, clignote en settings
    // ledIdlePin: s'allume quand menu fermé
    LedManager(uint8_t ledMenuPin, uint8_t ledIdlePin, ConfigMenu& menu);

    void begin();
    void update();

    void setEnabled(bool enabled);

  private:
    Led ledMenu;   // LED pour le menu (allumée/clignotante)
    Led ledIdle;   // LED pour l'état inactif
    ConfigMenu& menu;
    bool enabled;
};
