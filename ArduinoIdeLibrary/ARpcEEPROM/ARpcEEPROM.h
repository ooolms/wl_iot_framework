#ifndef ARPCEEPROM_H
#define ARPCEEPROM_H

#include <Arduino.h>

class ARpcEEPROM
{
	enum ArgType
	{
		NONE,
		RAW,
		STRING,
		FLOAT,
		INT,
		BOOL
	};
	struct Note
	{
		ArgType type;
		int offset;
		void *data;
		int size;
	};

public:
	explicit ARpcEEPROM(unsigned int maxArgsCount);
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
	Note *notes;
	uint16_t count;
	uint16_t usedCount;
	int argOffset;
	bool isValidConfig;
};

#endif
