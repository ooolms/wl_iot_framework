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

//CRIT test escaping !!!
ARpcStreamParserTests::ARpcStreamParserTests(QObject *parent)
	:QtUnitTestSet("ARpcStreamParserTests",parent)
{
	addTest((TestFunction)&ARpcStreamParserTests::testParsing,"test parsing");
	addTest((TestFunction)&ARpcStreamParserTests::testEscaping,"test escaping");
	addTest((TestFunction)&ARpcStreamParserTests::testEscapingHex,"test escaping hex");
	addTest((TestFunction)&ARpcStreamParserTests::testStreamReset,"test resetting parser by 0 char");
}

void ARpcStreamParserTests::testParsing()
{
	ARpcStreamParser parser;
	bool wasMsg=false;
	ARpcMessage msg;
	connect(&parser,&ARpcStreamParser::processMessage,[&wasMsg,&msg](const ARpcMessage &m)
	{
		wasMsg=true;
		msg=m;
	});
	parser.pushData("cmd|arg1|ar");
	parser.pushData("g2|arg3\n");
	VERIFY(wasMsg)
	COMPARE(msg.title,"cmd")
	COMPARE(msg.args.count(),3)
	COMPARE(msg.args[0],"arg1")
	COMPARE(msg.args[1],"arg2")
	COMPARE(msg.args[2],"arg3")
}

void ARpcStreamParserTests::testEscaping()
{
	ARpcStreamParser parser;
	bool wasMsg=false;
	ARpcMessage msg;
	connect(&parser,&ARpcStreamParser::processMessage,[&wasMsg,&msg](const ARpcMessage &m)
	{
		wasMsg=true;
		msg=m;
	});
	parser.pushData("cmd|ar\\\\g1|a\\|rg2|ar\\ng3\n");
	VERIFY(wasMsg)
	COMPARE(msg.title,"cmd")
	COMPARE(msg.args.count(),3)
	COMPARE(msg.args[0],"ar\\g1")
	COMPARE(msg.args[1],"a|rg2")
	COMPARE(msg.args[2],"ar\ng3")
}

void ARpcStreamParserTests::testEscapingHex()
{
	ARpcStreamParser parser;
	bool wasMsg=false;
	ARpcMessage msg;
	connect(&parser,&ARpcStreamParser::processMessage,[&wasMsg,&msg](const ARpcMessage &m)
	{
		wasMsg=true;
		msg=m;
	});
	parser.pushData("cmd|arg1|arg\\x2f\n");
	VERIFY(wasMsg)
	COMPARE(msg.title,"cmd")
	COMPARE(msg.args.count(),2)
	COMPARE(msg.args[0],"arg1")
	COMPARE(msg.args[1],"arg/")
}

void ARpcStreamParserTests::testStreamReset()
{
	ARpcStreamParser parser;
	bool wasMsg=false;
	ARpcMessage msg;
	connect(&parser,&ARpcStreamParser::processMessage,[&wasMsg,&msg](const ARpcMessage &m)
	{
		wasMsg=true;
		msg=m;
	});
	QByteArray data;
	data.append("cmddddd|aaaarrrrgggg");
	data.append('\0');
	data.append("cmd|arg1\n");
	parser.pushData(data);
	VERIFY(wasMsg)
	COMPARE(msg.title,"cmd")
	COMPARE(msg.args.count(),1)
	COMPARE(msg.args[0],"arg1")
}

