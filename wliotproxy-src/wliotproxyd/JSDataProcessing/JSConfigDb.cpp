#include "JSConfigDb.h"

JSConfigDb::JSConfigDb(const QString &programPath,const QByteArray &programId)
	:BaseProgramConfigDb(programPath,programId)
{
	load();
}

void JSConfigDb::setup(BaseProgramEngine *e)
{
	Q_UNUSED(e)
}

void JSConfigDb::cleanup(BaseProgramEngine *e,const QByteArray &oldData)
{
	Q_UNUSED(e)
	Q_UNUSED(oldData)
}

void JSConfigDb::loadOther(QDomElement &rootElem)
{
	Q_UNUSED(rootElem)
}

void JSConfigDb::storeOther(QDomElement &rootElem)
{
	Q_UNUSED(rootElem)
}
