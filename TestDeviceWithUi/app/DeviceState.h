#ifndef DEVICESTATE_H
#define DEVICESTATE_H

#include <QByteArray>
#include <QMap>

class ARpcStreamWriter;

struct DeviceStateMap
{
	QMap<QByteArray,QMap<int,QByteArray>> commands;
	QMap<QByteArray,QByteArray> additionalParams;
};

class DeviceState
{
public:
	explicit DeviceState(ARpcStreamWriter *wr);
	void prepareCommandParamState(const QByteArray &cmd,int paramIndex,const QByteArray &value);
	void prepareAdditionalParameter(const QByteArray &name,const QByteArray &value);
	void setCommandParamState(const QByteArray &cmd,int paramIndex,const QByteArray &value);
	void setAdditionalParamState(const QByteArray &name,const QByteArray &value);
	void dump();

private:
	void notifyCommandParamChanged(const QByteArray &cmd,int paramIndex);
	void writeCommandParamState(const QByteArray &cmd,int paramIndex);
	void notifyAdditionalParamChanged(const QByteArray &name);
	void writeAdditionalParamState(const QByteArray &name);
	void writeUInt(unsigned int c);

public:
	DeviceStateMap m;

private:
	ARpcStreamWriter *writer;
};

#endif // DEVICESTATE_H
