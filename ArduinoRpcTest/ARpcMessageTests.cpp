#include "ARpcMessageTests.h"
#include "ARpcMessage.h"

ARpcMessageTests::ARpcMessageTests(QObject *parent)
	:QtUnitTestSet("ARpcMessageTests",parent)
{
	addTest((TestFunction)&ARpcMessageTests::testParseAndDump,"Test parse and dump");
}

void ARpcMessageTests::testParseAndDump()
{
	ARpcMessage m;
	m.parse("cmd|arg1|arg2|arg3");
	COMPARE(m.title,QString("cmd"))
	COMPARE(m.args.count(),3)
	COMPARE(m.args[0],QString("arg1"))
	COMPARE(m.args[1],QString("arg2"))
	COMPARE(m.args[2],QString("arg3"))
	COMPARE("cmd|arg1|arg2|arg3",m.dump())
}

