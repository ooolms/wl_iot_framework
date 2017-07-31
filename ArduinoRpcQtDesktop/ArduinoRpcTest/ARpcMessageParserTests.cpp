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

#include "ARpcMessageParserTests.h"
#include "ARpcBase/ARpcMessageParser.h"
#include "ARpcBase/ARpcMessage.h"

ARpcMessageParserTests::ARpcMessageParserTests(QObject *parent)
	:QtUnitTestSet("ARpcMessageParserTests",parent)
{
	addTest((TestFunction)&ARpcMessageParserTests::testParseAndDump,"Test parse and dump");
}

void ARpcMessageParserTests::testParseAndDump()
{
	ARpcMessageParser mParser;
	ARpcMessage m;
	m=mParser.parse("cmd|arg1|arg2|arg3");
	COMPARE(m.title,QString("cmd"));
	COMPARE(m.args.count(),3);
	COMPARE(m.args[0],QString("arg1"));
	COMPARE(m.args[1],QString("arg2"));
	COMPARE(m.args[2],QString("arg3"));
	COMPARE("cmd|arg1|arg2|arg3",mParser.dump(m));
}
