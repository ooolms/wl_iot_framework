#include "ARpcUuidTest.h"
#include "ARpcUuid.h"

ARpcUuidTest::ARpcUuidTest(QObject *parent)
	:QtUnitTestSet("ARpcUuidTest",parent)
{
	id=QUuid::createUuid();
	addTest((TestFunction)&ARpcUuidTest::testParseStr,"Test parse from {} format");
	addTest((TestFunction)&ARpcUuidTest::testParseHex,"Test parse from hex format");
	addTest((TestFunction)&ARpcUuidTest::testIsValid,"Test isValid function");
}

void ARpcUuidTest::testIsValid()
{
	ARpcUuid id2;
	VERIFY(!id2.isValid())
	id2.parse(QUuid().toByteArray());
	VERIFY(!id2.isValid())
	id2.parse(id.toByteArray());
	VERIFY(id2.isValid())
}

void ARpcUuidTest::testParseStr()
{
	ARpcUuid id2(id.toByteArray().constData());
	VERIFY(id2.isValid())
	char str[39];
	memset(str,0,39);
	id2.toString(str);
	COMPARE(QByteArray(str),id.toByteArray())
	memset(str,0,39);
	id2.toHex(str);
	COMPARE(QByteArray(str),id.toRfc4122().toHex())
}

void ARpcUuidTest::testParseHex()
{
	ARpcUuid id2(id.toRfc4122().toHex());
	VERIFY(id2.isValid())
	char str[39];
	memset(str,0,39);
	id2.toString(str);
	COMPARE(QByteArray(str),id.toByteArray())
	memset(str,0,39);
	id2.toHex(str);
	COMPARE(QByteArray(str),id.toRfc4122().toHex())
}
