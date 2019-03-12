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

#include "FakeDevice.h"
#include "wliot/WLIOTProtocolDefs.h"
#include <QDebug>
#include <QThread>

FakeDevice::FakeDevice(QObject *parent)
	:RealDevice(parent)
{
	devId=QUuid::createUuid();
	onConnected();
}

bool FakeDevice::writeMsgToDevice(const Message &m)
{
	if(m.title==WLIOTProtocolDefs::identifyMsg)
	{
		emit logMsg("Identify request");
		QThread::usleep(100);
		emit newMessageFromDevice(
			Message(WLIOTProtocolDefs::deviceInfoMsg,QByteArrayList()<<devId.toByteArray()<<"Test device"));
	}
	else if(m.title==WLIOTProtocolDefs::funcCallMsg&&!m.args.isEmpty())
	{
		QByteArrayList args=m.args;
		args.removeAt(0);
		emit logMsg("Function called: "+m.args[0]+"; args: "+args.join("|"));
		QThread::usleep(100);
		emit newMessageFromDevice(Message("ok"));
	}
	return true;
}

QByteArray FakeDevice::deviceType()
{
	return "fake";
}
