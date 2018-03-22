#include "TestDeviceState.h"
#include <QByteArrayList>
#include <QList>
#include <QDebug>
#include "ARpcDevice.h"

static const QByteArrayList testMsgs=QByteArrayList()<<
	"call|#state\n"<<
	"call|test1|123\n"<<
	"call|test2|111|222|333\n"<<
	"call|test_add_param1|666\n";
static const QByteArrayList testOutputs=QByteArrayList()<<
	"ok\n"<<
	"statechanged|test1|0|123\nok\n"<<
	"statechanged|test2|0|111\nstatechanged|test2|1|222\nstatechanged|test2|2|333\nok\n"<<
	"statechanged|#|zzz|666\nok\n";

void testStateWriteCb(void *d,const char *str,unsigned long sz);
void testStateCommandCb(void *d,const char *cmd,const char *args[],unsigned char argsCount);

const char *devId="{89773865-9675-48fd-996f-0bf1dfad39bc}";
const char *devName="test";
ARpcDevice parser(3000,&testStateWriteCb,0,devId,devName);

static QByteArray buffer;
void testStateWriteCb(void *d,const char *str,unsigned long sz)
{
	Q_UNUSED(d)
	buffer.append(QByteArray(str,sz));
}

void testStateCommandCb(void *d,const char *cmd,const char *args[],unsigned char argsCount)
{
	Q_UNUSED(d)
	if(cmd==QByteArray("test1"))
	{
		if(argsCount<1)
		{
			parser.writeErr("invalid parameters");
			return;
		}
		parser.state()->setCommandParamState(0,0,args[0]);
		parser.writeOk();
	}
	else if(cmd==QByteArray("test2"))
	{
		if(argsCount<3)
		{
			parser.writeErr("invalid parameters");
			return;
		}
		parser.state()->setCommandParamState(1,0,args[0]);
		parser.state()->setCommandParamState(1,1,args[1]);
		parser.state()->setCommandParamState(1,2,args[2]);
		parser.writeOk();
	}
	else if(cmd==QByteArray("test_add_param1"))
	{
		if(argsCount<1)
		{
			parser.writeErr("invalid parameters");
			return;
		}
		parser.state()->setAdditionalParamState(0,args[0]);
		parser.writeOk();
	}
	else parser.writeErr("unknown command");
}

bool testDeviceState()
{
	/*
	 * command 1 - "test1", 1 param - "a"
	 * command 2 - "test2", 3 params - "aaa", "bbb", "ccc"
	 * additional parameter 1 - "zzz"
	*/
	parser.installCommandHandler(&testStateCommandCb,0);
	parser.state()->prepareCommands(2);
	parser.state()->prepareCommand(0,"test1",1);
	parser.state()->prepareCommand(1,"test2",3);
	parser.state()->prepareAdditionalParameters(1);
	parser.state()->prepareAdditionalParameter(0,"zzz");

	bool ok=true;
	qDebug()<<"TEST DEVICE STATE";
	for(int i=0;i<testMsgs.count();++i)
	{
		qDebug()<<"TEST: "<<i;
		buffer.clear();
		parser.putData(testMsgs[i],testMsgs[i].size());
		qDebug()<<buffer;
		if(buffer!=testOutputs[i])
		{
			qDebug()<<"FAILED";
			ok=false;
		}
		else qDebug()<<"OK";
	}
	return ok;
}
