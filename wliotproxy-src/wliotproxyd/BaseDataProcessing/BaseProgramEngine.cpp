#include "BaseProgramEngine.h"

BaseProgramEngine::BaseProgramEngine(QObject *parent)
	:QObject(parent)
{
}

void BaseProgramEngine::setProgramName(const QByteArray &n)
{
	mProgramName=n;
}

QByteArray BaseProgramEngine::programName()
{
	return mProgramName;
}
