#pragma once

// ============================================================
// PIN CONFIGURATION - Choose one of the following
// ============================================================



//#define PINOUT_UNO_NANO
#define PINOUT_PRO_MICRO


#ifdef PINOUT_UNO_NANO
#include "pinout_nano.h"
#endif // PINOUT_UNO_NANO

#ifdef PINOUT_PRO_MICRO
#include "pinout_pro.h"
#endif // PINOUT_PRO_MICRO