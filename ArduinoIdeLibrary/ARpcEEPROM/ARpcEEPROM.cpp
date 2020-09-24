#include "ARpcEEPROM.h"
#include <EEPROM.h>

ARpcEEPROM::ARpcEEPROM(unsigned int maxArgsCount)
{
	notes=0;
	count=0;
	usedCount=0;
	argOffset=1;
	isValidConfig=false;
	notes=new Note[maxArgsCount];
	for(unsigned int i=0;i<maxArgsCount;++i)
	{
		notes[i].type=NONE;
		notes[i].offset=0;
	}
}

ARpcEEPROM::~ARpcEEPROM()
{
	delete[] notes;
}

void ARpcEEPROM::setup(size_t sz)
{
#ifdef ARDUINO_ESP8266_RELEASE
	EEPROM.begin(sz);
#endif
}

void ARpcEEPROM::addRawArg(int sz,uint8_t *data)
{
	if(usedCount>=count)return;
	if(sz<0)sz=0;
	notes[usedCount].type=RAW;
	notes[usedCount].size=sz;
	notes[usedCount].data=data;
	notes[usedCount].offset=argOffset;
	++usedCount;
	argOffset+=sz;
}

void ARpcEEPROM::addStringArg(int sz,char *str)
{
	if(usedCount>=count)return;
	if(sz<0)sz=0;
	notes[usedCount].type=STRING;
	notes[usedCount].size=sz;
	notes[usedCount].data=str;
	notes[usedCount].offset=argOffset;
	++usedCount;
	argOffset+=sz;
}

void ARpcEEPROM::addIntArg(int *num)
{
	if(usedCount>=count)return;
	notes[usedCount].type=INT;
	notes[usedCount].size=sizeof(int);
	notes[usedCount].data=num;
	notes[usedCount].offset=argOffset;
	++usedCount;
	argOffset+=sizeof(int);
}

void ARpcEEPROM::addFloatArg(float *num)
{
	if(usedCount>=count)return;
	notes[usedCount].type=FLOAT;
	notes[usedCount].size=sizeof(float);
	notes[usedCount].data=num;
	notes[usedCount].offset=argOffset;
	++usedCount;
	argOffset+=sizeof(float);
}

void ARpcEEPROM::addBoolArg(uint8_t *flag)
{
	if(usedCount>=count)return;
	notes[usedCount].type=BOOL;
	notes[usedCount].size=1;
	notes[usedCount].data=flag;
	notes[usedCount].offset=argOffset;
	++usedCount;
	argOffset+=1;
}

void ARpcEEPROM::writeOneArg(int index)
{
	if(index>=count)return;
	Note *n=notes+index;
	if(n->type==NONE)return;
	const uint8_t *v=(const uint8_t*)n->data;
	for(int i=0;i<n->size;++i)
		EEPROM.write(n->offset+i,v[i]);
#ifdef ARDUINO_ESP8266_RELEASE
	EEPROM.commit();
#endif
}

void ARpcEEPROM::writeConfig()
{
	EEPROM.write(0,0xaa);
	for(int i=0;i<usedCount;++i)
	{
		Note *n=notes+i;
		const uint8_t *v=(const uint8_t*)n->data;
		for(int j=0;j<n->size;++j)
			EEPROM.write(n->offset+j,v[j]);
	}
#ifdef ARDUINO_ESP8266_RELEASE
	EEPROM.commit();
#endif
}

bool ARpcEEPROM::readConfig()
{
	if(EEPROM.read(0)!=0xaa)
		return false;
	for(int i=0;i<usedCount;++i)
	{
		Note *n=notes+i;
		uint8_t *v=(uint8_t*)n->data;
		for(int j=0;j<n->size;++j)
			v[j]=EEPROM.read(n->offset+j);
		if(n->type==STRING)
			v[n->size]=0;
	}
	return true;
}

bool ARpcEEPROM::isValid()
{
	return isValidConfig;
}
