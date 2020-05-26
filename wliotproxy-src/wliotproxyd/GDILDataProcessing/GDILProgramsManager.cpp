#include "GDILProgramsManager.h"
#include "../MainServerConfig.h"
#include "GDIL/xml/ProgramXmlParser.h"
#include "GDILProgramConfigDb.h"
#include "GDILEngine.h"
#include <QDir>

static const QString programsBaseDir=QString("/var/lib/wliotproxyd/gdil_data_processing/");

using namespace WLIOT;
using namespace WLIOTGDIL;

GDILProgramsManager::GDILProgramsManager(QObject *parent)
	:BaseProgramsManager(programsBaseDir,parent)
{
	loadPrograms();
}

QString GDILProgramsManager::fileExtension()
{
	return ".gdil";
}

BaseProgramEngine* GDILProgramsManager::makeEngine(IdType uid,const QByteArray &data)
{
	Program *p=ProgramXmlParser::fromXml(&bxpf,&bf,data);
	if(!p)return 0;
	GDILEngine *e=new GDILEngine(uid,&bf,&bxpf);
	e->setProgram(p);
	return e;
}

BaseProgramConfigDb* GDILProgramsManager::makeCfgDb(IdType uid,const QByteArray &programId,const QString &programPath)
{
	Q_UNUSED(uid)
	return new GDILProgramConfigDb(programPath,programId);
}
