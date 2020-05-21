#include "JSScriptsManager.h"
#include "JSEngine.h"
#include "JSConfigDb.h"
#include "../MainServerConfig.h"

static const QString jsScriptsBaseDir=QString("/var/lib/wliotproxyd/js_data_processing/");

JSScriptsManager::JSScriptsManager(QObject *parent)
	:BaseProgramsManager(jsScriptsBaseDir,parent)
{
	loadPrograms();
}

QString JSScriptsManager::fileExtension()
{
	return ".js";
}

BaseProgramEngine* JSScriptsManager::makeEngine(IdType uid,const QByteArray &data)
{
	JSEngine *e=new JSEngine(uid);
	e->setData(data);
	return e;
}

BaseProgramConfigDb* JSScriptsManager::makeCfgDb(IdType uid,const QString &programPath)
{
	Q_UNUSED(uid)
	return new JSConfigDb(programPath);
}
