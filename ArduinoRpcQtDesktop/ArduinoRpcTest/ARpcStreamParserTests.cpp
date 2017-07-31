/*******************************************
Copyright 2017 OOO "LMS"

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

	http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.*/

#include "ARpcStreamParserTests.h"
#include "ARpcBase/ARpcStreamParser.h"

ARpcStreamParserTests::ARpcStreamParserTests(QObject *parent)
	:QtUnitTestSet("ARpcStreamParserTests",parent)
{
	addTest((TestFunction)&ARpcStreamParserTests::testParsing,"test parsing");
}

void ARpcStreamParserTests::testParsing()
{
	ARpcStreamParser parser;
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

