#ifndef ARDUINO_H
#define ARDUINO_H

#include "EEPROM.h"
#include <string.h>

unsigned long millis();

typedef unsigned char __FlashStringHelper;

#define PSTR(S) (S)
#define F(S) (S)
#define strcmp_P strcmp

#endif // ARDUINO_H
