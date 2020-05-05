#ifndef IOTSERVERGDILPROGRAMSCOMMANDS_H
#define IOTSERVERGDILPROGRAMSCOMMANDS_H

#include "IotServerConnection.h"
#include "GDIL/core/BaseBlock.h"
#include "GDIL/core/Program.h"

class IotServerGDILProgramsCommands
	:public QObject
{
	Q_OBJECT
public:
	explicit IotServerGDILProgramsCommands(IotServerConnection *conn);
	bool list(QByteArrayList &programs,QList<bool> &states);
	bool get(const QByteArray &programName,QByteArray &text);
	bool remove(const QByteArray &programName);
	bool upload(const QByteArray &programName,const QByteArray &text);
	bool start(const QByteArray &programName);
	bool stop(const QByteArray &programName);
	bool restart(const QByteArray &programName);
	bool listConfigOptions(const QByteArray &programName,QMap<ConfigOptionId,TypeConstraints> &configOptionConstraints,
		QMap<ConfigOptionId,DataUnit> &configOptionValues);
	bool setConfigOption(const QByteArray &programName,const ConfigOptionId &id,const DataUnit &data);
	bool listTimers(const QByteArray &programName,QMap<quint32,TimerBlock::TimerConfig> &timers,
		QMap<quint32,QByteArray> &timerNames);
	bool setTimer(const QByteArray &programName,quint32 blockId,const TimerBlock::TimerConfig &cfg);

private:
	IotServerConnection *srvConn;
};

#endif // IOTSERVERGDILPROGRAMSCOMMANDS_H
