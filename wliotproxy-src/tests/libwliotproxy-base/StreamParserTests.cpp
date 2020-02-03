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

#include "StreamParserTests.h"
#include "wliot/devices/StreamParser.h"

//TODO test tryParse function
StreamParserTests::StreamParserTests(QObject *parent)
	:QtUnitTestSet("StreamParserTests",parent)
{
	addTest((TestFunction)&StreamParserTests::testParsing,"test parsing");
	addTest((TestFunction)&StreamParserTests::testEscaping,"test escaping");
	addTest((TestFunction)&StreamParserTests::testEscapingHex,"test escaping hex");
	addTest((TestFunction)&StreamParserTests::testStreamReset,"test resetting parser by 0 char");
}

void StreamParserTests::testParsing()
{
	StreamParser parser;
	bool wasMsg=false;
	Message msg;
	connect(&parser,&StreamParser::newMessage,[&wasMsg,&msg](const Message &m)
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

void StreamParserTests::testEscaping()
{
	StreamParser parser;
	bool wasMsg=false;
	Message msg;
	connect(&parser,&StreamParser::newMessage,[&wasMsg,&msg](const Message &m)
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

void StreamParserTests::testEscapingHex()
{
	StreamParser parser;
	bool wasMsg=false;
	Message msg;
	connect(&parser,&StreamParser::newMessage,[&wasMsg,&msg](const Message &m)
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

void StreamParserTests::testStreamReset()
{
	StreamParser parser;
	bool wasMsg=false;
	Message msg;
	connect(&parser,&StreamParser::newMessage,[&wasMsg,&msg](const Message &m)
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

