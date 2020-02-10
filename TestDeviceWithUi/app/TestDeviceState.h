#ifndef TESTDEVICESTATE_H
#define TESTDEVICESTATE_H

#include <QByteArray>
#include <QMap>
#include <QObject>
#include "wliot/devices/Message.h"

struct DeviceStateMap
{
	QMap<QByteArray,QMap<quint32,QByteArray>> commands;
	QMap<QByteArray,QByteArray> additionalParams;
};

class TestDeviceState
	:public QObject
{
	Q_OBJECT
public:
	explicit TestDeviceState();
	void prepareCommandParamState(const QByteArray &cmd,quint32 paramIndex,const QByteArray &value);
	void prepareAdditionalParameter(const QByteArray &name,const QByteArray &value);
	void setCommandParamState(const QByteArray &cmd,quint32 paramIndex,const QByteArray &value);
	void setAdditionalParamState(const QByteArray &name,const QByteArray &value);
	void dump(Message &m);

signals:
	void writeMsg(const Message &m);

private:
	void notifyCommandParamChanged(const QByteArray &cmd,quint32 paramIndex);
	void writeCommandParamState(const QByteArray &cmd,quint32 paramIndex,Message &m);
	void notifyAdditionalParamChanged(const QByteArray &name);
	void writeAdditionalParamState(const QByteArray &name,Message &m);

public:
	DeviceStateMap state;
};

#endif // TESTDEVICESTATE_H
