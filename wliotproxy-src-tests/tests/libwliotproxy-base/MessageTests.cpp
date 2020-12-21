#include "MessageTests.h"
#include "wliot/devices/Message.h"

using namespace WLIOT;

MessageTests::MessageTests(QObject *parent)
	:QtUnitTestSet("TestMessage",parent)
{
	addTest((TestFunction)&MessageTests::testEscape,"Test escape special chars");
	addTest((TestFunction)&MessageTests::testDump,"Test dump msg");
}

void MessageTests::testDump()
{
}

void MessageTests::testEscape()
{
	//msg with backslash
	COMPARE(Message::escape("11\\22"),QByteArray("11\\\\22"))
	//msg with |
	COMPARE(Message::escape("11|22"),QByteArray("11\\|22"))
	//msg with 0 char
	COMPARE(Message::escape(QByteArray("11\00022",5)),QByteArray("11\\\06022",6))
	//msg with \n
	COMPARE(Message::escape("11\n22"),QByteArray("11\\n22"))
}
