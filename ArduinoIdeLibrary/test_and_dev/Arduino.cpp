#include "Arduino.h"
#include <QDateTime>

long long startTime=QDateTime::currentMSecsSinceEpoch();

unsigned long millis()
{
	return (unsigned long)(QDateTime::currentMSecsSinceEpoch()-startTime);
}
