#include "SimpleMsgDispatchTests.h"
#include "wliot/devices/CommandCall.h"
#include "wliot/devices/SimpleMsgDispatch.h"
#include <QDebug>

class CommandCallTestsDevCmdCallback
	:public IFakeDeviceCallback
{
public:
	void sleep(int sec)
	{
		QThread::sleep(sec);
	}

	virtual bool processCommand(const QByteArray &callId,const QByteArray &cmd,
		const QByteArrayList &args,QByteArrayList &retVal)override
	{
		if(cmd=="testInfoMsg")
		{
			emit newMessageFromDevice(Message(WLIOTProtocolDefs::infoMsg,QByteArrayList()<<"info_msg"));
			return true;
		}
		else if(cmd=="testMeasMsg")
		{
			emit newMessageFromDevice(Message(WLIOTProtocolDefs::measurementMsg,QByteArrayList()<<"sens1"<<"val1"));
			return true;
		}
		else if(cmd=="testChangeCommandState")
		{
			emit newMessageFromDevice(Message(
				WLIOTProtocolDefs::stateChangedMsg,QByteArrayList()<<"cmd1"<<"1"<<"val1"));
			return true;
		}
		else if(cmd=="testChangeAdditionalState")
		{
			emit newMessageFromDevice(Message(
				WLIOTProtocolDefs::stateChangedMsg,QByteArrayList()<<"#"<<"st1"<<"val1"));
			return true;
		}
		retVal.append("unknown command");
		return false;
	}
};

SimpleMsgDispatchTests::SimpleMsgDispatchTests(QObject *parent)
	:QtUnitTestSet("SimpleMsgDispatchTests",parent)
{
	addTest((TestFunction)&SimpleMsgDispatchTests::testInfoMsgDispatch,"Test info message");
	addTest((TestFunction)&SimpleMsgDispatchTests::testMeasMsgDispatch,"Test meas message");
	addTest((TestFunction)&SimpleMsgDispatchTests::testCommandStateChangeMsgDispatch,
		"Test command state changed message");
	addTest((TestFunction)&SimpleMsgDispatchTests::testAdditionalStateChangeMsgDispatch,
		"Test additiona; state changed message");
}


bool SimpleMsgDispatchTests::init()
{
	device=new FakeDevice(new CommandCallTestsDevCmdCallback());
	device->identify();
	disp=new SimpleMsgDispatch(device);
	connect(device,&FakeDevice::newMessageFromDevice,disp,&SimpleMsgDispatch::onNewMessageFromDevice);
	return device->isConnected();
}

void SimpleMsgDispatchTests::cleanup()
{
	delete device;
}

bool SimpleMsgDispatchTests::testInit()
{
	device->setConnected(true);
	return true;
}

void SimpleMsgDispatchTests::testInfoMsgDispatch()
{
	QByteArray infoMsg;
	connect(disp,&SimpleMsgDispatch::infoMsg,[&infoMsg](const QByteArray &str)
	{
		qDebug()<<"onInfoMsg";
		infoMsg=str;
	});
	CommandCall *call=device->execCommand("testInfoMsg");
	VERIFY(call->wait());
	COMPARE(infoMsg,QByteArray("info_msg"));
}

void SimpleMsgDispatchTests::testMeasMsgDispatch()
{
	QByteArray measSens,measVal;
	connect(disp,&SimpleMsgDispatch::measurementMsg,
		[&measSens,&measVal](const QByteArray &sensor,const QByteArray &value)
		{
			qDebug()<<"onMeasMsg";
			measSens=sensor;
			measVal=value;
		}
	);
	CommandCall *call=device->execCommand("testMeasMsg");
	VERIFY(call->wait());
	COMPARE(measSens,QByteArray("sens1"));
	COMPARE(measVal,QByteArray("val1"));
}

void SimpleMsgDispatchTests::testCommandStateChangeMsgDispatch()
{
	QByteArray cmd,val;
	int argIndex;
	connect(disp,&SimpleMsgDispatch::commandStateChanged,[&cmd,&argIndex,&val](
		const QByteArray &command,int index,const QByteArray &value)
	{
		qDebug()<<"onCommandStateChangedMsg";
		cmd=command;
		argIndex=index;
		val=value;
	});
	CommandCall *call=device->execCommand("testChangeCommandState");
	VERIFY(call->wait());
	COMPARE(cmd,QByteArray("cmd1"));
	COMPARE(argIndex,1);
	COMPARE(val,QByteArray("val1"));
}

void SimpleMsgDispatchTests::testAdditionalStateChangeMsgDispatch()
{
	QByteArray param,val;
	connect(disp,&SimpleMsgDispatch::additionalStateChanged,[&param,&val](
		const QByteArray &p,const QByteArray &v)
	{
		qDebug()<<"onAdditionalStateChangedMsg";
		param=p;
		val=v;
	});
	CommandCall *call=device->execCommand("testChangeAdditionalState");
	VERIFY(call->wait());
	COMPARE(param,QByteArray("st1"));
	COMPARE(val,QByteArray("val1"));
}
