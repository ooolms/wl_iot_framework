/*#include "TestStarNetDevice.h"
#include "ARpcStarNetDevice.h"
#include <QDebug>

ARpcUuid device1Id("17d90059d4904dad96f411cdb47d81e8");
ARpcUuid device2Id("2905e137ebf0425a9486858ee897bad8");
ARpcUuid device3Id("3502d8b470f4485fb2edcd7781afaf47");

const char *lastCmd=0;
const char **lastArgs=0;
unsigned char lastArgsCount=0;
bool wasCmd=false;

void nullLastInfo()
{
	lastCmd=0;
	lastArgs=0;
	lastArgsCount=0;
	wasCmd=false;
}

class TestCommandCallback
	:public ARpcIMessageCallback
{
public:
	explicit TestCommandCallback(ARpcStarNetDevice *d)
	{
		dev=d;
	}

	virtual void processMessage(const char *cmd,const char *args[],unsigned char argsCount)override
	{
		wasCmd=true;
		lastCmd=cmd;
		lastArgs=args;
		lastArgsCount=argsCount;
		dev->disp().writeOk();
	}

private:
	ARpcStarNetDevice *dev;
};

bool testStarNet()
{

	TestCommandCallback net2Cb(&net2);
	TestCommandCallback net3Cb(&net3);
	net2.disp().installCommandHandler(&net2Cb);
	net3.disp().installCommandHandler(&net3Cb);
	qDebug()<<"TEST STAR NET";

	qDebug()<<"TEST 0: direct 1->2";
	nullLastInfo();
	net1.setDestDeviceId(device2Id);
	net1.disp().writeMsg("call","1","2","3");
	if(!wasCmd||strcmp(lastCmd,"1")!=0||lastArgsCount!=2||strcmp(lastArgs[0],"2")!=0||strcmp(lastArgs[1],"3")!=0)
	{
		qDebug()<<"FAILED";
		return false;
	}
	qDebug()<<"OK";

	net1.setDestDeviceId(device2Id);
	net1.disp().writeMsg("identify");

	qDebug()<<"TEST 1: undirect 1->3";
	nullLastInfo();
	net1.setDestDeviceId(device3Id);
	net1.disp().writeMsg("call","11","22","33");
	if(!wasCmd||strcmp(lastCmd,"11")!=0||lastArgsCount!=2||strcmp(lastArgs[0],"22")!=0||strcmp(lastArgs[1],"33")!=0)
	{
		qDebug()<<"FAILED";
		return false;
	}
	qDebug()<<"OK";

	return true;
}
*/
