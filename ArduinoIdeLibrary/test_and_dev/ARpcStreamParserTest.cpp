#include "ARpcStreamParserTest.h"

ARpcStreamParserTest::ARpcStreamParserTest(QObject *parent)
	:QtUnitTestSet("ARpcStreamParserTest",parent)
	,testObj(100)
{
	addTest((TestFunction)&ARpcStreamParserTest::testSimple,"Simple parsing test");
	addTest((TestFunction)&ARpcStreamParserTest::testEscapedSyms,"Test parsing escaped sequences");
	addTest((TestFunction)&ARpcStreamParserTest::testSimple,"Test parsing hex encoded chars");
	addTest((TestFunction)&ARpcStreamParserTest::testReset,"Test reset parser on 0-byte");
	addTest((TestFunction)&ARpcStreamParserTest::testParserBufferOverflow,"Test reset parser on buffer overflow");
}

void ARpcStreamParserTest::testSimple()
{
	testObj.prepareToTest("cmd",QByteArrayList()<<"arg1"<<"arg2"<<"arg3");
	QByteArray data("cmd|arg1|arg2|arg3\n");
	testObj.parser.putData(data.constData(),data.size());
	VERIFY(testObj.wasMsg)
	VERIFY(testObj.testResult)
}

void ARpcStreamParserTest::testEscapedSyms()
{
	testObj.prepareToTest("cmd2",QByteArrayList()<<"ar\\g1"<<"a|rg2"<<"ar\ng3");
	QByteArray data("cmd2|ar\\\\g1|a\\|rg2|ar\\ng3\n");
	testObj.parser.putData(data.constData(),data.size());
	VERIFY(testObj.wasMsg)
	VERIFY(testObj.testResult)
}

void ARpcStreamParserTest::testHexSeq()
{
	testObj.prepareToTest("cmd",QByteArrayList()<<"arg1"<<"arg/");
	QByteArray data("cmd|arg1|arg\\x2f\n");
	testObj.parser.putData(data.constData(),data.size());
	VERIFY(testObj.wasMsg)
	VERIFY(testObj.testResult)
}

void ARpcStreamParserTest::testReset()
{
	testObj.prepareToTest("cmd",QByteArrayList()<<"arg1");
	QByteArray data(QByteArray("cmddddd|aaaarrrrgggg")+QByteArray::fromHex("0x00")+"cmd|arg1\n");
	testObj.parser.putData(data.constData(),data.size());
	VERIFY(testObj.wasMsg)
	VERIFY(testObj.testResult)
}

void ARpcStreamParserTest::testParserBufferOverflow()
{
	testObj.prepareToTest("cmd",QByteArrayList()<<"arg1");
	QByteArray data;
	for(int i=0;i<100;++i)
		data.append("a");
	testObj.parser.putData(data.constData(),data.size());
	data="cmd|arg1\n";
	testObj.parser.putData(data.constData(),data.size());
	VERIFY(testObj.wasMsg)
	VERIFY(testObj.testResult)
}
