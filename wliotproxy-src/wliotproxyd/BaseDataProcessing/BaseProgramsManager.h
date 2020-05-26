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
	QByteArrayList programIds(IdType uid);//ids
	bool isWorking(IdType uid,const QByteArray &programId);
	bool startStopProgram(IdType uid,const QByteArray &programId,bool start);
	bool addProgram(IdType uid,QByteArray programName,const QByteArray &data,QByteArray &id);
	bool getProgram(IdType uid,const QByteArray &programId,QByteArray &data);
	bool removeProgram(IdType uid,const QByteArray &programId);
	bool updateProgram(IdType uid,const QByteArray &programId,const QByteArray &data);
	bool renameProgram(IdType uid,const QByteArray &programId,const QByteArray &name);
	BaseProgramEngine* engine(IdType uid,const QByteArray &programId);
	BaseProgramConfigDb *cfgDb(IdType uid,const QByteArray &programId);

protected:
	void loadPrograms();//call from child constructor
	virtual QString fileExtension()=0;//with .
	virtual BaseProgramEngine *makeEngine(IdType uid,const QByteArray &data)=0;//if data is not parsed, return 0
	virtual BaseProgramConfigDb *makeCfgDb(IdType uid,const QByteArray &programId,const QString &programPath)=0;

private:
	QString mBaseDirPath;
	QMap<IdType,QMap<QByteArray,BaseProgramEngine*>> programsMap;//uid -> (id -> engine)
	QMap<IdType,QMap<QByteArray,BaseProgramConfigDb*>> configsMap;//uid -> (id -> cfg)
};

#endif // BASEPROGRAMSMANAGER_H
