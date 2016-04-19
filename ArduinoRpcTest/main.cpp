#include <QApplication>
#include "ARpcStreamParserTests.h"
#include "ARpcMessageParserTests.h"
#include "ARpcTtyWatcherTests.h"
#include "ARpcComplexTests.h"
#include "qtunittestcollection.h"
#include "qtunitwidget.h"

class TestsCollection
	:public QtUnit::QtUnitTestCollection
{
public:
	explicit TestsCollection(QObject *parent=0)
		:QtUnitTestCollection("TestsCollection",parent)
	{
		new ARpcMessageParserTests(this);
		new ARpcStreamParserTests(this);
//		new ARpcTtyWatcherTests(this);//disabled temporary
		new ARpcComplexTests(this);
	}
};

int main(int argc,char **argv)
{
	QApplication app(argc,argv);
	QtUnit::QtUnitWidget w;
	w.setCollection(new TestsCollection(&w));
	w.show();
	return app.exec();
}
