#ifndef BASEPROGRAMSMANAGER_H
#define BASEPROGRAMSMANAGER_H

#include <QObject>
#include "BaseProgramEngine.h"
#include "BaseProgramConfigDb.h"
#include "../AccessManagement/AccessMgr.h"

class BaseProgramsManager
	:public QObject
{
	Q_OBJECT
public:
	explicit BaseProgramsManager(const QString &baseDirPath,QObject *parent=nullptr);
	virtual ~BaseProgramsManager();
	QByteArrayList programs(IdType uid);
	bool isWorking(IdType uid,const QByteArray &programName);
	bool startStopProgram(IdType uid,const QByteArray &programName,bool start);
	bool addProgram(IdType uid,QByteArray programName,const QByteArray &data);
	bool getProgram(IdType uid,const QByteArray &programName,QByteArray &data);
	bool removeProgram(IdType uid,const QByteArray &programName);
	bool updateProgram(IdType uid,const QByteArray &programName,const QByteArray &data);
	BaseProgramEngine* engine(IdType uid,const QByteArray &programName);
	BaseProgramConfigDb *cfgDb(IdType uid,const QByteArray &programName);

protected:
	void loadPrograms();//call from child constructor
	virtual QString fileExtension()=0;//with .
	virtual BaseProgramEngine *makeEngine(IdType uid,const QByteArray &data)=0;//if data is not parsed, return 0
	virtual BaseProgramConfigDb *makeCfgDb(IdType uid,const QString &programPath)=0;

private:
	QString mBaseDirPath;
	QMap<IdType,QMap<QByteArray,BaseProgramEngine*>> programsMap;//uid -> (name -> engine)
	QMap<IdType,QMap<QByteArray,BaseProgramConfigDb*>> configsMap;//uid -> (name -> cfg)
};

#endif // BASEPROGRAMSMANAGER_H
