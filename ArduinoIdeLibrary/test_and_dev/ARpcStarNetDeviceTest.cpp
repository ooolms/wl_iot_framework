#include "ARpcStarNetDeviceTest.h"
#include "ARpcStarNetDevice.h"
#include "TestNetWriters.h"
#include <QDebug>

/*
 * net1(1) <-> (1)net2(2) <-> net3(1)
*/

static ARpcUuid device1Id("17d90059d4904dad96f411cdb47d81e8");
static ARpcUuid device2Id("2905e137ebf0425a9486858ee897bad8");
static ARpcUuid device3Id("3502d8b470f4485fb2edcd7781afaf47");
static TestNullWriter writeToNothing;
static TestWriterStarNetDevice write1To2("1","2",0);
static TestWriterStarNetDevice write2To1("2","1",0);
static TestWriterStarNetDevice write2To3("2","3",0);
static TestWriterStarNetDevice write3To2("3","2",1);

static ARpcStarNetDevice net1(300,&write1To2,&writeToNothing,&device1Id,"test1");
static ARpcStarNetDevice net2(300,&write2To1,&write2To3,&device2Id,"test2");
static ARpcStarNetDevice net3(300,&write3To2,&writeToNothing,&device3Id,"test3");

class TestCommandCallback
	:public ARpcIMessageCallback
{
public:
	explicit TestCommandCallback(ARpcStarNetDevice *d)
	{
		dev=d;
		nullLastInfo();
	}

	void nullLastInfo()
	{
		lastCmd=0;
		lastArgs=0;
		lastArgsCount=0;
		wasCmd=false;
	}

	virtual void processMsg(const char *cmd,const char *args[],unsigned char argsCount)override
	{
		wasCmd=true;
		lastCmd=cmd;
		lastArgs=args;
		lastArgsCount=argsCount;
		dev->disp().writeOk();
	}

public:
	const char *lastCmd;
	const char **lastArgs;
	unsigned char lastArgsCount;
	bool wasCmd;

private:
	ARpcStarNetDevice *dev;
};

static TestCommandCallback net2Cb(&net2);
static TestCommandCallback net3Cb(&net3);

ARpcStarNetDeviceTest::ARpcStarNetDeviceTest(QObject *parent)
	:QtUnitTestSet("ARpcStarNetDeviceTest",parent)
{
	write1To2.setDevice(&net2);
	write2To1.setDevice(&net1);
	write2To3.setDevice(&net3);
	write3To2.setDevice(&net2);
	net2.disp().installCommandHandler(&net2Cb);
	net3.disp().installCommandHandler(&net3Cb);
	addTest((TestFunction)&ARpcStarNetDeviceTest::testDirectMsg,"Test direct message 1->2");
	addTest((TestFunction)&ARpcStarNetDeviceTest::testUndirectMsg,"Test direct message 1->(2)->3");
}

void ARpcStarNetDeviceTest::testDirectMsg()
{
	net2Cb.nullLastInfo();
	net1.setDestDeviceId(device2Id);
	net1.disp().writeMsg("call","id1","1","2","3");
	VERIFY(net2Cb.wasCmd)
	if(strcmp(net2Cb.lastCmd,"1")!=0||net2Cb.lastArgsCount!=2||
		strcmp(net2Cb.lastArgs[0],"2")!=0||strcmp(net2Cb.lastArgs[1],"3")!=0)
		FAIL("bad message")
}

void ARpcStarNetDeviceTest::testUndirectMsg()
{
	net3Cb.nullLastInfo();
	net1.setDestDeviceId(device3Id);
	net1.disp().writeMsg("call","id2","11","22","33");
	VERIFY(net3Cb.wasCmd)
	if(strcmp(net3Cb.lastCmd,"11")!=0||net3Cb.lastArgsCount!=2||
		strcmp(net3Cb.lastArgs[0],"22")!=0||strcmp(net3Cb.lastArgs[1],"33")!=0)
		FAIL("bad message")
}
