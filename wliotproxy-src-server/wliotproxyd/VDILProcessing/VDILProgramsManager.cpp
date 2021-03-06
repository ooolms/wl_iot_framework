#include "VDILProgramsManager.h"
#include "../MainServerConfig.h"
#include "VDIL/xml/ProgramXmlParser.h"
#include "VDILProgramConfigDb.h"
#include <VDIL/CoreBlocksPlugin.h>
#include <QDir>

static const QString programsBaseDir=QString("/var/lib/wliotproxyd/vdil_data_processing/");

using namespace WLIOT;
using namespace WLIOTVDIL;

VDILProgramsManager::VDILProgramsManager(QObject *parent)
	:BaseProgramsManager(programsBaseDir,parent)
{
	CoreBlocksPlugin *p=new CoreBlocksPlugin;
	if(!e.plg.registerPlugin(p))
		delete p;
}

QString VDILProgramsManager::fileExtension()
{
	return ".vdil";
}

BaseProgramConfigDb* VDILProgramsManager::makeCfgDb(IdType uid,const QByteArray &programId,const QString &programPath)
{
	Q_UNUSED(uid)
	return new VDILProgramConfigDb(programPath,programId);
}

QString VDILProgramsManager::processName()
{
	return "wliotproxy-run-vdil";
}

Program* VDILProgramsManager::getVDILProgram(IdType uid,const QByteArray &programId)
{
	QByteArray data;
	if(!getProgram(uid,programId,data))return 0;
	return ProgramXmlParser::fromXml(&e,data,false);
}
