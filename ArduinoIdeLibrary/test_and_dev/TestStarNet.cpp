#include "TestStarNet.h"
#include "ARpcStarNetDevice.h"
#include <QDebug>

ARpcUuid device1Id("17d90059d4904dad96f411cdb47d81e8");
ARpcUuid device2Id("2905e137ebf0425a9486858ee897bad8");
ARpcUuid device3Id("3502d8b470f4485fb2edcd7781afaf47");

void write1To2(void *data,const char *str,unsigned long size);
void write2To1(void *data,const char *str,unsigned long size);
void write2To3(void *data,const char *str,unsigned long size);
void write3To2(void *data,const char *str,unsigned long size);
void writeToNothing(void *data,const char *str,unsigned long size)
{
	Q_UNUSED(data)
	Q_UNUSED(str)
	Q_UNUSED(size)
}

/*
 * net1(1) <-> net2(1)
 * net2(2) <-> net3(1)
*/

ARpcStarNet net1(300,&write1To2,0,&writeToNothing,0,device1Id,"test1");
ARpcStarNet net2(300,&write2To1,0,&write2To3,0,device2Id,"test2");
ARpcStarNet net3(300,&write3To2,0,&writeToNothing,0,device3Id,"test3");

void write1To2(void *data,const char *str,unsigned long size)
{
	Q_UNUSED(data)
	static QByteArray b1,b2;
	for(unsigned long i=0;i<size;++i)
	{
		b1.append(str[i]);
		if(str[i]=='\n')
		{
			b2=b1;
			b1.clear();
			qDebug()<<"Data 1 to 2: "<<b2;
			net2.putData1(b2.constData(),b2.size());
		}
	}
}

void write2To1(void *data,const char *str,unsigned long size)
{
	Q_UNUSED(data)
	static QByteArray b1,b2;
	for(unsigned long i=0;i<size;++i)
	{
		b1.append(str[i]);
		if(str[i]=='\n')
		{
			b2=b1;
			b1.clear();
			qDebug()<<"Data 2 to 1: "<<b2;
			net1.putData1(b2.constData(),b2.size());
		}
	}
}

void write2To3(void *data,const char *str,unsigned long size)
{
	Q_UNUSED(data)
	static QByteArray b1,b2;
	for(unsigned long i=0;i<size;++i)
	{
		b1.append(str[i]);
		if(str[i]=='\n')
		{
			b2=b1;
			b1.clear();
			qDebug()<<"Data 2 to 3: "<<b2;
			net3.putData1(b2.constData(),b2.size());
		}
	}
}

void write3To2(void *data,const char *str,unsigned long size)
{
	Q_UNUSED(data)
	static QByteArray b1,b2;
	for(unsigned long i=0;i<size;++i)
	{
		b1.append(str[i]);
		if(str[i]=='\n')
		{
			b2=b1;
			b1.clear();
			qDebug()<<"Data 3 to 2: "<<b2;
			net2.putData2(b2.constData(),b2.size());
		}
	}
}

const char *lastCmd=0;
const char **lastArgs=0;
unsigned char lastArgsCount=0;
bool wasCmd2=false;

void nullLastInfo()
{
	lastCmd=0;
	lastArgs=0;
	lastArgsCount=0;
	wasCmd2=false;
}

void net2CmdCb(void *data,const char *cmd,const char **args,unsigned char argsCount)
{
	Q_UNUSED(data)
	wasCmd2=true;
	lastCmd=cmd;
	lastArgs=args;
	lastArgsCount=argsCount;
	net2.writeOk();
}

void net3CmdCb(void *data,const char *cmd,const char **args,unsigned char argsCount)
{
	Q_UNUSED(data)
	wasCmd2=true;
	lastCmd=cmd;
	lastArgs=args;
	lastArgsCount=argsCount;
	net3.writeOk();
}

bool testStarNet()
{
	net2.installCommandHandler(&net2CmdCb,0);
	net3.installCommandHandler(&net3CmdCb,0);
	qDebug()<<"TEST STAR NET";

	qDebug()<<"TEST0: direct 1->2";
	nullLastInfo();
	net1.setDestDeviceId(device2Id);
	net1.writeMsg("call","1","2","3");
	if(!wasCmd2||strcmp(lastCmd,"1")!=0||lastArgsCount!=2||strcmp(lastArgs[0],"2")!=0||strcmp(lastArgs[1],"3")!=0)
	{
		qDebug()<<"FAILED";
		return false;
	}
	qDebug()<<"OK";

	qDebug()<<"TEST1: undirect 1->3";
	nullLastInfo();
	net1.setDestDeviceId(device3Id);
	net1.writeMsg("call","11","22","33");
	if(!wasCmd2||strcmp(lastCmd,"11")!=0||lastArgsCount!=2||strcmp(lastArgs[0],"22")!=0||strcmp(lastArgs[1],"33")!=0)
	{
		qDebug()<<"FAILED";
		return false;
	}
	qDebug()<<"OK";

	return true;
}
