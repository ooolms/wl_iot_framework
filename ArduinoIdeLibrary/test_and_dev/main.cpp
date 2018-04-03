#include <QCoreApplication>
#include "TestParser.h"
#include "TestDeviceState.h"
#include "TestUuid.h"
#include "TestStarNet.h"
#include <QDebug>

int main(int,char *[])
{
	bool ok=testParser();
	qDebug()<<"\n";
	ok=ok&&testDeviceState();
	qDebug()<<"\n";
	ok=ok&&testUuid();
	qDebug()<<"\n";
	ok=ok&&testStarNet();
	return ok?0:1;
}
