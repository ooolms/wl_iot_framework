#include "ARpcMessageParserTests.h"
#include "ARpcMessageParser.h"
#include "ARpcMessage.h"

ARpcMessageParserTests::ARpcMessageParserTests(QObject *parent)
	:QtUnitTestSet("ARpcMessageParserTests",parent)
{
}

void ARpcMessageParserTests::testParseAndDump()
{
	ARpcConfig cfg;
	ARpcMessageParser mParser(cfg);
	ARpcMessage m;
	m=mParser.parse("cmd|arg1|arg2|arg3");
	COMPARE(m.title,QString("cmd"))
	COMPARE(m.args.count(),3)
	COMPARE(m.args[0],QString("arg1"))
	COMPARE(m.args[1],QString("arg2"))
	COMPARE(m.args[2],QString("arg3"))
	COMPARE("cmd|arg1|arg2|arg3\n",mParser.dump(m))
}
