#include "ARpcEEPROMTest.h"
#include "ARpcEEPROM.h"

ARpcEEPROMTest::ARpcEEPROMTest(QObject *parent)
	:QtUnitTestSet("ARpcEEPROMTest",parent)
{
	addTest((TestFunction)&ARpcEEPROMTest::testReadWriteConfig,"Test read/write EEPROM config");
}

void ARpcEEPROMTest::testReadWriteConfig()
{
	char str1[21];
	strcpy(str1,"Some text");
	int i1=5;
	float f1=10.0;
	uint8_t b1=true;
	ARpcEEPROM r(4);
	r.addStringArg(20,str1);
	r.addIntArg(&i1);
	r.addFloatArg(&f1);
	r.addBoolArg(&b1);
}
