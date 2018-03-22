#include <QCoreApplication>
#include "TestParser.h"
#include "TestDeviceState.h"
#include "TestUuid.h"
#include <QDebug>

int main(int,char *[])
{
	bool ok=testParser();
	ok=ok&&testDeviceState();
	ok=ok&&testUuid();
	return ok?0:1;
}
