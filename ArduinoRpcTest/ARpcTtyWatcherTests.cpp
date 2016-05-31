#include "ARpcTtyWatcherTests.h"
#include "ARpcDevices/ARpcTtyDevice.h"
#include "ARpcSimpleAPI/ARpcSyncCall.h"
#include "ARpcSimpleAPI/ARpcSyncUnsafeCall.h"
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

	ARpcTtyDevice w("/dev/ttyACM0",cfg);
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

	ARpcTtyDevice w("/dev/ttyACM0",cfg);
	VERIFY(w.isConnected())
}

void ARpcTtyWatcherTests::testCallBreakWhenDevDisconnected()
{
	QMessageBox::warning(0,"!","Ensure arduino connected on /dev/ttyACM0");
	ARpcTtyDevice w("/dev/ttyACM0",cfg);
	VERIFY(w.isConnected())
	ARpcSyncCall call(cfg);
	ARpcSyncUnsafeCall call2(cfg);
	QStringList retVal;
	QMessageBox m1(QMessageBox::Warning,"!","Disconnect arduino and reconnect again",QMessageBox::Ok);
	QTimer timer;
	timer.setInterval(10000);
	timer.setSingleShot(true);
	bool timeoutAborted=false;
	connect(&timer,&QTimer::timeout,[&timer,&call,&call2,&m1,&timeoutAborted]{
		call.abort();
		call2.abort();
		m1.close();
		timeoutAborted=true;
	});

	m1.show();
	timer.start();
	VERIFY(!call.call(&w,ARpcMessage("testNoAnswer"),retVal));
	VERIFY_MESSAGE(!timeoutAborted,"timer aborted")
	timer.stop();
	m1.close();
	qApp->processEvents();
	QMessageBox::warning(0,"!","Ensure arduino connected on /dev/ttyACM0");
	m1.show();
	timer.start();
	VERIFY(!call2.call(&w,ARpcMessage("testNoAnswer"),retVal));
	VERIFY_MESSAGE(!timeoutAborted,"timer aborted")
	timer.stop();
	m1.close();
}
