#ifndef DEVICESTATE_H
#define DEVICESTATE_H

#include <QByteArray>
#include <QMap>
#include <QObject>
#include "Message.h"

struct DeviceStateMap
{
	QMap<QByteArray,QMap<int,QByteArray>> commands;
	QMap<QByteArray,QByteArray> additionalParams;
};

class DeviceState
	:public QObject
{
	Q_OBJECT
public:
	explicit DeviceState();
	void prepareCommandParamState(const QByteArray &cmd,int paramIndex,const QByteArray &value);
	void prepareAdditionalParameter(const QByteArray &name,const QByteArray &value);
	void setCommandParamState(const QByteArray &cmd,int paramIndex,const QByteArray &value);
	void setAdditionalParamState(const QByteArray &name,const QByteArray &value);
	void dump(Message &m);

signals:
	void writeMsg(const Message &m);

private:
	void notifyCommandParamChanged(const QByteArray &cmd,int paramIndex);
	void writeCommandParamState(const QByteArray &cmd,int paramIndex,Message &m);
	void notifyAdditionalParamChanged(const QByteArray &name);
	void writeAdditionalParamState(const QByteArray &name,Message &m);

public:
	DeviceStateMap state;
};

#endif // DEVICESTATE_H
