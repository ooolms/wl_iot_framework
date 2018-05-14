#include "TestStarNetDevice.h"
#include "ARpcStarNetDevice.h"
#include <QDebug>

ARpcUuid device1Id("17d90059d4904dad96f411cdb47d81e8");
ARpcUuid device2Id("2905e137ebf0425a9486858ee897bad8");
ARpcUuid device3Id("3502d8b470f4485fb2edcd7781afaf47");

class TestWriter
	:public ARpcIWriteCallback
{
public:
	TestWriter(const QByteArray &fromName,const QByteArray &toName,int devInput)
	{
		mFromName=fromName;
		mToName=toName;
		input=devInput;
	}

	void setDevice(ARpcStarNetDevice *dev)
	{
		mDevice=dev;
	}

	virtual void writeData(const char *data,unsigned long size)override
	{
		for(unsigned long i=0;i<size;++i)
		{
			b1.append(data[i]);
			if(data[i]=='\n')
			{
				b2=b1;
				b1.clear();
				qDebug()<<"Data "<<mFromName<<" to "<<mToName<<": "<<b2;
				if(input==0)
					mDevice->putData1(b2.constData(),b2.size());
				else mDevice->putData2(b2.constData(),b2.size());
			}
		}
	}

	virtual void writeStr(const char *str)override
	{
		unsigned long size=strlen(str);
		for(unsigned long i=0;i<size;++i)
		{
			b1.append(str[i]);
			if(str[i]=='\n')
			{
				b2=b1;
				b1.clear();
				qDebug()<<"Data "<<mFromName<<" to "<<mToName<<": "<<b2;
				if(input==0)
					mDevice->putData1(b2.constData(),b2.size());
				else mDevice->putData2(b2.constData(),b2.size());
			}
		}

	}

private:
	QByteArray mFromName,mToName;
	ARpcStarNetDevice *mDevice;
	QByteArray b1,b2;
	int input;
};

class TestNullWriter
	:public ARpcIWriteCallback
{
public:
	virtual void writeData(const char *data,unsigned long size)override
	{
		Q_UNUSED(data)
		Q_UNUSED(size)
	}

	virtual void writeStr(const char *str)override
	{
		Q_UNUSED(str)
	}
};

/*
 * net1(1) <-> net2(1)
 * net2(2) <-> net3(1)
*/

TestNullWriter writeToNothing;
TestWriter write1To2("1","2",0);
TestWriter write2To1("2","1",0);
TestWriter write2To3("2","3",0);
TestWriter write3To2("3","2",1);

ARpcStarNetDevice net1(300,&write1To2,&writeToNothing,&device1Id,"test1");
ARpcStarNetDevice net2(300,&write2To1,&write2To3,&device2Id,"test2");
ARpcStarNetDevice net3(300,&write3To2,&writeToNothing,&device3Id,"test3");

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
		dev->writeOk();
	}

private:
	ARpcStarNetDevice *dev;
};

bool testStarNet()
{
	write1To2.setDevice(&net2);
	write2To1.setDevice(&net1);
	write2To3.setDevice(&net3);
	write3To2.setDevice(&net2);

	TestCommandCallback net2Cb(&net2);
	TestCommandCallback net3Cb(&net3);
	net2.installCommandHandler(&net2Cb);
	net3.installCommandHandler(&net3Cb);
	qDebug()<<"TEST STAR NET";

	qDebug()<<"TEST 0: direct 1->2";
	nullLastInfo();
	net1.setDestDeviceId(device2Id);
	net1.writeMsg("call","1","2","3");
	if(!wasCmd||strcmp(lastCmd,"1")!=0||lastArgsCount!=2||strcmp(lastArgs[0],"2")!=0||strcmp(lastArgs[1],"3")!=0)
	{
		qDebug()<<"FAILED";
		return false;
	}
	qDebug()<<"OK";

	net1.setDestDeviceId(device2Id);
	net1.writeMsg("identify");

	qDebug()<<"TEST 1: undirect 1->3";
	nullLastInfo();
	net1.setDestDeviceId(device3Id);
	net1.writeMsg("call","11","22","33");
	if(!wasCmd||strcmp(lastCmd,"11")!=0||lastArgsCount!=2||strcmp(lastArgs[0],"22")!=0||strcmp(lastArgs[1],"33")!=0)
	{
		qDebug()<<"FAILED";
		return false;
	}
	qDebug()<<"OK";

	return true;
}
