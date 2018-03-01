#include <QCoreApplication>
#include "TestParser.h"
#include "TestDeviceState.h"
#include <QDebug>

int main(int,char *[])
{
	bool ok=testParser();
	ok=ok&&testDeviceState();
	return ok?0:1;
}
