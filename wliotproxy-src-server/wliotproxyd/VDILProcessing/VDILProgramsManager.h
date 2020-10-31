#ifndef VDILPROGRAMSMANAGER_H
#define VDILPROGRAMSMANAGER_H

#include <QObject>
#include "../AccessManagement/AccessPolicyTypes.h"
#include "../Processing/BaseProgramsManager.h"
#include "VDIL/core/Engine.h"
#include "VDILEngineCallbacks.h"
#include "../AccessManagement/AccessMgr.h"

class VDILProgramsManager
	:public BaseProgramsManager
{
	Q_OBJECT
public:
	explicit VDILProgramsManager(QObject *parent=nullptr);
	WLIOTVDIL::Program* getVDILProgram(IdType uid,const QByteArray &programId);

protected:
	virtual QString fileExtension()override;
	virtual BaseProgramConfigDb *makeCfgDb(IdType uid,const QByteArray &programId,const QString &programPath)override;
	virtual QString processName()override;

private:
	WLIOTVDIL::Engine e;
};

#endif // VDILPROGRAMSMANAGER_H
