#ifndef IOTSERVERGDILPROGRAMS_H
#define IOTSERVERGDILPROGRAMS_H

#include "IotServerIGDILProgramsManager.h"
#include "IotServerConnection.h"
#include "IotServerCommands.h"

class IotServerGDILPrograms
	:public IotServerIGDILProgramsManager
{
	Q_OBJECT
public:
	explicit IotServerGDILPrograms(IotServerConnection *conn,IotServerCommands *cmds);
	virtual QByteArrayList programs()override;
	virtual bool get(const QByteArray &programName,QByteArray &text)override;
	virtual bool isWorking(const QByteArray &programName)override;
	virtual bool setProgramXml(const QByteArray &programName,const QByteArray &xml)override;
	virtual bool remove(const QByteArray &programName)override;
	virtual void start(const QByteArray &programName)override;
	virtual void stop(const QByteArray &programName)override;
	virtual void restart(const QByteArray &programName)override;
	virtual bool listConfigOptions(const QByteArray &programName,
		QMap<ConfigOptionId,TypeConstraints> &configOptionConstraints,
		QMap<ConfigOptionId,DataUnit> &configOptionValues)override;
	virtual bool setConfigOption(const QByteArray &programName,const ConfigOptionId &id,const DataUnit &data)override;
	virtual bool listTimers(const QByteArray &programName,QMap<quint32,TimerBlock::TimerConfig> &timers,
		QMap<quint32,QByteArray> &timerNames)override;
	virtual bool setTimer(const QByteArray &programName,quint32 blockId,const TimerBlock::TimerConfig &cfg)override;
	bool reloadPrograms();

private slots:
	void onConnected();
	void onDisconnected();

private:
	IotServerConnection *srvConn;
	IotServerCommands *srvCmds;
	QMap<QByteArray,bool> programsMap;
	bool ready;
};

#endif // IOTSERVERGDILPROGRAMS_H
