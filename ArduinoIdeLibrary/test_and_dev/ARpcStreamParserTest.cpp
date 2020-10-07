#include "ARpcStreamParserTest.h"
#include "ARpcStreamWriter.h"

class ByteArrayWriteCallback
	:public ARpcIWriteCallback
{
public:
	ByteArrayWriteCallback(QByteArray *b)
	{
		arr=b;
	}
	virtual void writeData(const char *data,unsigned long sz) override
	{
		arr->append(data,sz);
	}
	virtual void writeStr(const char *str) override
	{
		arr->append(str);
	}
	virtual void writeStr(const __FlashStringHelper *str) override
	{
		arr->append((const char *)str);
	}

public:
	QByteArray *arr;
};

ARpcStreamParserTest::ARpcStreamParserTest(QObject *parent)
	:QtUnitTestSet("ARpcStreamParserTest",parent)
	,testObj(100)
{
	addTest((TestFunction)&ARpcStreamParserTest::testSimple,"Simple parsing test");
	addTest((TestFunction)&ARpcStreamParserTest::testEscapedSyms,"Test parsing escaped sequences");
	addTest((TestFunction)&ARpcStreamParserTest::testSimple,"Test parsing hex encoded chars");
	addTest((TestFunction)&ARpcStreamParserTest::testReset,"Test reset parser on 0-byte");
	addTest((TestFunction)&ARpcStreamParserTest::testParserBufferOverflow,"Test reset parser on buffer overflow");
	addTest((TestFunction)&ARpcStreamParserTest::test4Floats,"test4Floats");
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

void ARpcStreamParserTest::test4Floats()
{
	class MsgCb
		:public ARpcIMessageCallback
	{
	public:
		MsgCb()
		{
			ok=false;
		}
		virtual void processMsg(const char *msg,const char **args,unsigned char argsCount)override
		{
			if(QByteArray(msg)=="meas"&&argsCount==3)
			{
				f1=*((const float*)args[0]);
				f2=*((const float*)args[1]);
				f3=*((const float*)args[2]);
				ok=true;
			}
		}

	public:
		bool ok;
		float f1,f2,f3;
	};

	QByteArray a;
	ByteArrayWriteCallback cb(&a);
	ARpcStreamWriter writer(&cb);
	float x,y=10,z=-13.2;
	writer.beginWriteMsg();
	writer.writeArg("meas",4);
	writer.writeArg((const char*)&x,4);
	writer.writeArg((const char*)&y,4);
	writer.writeArg((const char*)&z,4);
	writer.endWriteMsg();
	MsgCb cb2;
	ARpcStreamParser p(200,&cb2);
	p.putData(a.constData(),a.size());
	VERIFY(cb2.ok)
	COMPARE(cb2.f1,x)
	COMPARE(cb2.f2,y)
	COMPARE(cb2.f3,z)
}
