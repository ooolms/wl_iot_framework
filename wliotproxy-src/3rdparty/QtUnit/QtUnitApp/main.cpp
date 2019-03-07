#include <QApplication>
#include <QDebug>
#include "testqtunittest.h"
#include "test2qtunittest.h"
#include "QtUnitTestCollection.h"
#include "QtUnitMain.h"
#include "QtUnitWidget.h"

class TestMain
	:public QtUnit::QtUnitMain
{
public:
	TestMain(int argc,char **argv):QtUnitMain(argc,argv){}

protected:
	virtual QtUnit::QtUnitTestCollection* makeCollection()
	{
		QtUnit::QtUnitTestCollection *collection=new QtUnit::QtUnitTestCollection("test collection");
		new TestQtUnitTest(collection);
		new Test2QtUnitTest(collection);
		return collection;
	}
	virtual QString project(){return "QtUnitTestApp";}
};

int main(int argc,char **argv)
{
	TestMain m(argc,argv);
	return m.main();
}
