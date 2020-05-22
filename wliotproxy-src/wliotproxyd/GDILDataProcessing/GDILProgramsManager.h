#ifndef GDILPROGRAMSMANAGER_H
#define GDILPROGRAMSMANAGER_H

#include <QObject>
#include "../AccessManagement/AccessPolicyTypes.h"
#include "../BaseDataProcessing/BaseProgramsManager.h"
#include "GDIL/core/BlocksFactory.h"
#include "GDIL/xml/BlocksXmlParserFactory.h"
#include "GDILEngineCallbacks.h"
#include "../AccessManagement/AccessMgr.h"

class GDILProgramsManager
	:public BaseProgramsManager
{
	Q_OBJECT
public:
	explicit GDILProgramsManager(QObject *parent=nullptr);

protected:
	virtual QString fileExtension()override;
	virtual BaseProgramEngine *makeEngine(IdType uid,const QByteArray &data)override;
	virtual BaseProgramConfigDb *makeCfgDb(IdType uid,const QString &programPath)override;

private:
	GDILEngineCallbacks cmdCb;
	WLIOTGDIL::BlocksFactory bf;
	WLIOTGDIL::BlocksXmlParserFactory bxpf;
};

#endif // GDILPROGRAMSMANAGER_H
