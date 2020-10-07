#include "JSScriptsManager.h"
#include "JSEngine.h"
#include "JSProgramConfigDb.h"
#include "../MainServerConfig.h"

static const QString jsScriptsBaseDir=QString("/var/lib/wliotproxyd/js_data_processing/");

JSScriptsManager::JSScriptsManager(QObject *parent)
	:BaseProgramsManager(jsScriptsBaseDir,parent)
{
}

QString JSScriptsManager::fileExtension()
{
	return ".js";
}

BaseProgramConfigDb* JSScriptsManager::makeCfgDb(IdType uid,const QByteArray &programId,const QString &programPath)
{
	Q_UNUSED(uid)
	return new JSProgramConfigDb(programPath,programId);
}


QString JSScriptsManager::processName()
{
	return "wliotproxy-run-js";
}
