
#pragma once

// ============================================================
// PIN CONFIGURATION 
// ============================================================

// Pro Micro 5V/16MHz Alternative Pinout
// Pin availability: D2-D7, D14-D16, A0-A3, A6, A7
// Note: D0/D1 are RX/TX (avoid), D8-D13 not available on Pro Micro

// KY-040 Encoder pins
#define PIN_CLK   2
#define PIN_DT    3
#define PIN_SW    4

// LED pins
#define PIN_LED_STATUS  6   // D6 - Allumée quand menu ouvert, clignote en settings

// Optional sensor pins
#define PIN_POT   A0      // A0
#define PIN_TRIG  14      // D14 (MISO) - Ultrasonic trigger
#define PIN_ECHO  15      // D15 (MOSI) - Ultrasonic echo


#if (defined(USBCON) || defined(ARDUINO_AVR_PROMICRO) || defined(ARDUINO_AVR_LEONARDO) || defined(ARDUINO_AVR_MICRO))
#define VOUNCE_USE_MIDIUSB
#endif
