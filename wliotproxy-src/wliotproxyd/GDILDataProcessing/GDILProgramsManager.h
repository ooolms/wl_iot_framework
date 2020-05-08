#ifndef GDILPROGRAMSMANAGER_H
#define GDILPROGRAMSMANAGER_H

#include <QObject>
#include "../AccessManagement/AccessPolicyTypes.h"
#include "GDILEngine.h"
#include "GDIL/core/BlocksFactory.h"
#include "GDIL/xml/BlocksXmlParserFactory.h"
#include "GDILEngineCallbacks.h"
#include "GDILEngineHelper.h"
#include "GDILProgramConfigDb.h"

class GDILProgramsManager
	:public QObject
{
	Q_OBJECT
public:
	explicit GDILProgramsManager(QObject *parent=nullptr);
	virtual ~GDILProgramsManager();
	QStringList programs(IdType uid);
	bool isWorking(IdType uid,const QString &programName);
	bool startStopProgram(IdType uid,const QString &programName,bool start);
	bool addProgram(IdType uid,QString programName,const QByteArray &text);
	bool getProgram(IdType uid,const QString &programName,QByteArray &text);
	bool removeProgram(IdType uid,const QString &programName);
	bool updateProgram(IdType uid,const QString &programName,const QByteArray &text);
	Program* program(IdType uid,const QString &programName);
	GDILEngine* engine(IdType uid,const QString &programName);
	GDILProgramConfigDb *cfgDb(IdType uid,const QString &programName);

private:
	QMap<IdType,QMap<QString,GDILEngine*>> programsMap;
	QMap<IdType,QMap<QString,GDILProgramConfigDb*>> configsMap;
	GDILEngineHelper helper;
	GDILEngineCallbacks cmdCb;
	BlocksFactory blocksFactory;
	BlocksXmlParserFactory blocksXmlFactory;
};

#endif // GDILPROGRAMSMANAGER_H