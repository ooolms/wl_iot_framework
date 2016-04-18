#include "ARpcStreamParserTests.h"
#include "ARpcStreamParser.h"

ARpcStreamParserTests::ARpcStreamParserTests(QObject *parent)
	:QtUnitTestSet("ARpcStreamParserTests",parent)
{
	addTest((TestFunction)&ARpcStreamParserTests::testParsing,"test parsing");
}

void ARpcStreamParserTests::testParsing()
{
	ARpcConfig cfg;
	ARpcMessageParser mParser(cfg);
	ARpcStreamParser parser(cfg,&mParser);
	bool wasMsg=false;
	ARpcMessage msg;
	QMetaObject::Connection conn=connect(&parser,&ARpcStreamParser::processMessage,[&wasMsg,&msg](const ARpcMessage &m){
		wasMsg=true;
		msg=m;
	});
	parser.pushData("cmd|arg1|ar");
	parser.pushData("g2|arg3\n");
	VERIFY(wasMsg)
	COMPARE(msg.title,QString("cmd"))
	COMPARE(msg.args.count(),3)
	COMPARE(msg.args[0],QString("arg1"))
	COMPARE(msg.args[1],QString("arg2"))
	COMPARE(msg.args[2],QString("arg3"))
	disconnect(conn);
}

