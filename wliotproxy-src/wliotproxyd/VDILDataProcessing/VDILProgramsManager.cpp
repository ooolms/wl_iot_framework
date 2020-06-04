#include "VDILProgramsManager.h"
#include "../MainServerConfig.h"
#include "VDIL/xml/ProgramXmlParser.h"
#include "VDILProgramConfigDb.h"
#include "VDILEngine.h"
#include <QDir>

static const QString programsBaseDir=QString("/var/lib/wliotproxyd/vdil_data_processing/");

using namespace WLIOT;
using namespace WLIOTVDIL;

VDILProgramsManager::VDILProgramsManager(QObject *parent)
	:BaseProgramsManager(programsBaseDir,parent)
{
	loadPrograms();
}

QString VDILProgramsManager::fileExtension()
{
	return ".vdil";
}

BaseProgramEngine* VDILProgramsManager::makeEngine(IdType uid,const QByteArray &data)
{
	Program *p=ProgramXmlParser::fromXml(&bxpf,&bf,data);
	if(!p)return 0;
	VDILEngine *e=new VDILEngine(uid,&bf,&bxpf);
	e->setProgram(p);
	return e;
}

BaseProgramConfigDb* VDILProgramsManager::makeCfgDb(IdType uid,const QByteArray &programId,const QString &programPath)
{
	Q_UNUSED(uid)
	return new VDILProgramConfigDb(programPath,programId);
}
