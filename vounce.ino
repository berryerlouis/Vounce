#include <EEPROM.h>
#include "Button.h"


// Pins KY-040
#define PIN_SW  4   // SW
#define PIN_CLK 3   // CLK
#define PIN_DT  2   // DT

// EEPROM addresses
#define ADDR_SENSIVITY      0
#define ADDR_ENCODER_MIN    1
#define ADDR_ENCODER_MAX    2
#define ADDR_SW_VALUE_MIN   3
#define ADDR_SW_VALUE_MAX   4

// MIDI addresses
#define MIDI_CC_0         0
#define MIDI_CHANNEL_10   10

#define MIDI_CC_1         1
#define MIDI_CHANNEL_11   11

// Variables encodeur
int compteur = 127/2;  
int lastStateCLK;
int lastStateDT;

volatile bool buttonHasChanged = false; 
volatile bool encoderHasChanged = false; 

// Paramètres MIDI configurables
byte sensitivity = 0;
byte encoderMin = 0;
byte encoderMax = 127;
byte swValueMin = 0;
byte swValueMax = 127;
byte swValueLast = 0;

// Paramètres menu
bool menuMode = false;
int menuModeItem = 0;

Button myButton(PIN_SW, 1000);

void setup() {
    Serial.begin(115200);

    sensitivity = EEPROM.read(ADDR_SENSIVITY);
    encoderMin = EEPROM.read(ADDR_ENCODER_MIN);
    encoderMax = EEPROM.read(ADDR_ENCODER_MAX);
    swValueMin = EEPROM.read(ADDR_SW_VALUE_MIN);
    swValueMax = EEPROM.read(ADDR_SW_VALUE_MAX);
    if (sensitivity == 255) sensitivity = 1;
    if (encoderMin == 255) encoderMin = 0;
    if (encoderMax == 255) encoderMax = 127;
    if (swValueMin == 255) swValueMin = 0;
    if (swValueMax == 255) swValueMax = 127;

    swValueLast = swValueMin;

    pinMode(PIN_DT, INPUT_PULLUP);
    pinMode(PIN_CLK, INPUT_PULLUP);
    pinMode(LED_BUILTIN, OUTPUT);

    lastStateCLK  = digitalRead(PIN_CLK);
    lastStateDT  = digitalRead(PIN_CLK);

    attachInterrupt(digitalPinToInterrupt(PIN_CLK), onChangeCLK, CHANGE);

    delay(200);
    
    sendCCSwitch(0);
    sendCCEncoder(((encoderMax - encoderMin) / 2) + encoderMin);

    myButton.begin();
    myButton.onShortPress(ButtonShortPress);
    myButton.onLongPress(ButtonLongPress);
}

void loop() {
  myButton.update();
  if (encoderHasChanged == true) {
    updateEncoder();
  }
}

void ButtonShortPress(void) {
  if(menuMode == false) {
    if(swValueLast == swValueMax) {
      sendCCSwitch(swValueMin);
      swValueLast = swValueMin;
    } else{
      sendCCSwitch(swValueMax);
      swValueLast = swValueMax;
    }
  } else {
    if( menuModeItem == 1) {
      encoderMin = compteur;
      menuModeItem ++;
    } else if( menuModeItem == 2) {
      encoderMax = compteur;
      menuModeItem ++;
    } else if( menuModeItem == 3) {
      sensitivity = compteur;
      menuModeItem ++;
    } else if( menuModeItem == 4) {
      swValueMin = compteur;
      menuModeItem ++;
    } else if( menuModeItem == 5) {
      swValueMax = compteur;
      EEPROM.write(ADDR_ENCODER_MIN, encoderMin);
      EEPROM.write(ADDR_ENCODER_MAX, encoderMax);
      EEPROM.write(ADDR_SENSIVITY, sensitivity);
      EEPROM.write(ADDR_SW_VALUE_MIN, swValueMin);
      EEPROM.write(ADDR_SW_VALUE_MAX, swValueMax);
      menuMode = false;
      menuModeItem = 0;
      digitalWrite(LED_BUILTIN, LOW); 
    }
    sendCCSwitch(menuModeItem);
  }
}

void ButtonLongPress(void) {
  menuMode = true;
  menuModeItem = 1;
  digitalWrite(LED_BUILTIN, HIGH);
  sendCCSwitch(menuModeItem);
}

void updateEncoder(void) {
  if(menuMode == false) {
    compteur = constrain(compteur, encoderMin, encoderMax);
  }
  compteur = constrain(compteur, 0, 127);
  sendCCEncoder(compteur);
  encoderHasChanged = false;
}


void onChangeCLK() {
    int stateCLK = digitalRead(PIN_CLK);
    int stateDT  = digitalRead(PIN_DT);

    if((stateCLK == HIGH) && (stateDT == LOW) && (lastStateCLK == LOW) && (lastStateDT == HIGH)) {
      if(menuMode == false) {
        compteur+=sensitivity;  
      } else {
        compteur+= 1;
      }  
      encoderHasChanged = true; 
    }
    if((stateCLK == HIGH) && (stateDT == HIGH) && (lastStateCLK == LOW) && (lastStateDT == LOW)) { 
      if(menuMode == false) {
        compteur-=sensitivity;  
      } else {
        compteur-= 1;
      }  
      encoderHasChanged = true; 
    }
    lastStateCLK = stateCLK;
    lastStateDT = stateDT;
}


void sendCCSwitch(byte state) {
  sendCC(MIDI_CHANNEL_11, MIDI_CC_1, state);
}

void sendCCEncoder(byte value) {
  sendCC(MIDI_CHANNEL_10, MIDI_CC_0, value);
}

void sendCC(byte channel, byte control, byte value) {
    Serial.write(0xB0 | channel);     // CC canal
    Serial.write(control);            // numéro de CC
    Serial.write(value);              // valeur
}
