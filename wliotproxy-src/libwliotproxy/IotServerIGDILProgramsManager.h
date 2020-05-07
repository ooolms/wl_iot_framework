#ifndef IOTSERVERIGDILPROGRAMSMANAGER_H
#define IOTSERVERIGDILPROGRAMSMANAGER_H

#include <QObject>
#include <QByteArray>
#include "GDIL/core/Program.h"

class IotServerIGDILProgramsManager
	:public QObject
{
	Q_OBJECT
public:
	explicit IotServerIGDILProgramsManager(QObject *parent=nullptr);
	virtual QByteArrayList programs()=0;
	virtual bool get(const QByteArray &programName,QByteArray &text)=0;
	virtual bool isWorking(const QByteArray &programName)=0;
	virtual bool setProgramXml(const QByteArray &programName,const QByteArray &xml)=0;
	virtual bool remove(const QByteArray &programName)=0;
	virtual void start(const QByteArray &programName)=0;
	virtual void stop(const QByteArray &programName)=0;
	virtual void restart(const QByteArray &programName)=0;
	virtual bool listConfigOptions(const QByteArray &programName,
		QMap<ConfigOptionId,TypeConstraints> &configOptionConstraints,
		QMap<ConfigOptionId,DataUnit> &configOptionValues)=0;
	virtual bool setConfigOption(const QByteArray &programName,const ConfigOptionId &id,const DataUnit &data)=0;
	virtual bool listTimers(const QByteArray &programName,QMap<quint32,TimerBlock::TimerConfig> &timers,
		QMap<quint32,QByteArray> &timerNames)=0;
	virtual bool setTimer(const QByteArray &programName,quint32 blockId,const TimerBlock::TimerConfig &cfg)=0;

signals:
	void stateChanged(const QByteArray &programName,bool isWorking);
};

#endif // IOTSERVERIGDILPROGRAMSMANAGER_H
