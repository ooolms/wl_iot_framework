#ifndef VDILPROGRAMSMANAGER_H
#define VDILPROGRAMSMANAGER_H

#include <QObject>
#include "../AccessManagement/AccessPolicyTypes.h"
#include "../BaseDataProcessing/BaseProgramsManager.h"
#include "VDIL/core/BlocksFactory.h"
#include "VDIL/xml/BlocksXmlParserFactory.h"
#include "VDILEngineCallbacks.h"
#include "../AccessManagement/AccessMgr.h"

class VDILProgramsManager
	:public BaseProgramsManager
{
	Q_OBJECT
public:
	explicit VDILProgramsManager(QObject *parent=nullptr);

protected:
	virtual QString fileExtension()override;
	virtual BaseProgramEngine *makeEngine(IdType uid,const QByteArray &data)override;
	virtual BaseProgramConfigDb *makeCfgDb(IdType uid,const QByteArray &programId,const QString &programPath)override;

private:
	VDILEngineCallbacks cmdCb;
	WLIOTVDIL::BlocksFactory bf;
	WLIOTVDIL::BlocksXmlParserFactory bxpf;
};

#endif // VDILPROGRAMSMANAGER_H
