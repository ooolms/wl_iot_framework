/*******************************************
Copyright 2017 OOO "LMS"

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

	http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.*/

#include "CommandCallTests.h"
#include "wliot/devices/CommandCall.h"
#include "FakeDevice.h"
#include <QThread>
#include <QDebug>
#include <QEventLoop>

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
		if(cmd=="testOk")return true;
		else if(cmd=="testErr")
		{
			retVal.append("epic fail");
			return false;
		}
		else if(cmd=="testLongCmdOk")
		{
			sleep(3);
			emit newMessageFromDevice(Message(WLIOTProtocolDefs::funcSynccMsg,QByteArrayList()<<callId));
			sleep(3);
			return true;
		}
		else if(cmd=="testLongCmdFail")
		{
			sleep(6);
			return true;
		}
		else if(cmd=="testInfoMsg")
		{
			emit newMessageFromDevice(Message(WLIOTProtocolDefs::infoMsg,QByteArrayList()<<"info_msg"));
			return true;
		}
		else if(cmd=="testDevReset")
		{
			emit devIsReset();
			return true;
		}
		else if(cmd=="testMeasMsg")
		{
			emit newMessageFromDevice(Message(WLIOTProtocolDefs::measurementMsg,QByteArrayList()<<"sens1"<<"val1"));
			return true;
		}
		retVal.append("unknown command");
		return false;
	}
};

CommandCallTests::CommandCallTests(QObject *parent)
	:QtUnitTestSet("CommandCallTests",parent)
{
	addTest((TestFunction)&CommandCallTests::testOk,"Test ok func");
	addTest((TestFunction)&CommandCallTests::testErr,"Test failing func");
	addTest((TestFunction)&CommandCallTests::testLongCommand,"Test regular sync");
	addTest((TestFunction)&CommandCallTests::testDevResetWhenCall,"Test func call fails if device is reset");
}

bool CommandCallTests::init()
{
	device=new FakeDevice(new CommandCallTestsDevCmdCallback());
	device->identify();
	return device->isConnected();
}

void CommandCallTests::cleanup()
{
	delete device;
}

bool CommandCallTests::testInit()
{
	device->setConnected(true);
	return true;
}

void CommandCallTests::testOk()
{
	CommandCall call(device,"testOk");
	VERIFY(call.call());
}

void CommandCallTests::testErr()
{
	CommandCall call(device,"testErr");
	VERIFY(!call.call());
	COMPARE(call.returnValue().count(),1);
	COMPARE(call.returnValue()[0],QByteArray("epic fail"));
}

void CommandCallTests::testLongCommand()
{
	CommandCall call(device,"testLongCmdOk");
	VERIFY(call.call());
	CommandCall call2(device,"testLongCmdFail");
	VERIFY(!call2.call());
}

void CommandCallTests::testDevResetWhenCall()
{
	CommandCall call(device,"testDevReset");
	VERIFY(!call.call())
}
