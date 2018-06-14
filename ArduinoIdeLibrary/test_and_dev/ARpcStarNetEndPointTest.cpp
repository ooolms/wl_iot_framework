#include "ARpcStarNetEndPointTest.h"
#include "ARpcStarNetEndPoint.h"
#include "ARpcStarNetDevice.h"
#include "TestNetWriters.h"

class TestMessageCallback
	:public ARpcINetMsgCallback
{
public:
	TestMessageCallback()
	{
		nullLastInfo();
	}

	void nullLastInfo()
	{
		lastMsg.clear();
		lastSrcId=ARpcUuid();
		lastArgs.clear();
		wasMsg=false;
	}

	virtual void processMsg(const ARpcUuid &srcId,const char *msg,
		const char *args[],unsigned char argsCount)override
	{
		wasMsg=true;
		lastSrcId=srcId;
		lastMsg=msg;
		lastArgs.clear();
		for(int i=0;i<argsCount;++i)
			lastArgs.append(QByteArray(args[i]));
	}

public:
	QByteArray lastMsg;
	QByteArrayList lastArgs;
	bool wasMsg;
	ARpcUuid lastSrcId;
};

class TestMessageCallbackEcho
	:public ARpcINetMsgCallback
{
public:	
	void setDevice(ARpcStarNetEndPoint *d)
	{
		dev=d;
	}

	virtual void processMsg(const ARpcUuid &srcId,const char *msg,
		const char *args[],unsigned char argsCount)override
	{
		dev->writeMsg(srcId,msg,args,argsCount);
	}

private:
	ARpcStarNetEndPoint *dev;
};

/*
 * ep1 <-> (1)net2(2) <-> ep3
*/

static ARpcUuid device1Id("17d90059d4904dad96f411cdb47d81e8");
static ARpcUuid device2Id("2905e137ebf0425a9486858ee897bad8");
static ARpcUuid device3Id("3502d8b470f4485fb2edcd7781afaf47");
static TestNullWriter writeToNothing;
static TestWriterStarNetDevice write1To2("1","2",0);
static TestWriterStarNetEndPoint write2To1("2","1");
static TestWriterStarNetEndPoint write2To3("2","3");
static TestWriterStarNetDevice write3To2("3","2",1);
static TestMessageCallback cb1;
static TestMessageCallbackEcho cb3;
static ARpcStarNetEndPoint ep1(300,&write1To2,&cb1,&device1Id);
static ARpcStarNetDevice net2(300,&write2To1,&write2To3,&device2Id,"test2");
static ARpcStarNetEndPoint ep3(300,&write3To2,&cb3,&device3Id);

ARpcStarNetEndPointTest::ARpcStarNetEndPointTest(QObject *parent)
	:QtUnitTestSet("ARpcStarNetEndPointTest",parent)
{
	write1To2.setDevice(&net2);
	write2To1.setDevice(&ep1);
	write2To3.setDevice(&ep3);
	write3To2.setDevice(&net2);
	cb3.setDevice(&ep3);
	addTest((TestFunction)&ARpcStarNetEndPointTest::testEscapingInNet,"Test escaping in the network");
}

void ARpcStarNetEndPointTest::testEscapingInNet()
{
	const char *msg="msg";
	const char *arg1="aa\\\\bb";
	const char *arg2="\x01\x02\x03";
	ep1.writeMsg(device3Id,msg,arg1,arg2);
	VERIFY(cb1.wasMsg);
	COMPARE(cb1.lastSrcId,device3Id);
	COMPARE(cb1.lastArgs.count(),2);
	COMPARE(cb1.lastArgs[0],QByteArray(arg1));
	COMPARE(cb1.lastArgs[1],QByteArray(arg2));
}
