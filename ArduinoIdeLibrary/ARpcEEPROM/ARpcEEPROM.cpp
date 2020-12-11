#include "ARpcEEPROM.h"
#include <EEPROM.h>

ARpcEEPROM::ARpcEEPROM()
{
	startNote=0;
	argOffset=1;
	count=0;
	isValidConfig=false;
}

ARpcEEPROM::~ARpcEEPROM()
{
	if(startNote)
		delete startNote;
}

void ARpcEEPROM::setup(size_t sz)
{
#ifdef ARDUINO_ESP8266_RELEASE
	EEPROM.begin(sz);
#endif
}

void ARpcEEPROM::addRawArg(int sz,uint8_t *data)
{
	if(sz<0)sz=0;
	Note *n=mkNote();
	n->type=RAW;
	n->size=sz;
	n->data=data;
	n->offset=argOffset;
	++count;
	argOffset+=sz;
}

void ARpcEEPROM::addStringArg(int sz,char *str)
{
	if(sz<0)sz=0;
	Note *n=mkNote();
	n->type=STRING;
	n->size=sz;
	n->data=str;
	n->offset=argOffset;
	++count;
	argOffset+=sz;
}

void ARpcEEPROM::addIntArg(int *num)
{
	Note *n=mkNote();
	n->type=INT;
	n->size=sizeof(int);
	n->data=num;
	n->offset=argOffset;
	++count;
	argOffset+=sizeof(int);
}

void ARpcEEPROM::addFloatArg(float *num)
{
	Note *n=mkNote();
	n->type=FLOAT;
	n->size=sizeof(float);
	n->data=num;
	n->offset=argOffset;
	++count;
	argOffset+=sizeof(float);
}

void ARpcEEPROM::addBoolArg(uint8_t *flag)
{
	Note *n=mkNote();
	n->type=BOOL;
	n->size=1;
	n->data=flag;
	n->offset=argOffset;
	++count;
	argOffset+=1;
}

void ARpcEEPROM::writeOneArg(int index)
{
	if(index>=count)return;
	Note *n=at(index);
	if(!n)return;
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
	Note *n=startNote;
	while(n)
	{
		const uint8_t *v=(const uint8_t*)n->data;
		for(int i=0;i<n->size;++i)
			EEPROM.write(n->offset+i,v[i]);
		n=n->next;
	}
#ifdef ARDUINO_ESP8266_RELEASE
	EEPROM.commit();
#endif
}

bool ARpcEEPROM::readConfig()
{
	if(EEPROM.read(0)!=0xaa)
		return false;
	Note *n=startNote;
	while(n)
	{
		uint8_t *v=(uint8_t*)n->data;
		for(int i=0;i<n->size;++i)
			v[i]=EEPROM.read(n->offset+i);
		if(n->type==STRING)
			v[n->size]=0;
		n=n->next;
	}
	return true;
}

bool ARpcEEPROM::isValid()
{
	return isValidConfig;
}

ARpcEEPROM::Note* ARpcEEPROM::at(int index)
{
	Note* n=startNote;
	for(int i=0;i<index;++i)
	{
		if(!n)return 0;
		n=n->next;
	}
	return n;
}

ARpcEEPROM::Note* ARpcEEPROM::mkNote()
{
	Note *n=new Note;
	if(!startNote)
		startNote=lastNote=n;
	else
	{
		lastNote->next=n;
		lastNote=n;
	}
	return n;
}
