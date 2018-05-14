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
	"statechanged|test1|1|123\nok\n"<<
	"statechanged|test2|1|111\nstatechanged|test2|2|222\nstatechanged|test2|3|333\nok\n"<<
	"statechanged|#|zzz|666\nok\n";

class TestStateWriteCb
	:public ARpcIWriteCallback
{
public:
	virtual void writeData(const char *data,unsigned long sz)override
	{
		buffer.append(data,sz);
	}

	virtual void writeStr(const char *str)override
	{
		buffer.append(str);
	}

	QByteArray buffer;
}testStateWriteCb;

static const ARpcUuid devId("{89773865-9675-48fd-996f-0bf1dfad39bc}");
static const char *devName="test";
ARpcDevice parser(3000,&testStateWriteCb,&devId,devName);

class TestStateCommandCb
	:public ARpcIMessageCallback
{
public:
	virtual void processMessage(const char *cmd,const char *args[],unsigned char argsCount)override
	{
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
}testStateCommandCb;

bool testDeviceState()
{
	/*
	 * command 1 - "test1", 1 param - "a"
	 * command 2 - "test2", 3 params - "aaa", "bbb", "ccc"
	 * additional parameter 1 - "zzz"
	*/
	parser.installCommandHandler(&testStateCommandCb);
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
		testStateWriteCb.buffer.clear();
		parser.putData(testMsgs[i],testMsgs[i].size());
		qDebug()<<testStateWriteCb.buffer;
		if(testStateWriteCb.buffer!=testOutputs[i])
		{
			qDebug()<<"FAILED";
			ok=false;
		}
		else qDebug()<<"OK";
	}
	return ok;
}
