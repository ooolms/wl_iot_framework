#ifndef JSCONFIGDB_H
#define JSCONFIGDB_H

#include "../BaseDataProcessing/BaseProgramConfigDb.h"

class JSConfigDb
	:public BaseProgramConfigDb
{
public:
	explicit JSConfigDb(const QString &programPath);
	virtual void setup(BaseProgramEngine *e)override;
	virtual void cleanup(BaseProgramEngine *e,const QByteArray &oldData)override;

protected:
	virtual void loadOther(QDomElement &rootElem)override;
	virtual void storeOther(QDomElement &rootElem)override;
};

#endif // JSCONFIGDB_H
