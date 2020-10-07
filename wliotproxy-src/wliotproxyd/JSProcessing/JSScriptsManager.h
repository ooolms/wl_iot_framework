#ifndef JSSCRIPTSMANAGER_H
#define JSSCRIPTSMANAGER_H

#include <QObject>
#include "../Processing/BaseProgramsManager.h"

class JSScriptsManager
	:public BaseProgramsManager
{
	Q_OBJECT
public:
	explicit JSScriptsManager(QObject *parent=nullptr);

protected:
	virtual QString fileExtension()override;
	virtual BaseProgramConfigDb *makeCfgDb(IdType uid,const QByteArray &programId,const QString &programPath)override;
	virtual QString processName() override;
};

#endif // JSSCRIPTSMANAGER_H
