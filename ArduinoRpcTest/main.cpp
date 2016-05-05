#include <QApplication>
#include "ARpcStreamParserTests.h"
#include "ARpcMessageParserTests.h"
#include "ARpcTtyWatcherTests.h"
#include "ARpcComplexTests.h"
#include "QtUnitTestCollection.h"
#include "QtUnitMain.h"
#include <stdio.h>

class TestsCollection
	:public QtUnit::QtUnitTestCollection
{
public:
	explicit TestsCollection(QObject *parent=0)
		:QtUnitTestCollection("TestsCollection",parent)
	{
		new ARpcMessageParserTests(this);
		new ARpcStreamParserTests(this);
		new ARpcTtyWatcherTests(this);//disabled temporary
		new ARpcComplexTests(this);
	}
};

int main(int argc,char **argv)
{
	QApplication app(argc,argv);
//	{
//		ARpcConfig cfg;
//		ARpcTtyDevice dev("/dev/ttyACM0",cfg);
//		getwchar();
//	}
	TestsCollection c;
	return QtUnit::QtUnitMain(argc,argv,&c,"ArduinoRpc");
}
