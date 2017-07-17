#include "IotProxyInstance.h"
#include <QCoreApplication>
#include <QDebug>

using namespace std;

int main(int argc,char **argv)
{
	QCoreApplication app(argc,argv);
	IotProxyInstance::inst().setup(argc,argv);
	int retVal=app.exec();
	IotProxyInstance::inst().terminate();
	return retVal;
}
