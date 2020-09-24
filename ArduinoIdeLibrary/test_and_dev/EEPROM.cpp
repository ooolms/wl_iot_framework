#include "EEPROM.h"
#include <string.h>

EEPROM_Fake EEPROM;

EEPROM_Fake::EEPROM_Fake()
{
	data=new uint8_t[1024];
	memset(data,0,1024);
}

EEPROM_Fake::~EEPROM_Fake()
{
	delete[] data;
}

uint8_t EEPROM_Fake::read(int addr)
{
	return data[addr];
}

void EEPROM_Fake::write(int addr,uint8_t val)
{
	data[addr]=val;
}
