#ifndef JSSCRIPTSMANAGER_H
#define JSSCRIPTSMANAGER_H

#include <QObject>
#include "../BaseDataProcessing/BaseProgramsManager.h"

class JSScriptsManager
	:public BaseProgramsManager
{
	Q_OBJECT
public:
	explicit JSScriptsManager(QObject *parent=nullptr);

protected:
	virtual QString fileExtension()override;
	virtual BaseProgramEngine *makeEngine(IdType uid, const QByteArray &data) override;
	virtual BaseProgramConfigDb *makeCfgDb(IdType uid, const QString &programPath) override;
};

#endif // JSSCRIPTSMANAGER_H
