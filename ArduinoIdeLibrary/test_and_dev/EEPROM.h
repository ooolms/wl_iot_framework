#ifndef EEPROM_H
#define EEPROM_H

#include <stdint.h>

class EEPROM_Fake
{
public:
	EEPROM_Fake();
	~EEPROM_Fake();

public:
	uint8_t read(int addr);
	void write(int addr,uint8_t val);

private:
	uint8_t *data;
};

extern EEPROM_Fake EEPROM;

#endif // EEPROM_H
