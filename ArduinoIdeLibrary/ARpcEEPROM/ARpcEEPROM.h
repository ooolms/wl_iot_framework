#ifndef ARPCEEPROM_H
#define ARPCEEPROM_H

#include <Arduino.h>

class ARpcEEPROM
{
	enum ArgType
	{
		RAW,
		STRING,
		FLOAT,
		INT,
		BOOL
	};

	class Note
	{
	public:
		Note()
		{
			data=0;
			next=0;
		}
		~Note()
		{
			if(next)
				delete next;
		}

		ArgType type;
		int offset;
		void *data;
		int size;
		Note *next;
	};

public:
	explicit ARpcEEPROM();
	~ARpcEEPROM();
	void setup(size_t sz=1024);//for ESP8266
	void addRawArg(int sz,uint8_t *data);
	void addStringArg(int sz,char *str);//mem for string - sz+1 (end zero char)
	void addIntArg(int *num);
	void addFloatArg(float *num);
	void addBoolArg(uint8_t *flag);
	void writeOneArg(int index);
	void writeConfig();
	bool readConfig();
	bool isValid();

private:
	Note* at(int index);
	Note* mkNote();

private:
	Note *startNote;
	Note *lastNote;
	uint16_t count;
	int argOffset;
	bool isValidConfig;
};

#endif
