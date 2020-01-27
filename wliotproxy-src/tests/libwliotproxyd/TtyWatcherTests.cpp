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

#include "TtyWatcherTests.h"
#include "wliot/devices/SerialDeviceBackend.h"
#include "wliot/devices/StdHighLevelDeviceBackend.h"
#include "wliot/devices/RealDevice.h"
#include "wliot/devices/CommandCall.h"
#include <QMessageBox>
#include <QTimer>
#include <QApplication>

TtyWatcherTests::TtyWatcherTests(QObject *parent)
	:QtUnitTestSet("TtyWatcherTests",parent)
{
	addTest((TestFunction)&TtyWatcherTests::testStartupConnection,"Test startup connection");
	addTest((TestFunction)&TtyWatcherTests::testCallBreakWhenDevDisconnected,
		"Test that calls are broken when device is disconnected");
}

void TtyWatcherTests::testStartupConnection()
{
	QMessageBox::warning(0,"!","Ensure arduino connected on /dev/ttyACM0");

	SerialDeviceBackend *be=new SerialDeviceBackend("/dev/ttyACM0");
	RealDevice w;
	w.setBackend(new StdHighLevelDeviceBackend(be));
	be->tryOpen();
	VERIFY(w.isConnected());
}

void TtyWatcherTests::testCallBreakWhenDevDisconnected()
{
	QMessageBox::warning(nullptr,"!","Ensure arduino connected on /dev/ttyACM0");
	SerialDeviceBackend *be=new SerialDeviceBackend("/dev/ttyACM0");
	RealDevice w;
	w.setBackend(new StdHighLevelDeviceBackend(be));
	be->tryOpen();
	VERIFY(w.isConnected());
	QSharedPointer<CommandCall> call=w.execCommand("testNoAnswer");
	QMessageBox m1(QMessageBox::Warning,"!","Disconnect arduino and reconnect again",QMessageBox::Ok);
	QTimer timer;
	timer.setInterval(10000);
	timer.setSingleShot(true);
	bool timeoutAborted=false;
	connect(&timer,&QTimer::timeout,[&call,&m1,&timeoutAborted]{
		call->abort();
		m1.close();
		timeoutAborted=true;
	});

	m1.show();
	timer.start();
	VERIFY(!call->wait());
	VERIFY_MESSAGE(!timeoutAborted,"timer aborted");
	timer.stop();
	m1.close();
}
