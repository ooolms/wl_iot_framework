#ifndef JSPROGRAMCONFIGDB_H
#define JSPROGRAMCONFIGDB_H

#include "../Processing/BaseProgramConfigDb.h"

class JSProgramConfigDb
	:public BaseProgramConfigDb
{
public:
	explicit JSProgramConfigDb(const QString &programPath,const QByteArray &programId);

protected:
	virtual void loadOther(QDomElement &rootElem)override;
	virtual void storeOther(QDomElement &rootElem)override;
};

#endif // JSPROGRAMCONFIGDB_H
