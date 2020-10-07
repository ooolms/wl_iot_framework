#ifndef BASEPROGRAMSMANAGER_H
#define BASEPROGRAMSMANAGER_H

#include <QObject>
#include <QProcess>
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
	void loadPrograms();
	QByteArrayList programIds(IdType uid);//ids
	bool isRunning(IdType uid,const QByteArray &programId);
	bool startStopProgram(IdType uid,const QByteArray &programId,bool start);
	bool addProgram(IdType uid,QByteArray programName,const QByteArray &data,QByteArray &id);
	bool getProgram(IdType uid,const QByteArray &programId,QByteArray &data);
	bool removeProgram(IdType uid,const QByteArray &programId);
	bool updateProgram(IdType uid,const QByteArray &programId,const QByteArray &data);
	bool renameProgram(IdType uid,const QByteArray &programId,const QByteArray &name);
	BaseProgramConfigDb *cfgDb(IdType uid,const QByteArray &programId);

protected:
	virtual QString fileExtension()=0;//with .
	virtual QString processName()=0;
	virtual BaseProgramConfigDb *makeCfgDb(IdType uid,const QByteArray &programId,const QString &programPath)=0;

private slots:
	void onProcReadyRead();

private:
	void stopProcess(QProcess *proc);

private:
	QString mBaseDirPath;
	QString processRunPath;
	QMap<IdType,QMap<QByteArray,QProcess*>> programsMap;//uid -> (id -> engine)
	QMap<IdType,QMap<QByteArray,BaseProgramConfigDb*>> configsMap;//uid -> (id -> cfg)
};

#endif // BASEPROGRAMSMANAGER_H
