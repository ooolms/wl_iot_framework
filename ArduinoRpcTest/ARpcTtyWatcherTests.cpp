#include "ARpcTtyWatcherTests.h"
#include "ARpcTtyDevice.h"
#include <QMessageBox>

ARpcTtyWatcherTests::ARpcTtyWatcherTests(QObject *parent)
	:QtUnitTestSet("ARpcTtyWatcherTests",parent)
{
	addTest((TestFunction)&ARpcTtyWatcherTests::testConnectionOnTheFly,"Test on-the-fly connection");
	addTest((TestFunction)&ARpcTtyWatcherTests::testStartupConnection,"Test startup connection");
}

void ARpcTtyWatcherTests::testConnectionOnTheFly()
{
	QMessageBox::warning(0,"!","Ensure arduino disconnected on /dev/ttyACM0");

	ARpcTtyDevice w("/dev/ttyACM0",ARpcConfig());
	QMessageBox m1(QMessageBox::Warning,"!","Insert arduino",QMessageBox::Ok);
	QMetaObject::Connection conn=connect(&w,SIGNAL(ttyConnected()),&m1,SLOT(accept()));
	m1.exec();
	VERIFY(w.isConnected())
	disconnect(conn);

	QMessageBox m2(QMessageBox::Warning,"!","Remove arduino",QMessageBox::Ok);
	conn=connect(&w,SIGNAL(ttyDisconnected()),&m2,SLOT(accept()));
	m2.exec();
	VERIFY(!w.isConnected())
	disconnect(conn);

	conn=connect(&w,SIGNAL(ttyConnected()),&m1,SLOT(accept()));
	m1.exec();
	VERIFY(w.isConnected())
	disconnect(conn);
}

void ARpcTtyWatcherTests::testStartupConnection()
{
	QMessageBox::warning(0,"!","Ensure arduino connected on /dev/ttyACM0");

	ARpcTtyDevice w("/dev/ttyACM0",ARpcConfig());
	VERIFY(w.isConnected())
}
