#include "JSProgramConfigDb.h"

JSProgramConfigDb::JSProgramConfigDb(const QString &programPath,const QByteArray &programId)
	:BaseProgramConfigDb(programPath,programId)
{
	load();
}

void JSProgramConfigDb::loadOther(QDomElement &rootElem)
{
	Q_UNUSED(rootElem)
}

void JSProgramConfigDb::storeOther(QDomElement &rootElem)
{
	Q_UNUSED(rootElem)
}
