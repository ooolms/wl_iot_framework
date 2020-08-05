#include "BaseProgramEngine.h"

BaseProgramEngine::BaseProgramEngine(IdType uid,QObject *parent)
	:QObject(parent)
{
	mUid=uid;
}

void BaseProgramEngine::setProgramName(const QByteArray &n)
{
	mProgramName=n;
}

QByteArray BaseProgramEngine::programName()
{
	return mProgramName;
}

void BaseProgramEngine::onProgramNameChanged()
{
}

IdType BaseProgramEngine::uid()
{
	return mUid;
}
