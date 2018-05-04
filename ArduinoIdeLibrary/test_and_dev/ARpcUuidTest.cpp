#include "ARpcUuidTest.h"
#include "ARpcUuid.h"

ARpcUuidTest::ARpcUuidTest(QObject *parent)
	:QtUnitTestSet("ARpcUuidTest",parent)
{
	id=QUuid::createUuid();
	addTest((TestFunction)&ARpcUuidTest::test1,"Test parse from {} format");
	addTest((TestFunction)&ARpcUuidTest::test2,"Test parse from hex format");
}

void ARpcUuidTest::test1()
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

void ARpcUuidTest::test2()
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
