#pragma once

// ============================================================
// PIN CONFIGURATION - Choose one of the following
// ============================================================
// ARDUINO UNO / NANO (default)
//#define PINOUT_UNO_NANO
#define PINOUT_PRO_MICRO

#if defined(PINOUT_PRO_MICRO) && (defined(USBCON) || defined(ARDUINO_AVR_PROMICRO) || defined(ARDUINO_AVR_LEONARDO) || defined(ARDUINO_AVR_MICRO))
#define VOUNCE_USE_MIDIUSB
#endif




#ifdef PINOUT_UNO_NANO
// KY-040 Encoder pins
#define PIN_CLK   3
#define PIN_DT    2
#define PIN_SW    4

// Menu button pin
#define PIN_MENU  8

// LED pins
#define PIN_LED_MENU  9   // Allumée quand menu ouvert, clignote en settings
#define PIN_LED_IDLE  10  // Allumée quand menu fermé

// Optional sensor pins
#define PIN_POT   A0
#define PIN_TRIG  6
#define PIN_ECHO  5

#endif // PINOUT_UNO_NANO

#ifdef PINOUT_PRO_MICRO
// Pro Micro 5V/16MHz Alternative Pinout
// Pin availability: D2-D7, D14-D16, A0-A3, A6, A7
// Note: D0/D1 are RX/TX (avoid), D8-D13 not available on Pro Micro

// KY-040 Encoder pins
#define PIN_CLK   2
#define PIN_DT    3
#define PIN_SW    4

// Menu button pin
#define PIN_MENU  5   // D5

// LED pins
#define PIN_LED_MENU  6   // D6 - Allumée quand menu ouvert, clignote en settings
#define PIN_LED_IDLE  7   // D7 - Allumée quand menu fermé

// Optional sensor pins
#define PIN_POT   A0      // A0
#define PIN_TRIG  14      // D14 (MISO) - Ultrasonic trigger
#define PIN_ECHO  15      // D15 (MOSI) - Ultrasonic echo

#endif // PINOUT_PRO_MICRO