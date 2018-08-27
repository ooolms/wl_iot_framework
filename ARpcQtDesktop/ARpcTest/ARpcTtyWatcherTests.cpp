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

#include "ARpcTtyWatcherTests.h"
#include "ARpcDevices/ARpcSerialDevice.h"
#include "ARpcBase/ARpcSyncCall.h"
#include <QMessageBox>
#include <QTimer>
#include <QApplication>
#include <unistd.h>

ARpcTtyWatcherTests::ARpcTtyWatcherTests(QObject *parent)
	:QtUnitTestSet("ARpcTtyWatcherTests",parent)
{
	addTest((TestFunction)&ARpcTtyWatcherTests::testConnectionOnTheFly,"Test on-the-fly connection");
	addTest((TestFunction)&ARpcTtyWatcherTests::testStartupConnection,"Test startup connection");
	addTest((TestFunction)&ARpcTtyWatcherTests::testCallBreakWhenDevDisconnected,
		"Test that calls are broken when device is disconnected");
}

void ARpcTtyWatcherTests::testConnectionOnTheFly()
{
	QMessageBox::warning(0,"!","Ensure arduino disconnected on /dev/ttyACM0");

	ARpcSerialDevice w("/dev/ttyACM0");
	QMessageBox m1(QMessageBox::Warning,"!","Insert arduino",QMessageBox::Ok);
	QMetaObject::Connection conn=connect(&w,SIGNAL(connected()),&m1,SLOT(accept()));
	m1.exec();
	VERIFY(w.isConnected())
	disconnect(conn);

	QMessageBox m2(QMessageBox::Warning,"!","Remove arduino",QMessageBox::Ok);
	conn=connect(&w,SIGNAL(disconnected()),&m2,SLOT(accept()));
	m2.exec();
	VERIFY(!w.isConnected())
	disconnect(conn);

	conn=connect(&w,SIGNAL(connected()),&m1,SLOT(accept()));
	m1.exec();
	VERIFY(w.isConnected())
	disconnect(conn);
}

void ARpcTtyWatcherTests::testStartupConnection()
{
	QMessageBox::warning(0,"!","Ensure arduino connected on /dev/ttyACM0");

	ARpcSerialDevice w("/dev/ttyACM0");
	VERIFY(w.isConnected());
}

void ARpcTtyWatcherTests::testCallBreakWhenDevDisconnected()
{
	QMessageBox::warning(0,"!","Ensure arduino connected on /dev/ttyACM0");
	ARpcSerialDevice w("/dev/ttyACM0");
	VERIFY(w.isConnected());
	ARpcSyncCall call(&w);
	QByteArrayList retVal;
	QMessageBox m1(QMessageBox::Warning,"!","Disconnect arduino and reconnect again",QMessageBox::Ok);
	QTimer timer;
	timer.setInterval(10000);
	timer.setSingleShot(true);
	bool timeoutAborted=false;
	connect(&timer,&QTimer::timeout,[&timer,&call,&m1,&timeoutAborted]{
		call.abort();
		m1.close();
		timeoutAborted=true;
	});

	m1.show();
	timer.start();
	VERIFY(!call.call("testNoAnswer",retVal));
	VERIFY_MESSAGE(!timeoutAborted,"timer aborted");
	timer.stop();
	m1.close();
}
