#include "IotProxyInstance.h"
#include <QCoreApplication>
#include <QDebug>
#include <sys/syslog.h>

using namespace std;

int main(int argc,char **argv)
{
	QCoreApplication app(argc,argv);
	IotProxyInstance::inst().setup(argc,argv);
	int retVal=app.exec();
	return retVal;
}
