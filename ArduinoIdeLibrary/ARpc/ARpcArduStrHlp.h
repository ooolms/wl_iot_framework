#ifndef ARPCARDUSTRHLP_H
#define ARPCARDUSTRHLP_H

#include <string.h>

#ifdef ARDUINO
#include <Arduino.h>
#define use_strcmp strcmp_P
#else
#define F(S) (S)
#define PSTR(S) (S)
#define use_strcmp strcmp
#endif

#endif // ARPCARDUSTRHLP_H
