#ifndef IOTSERVERCOMMANDCALL_H
#define IOTSERVERCOMMANDCALL_H

#include <QObject>
#include <QTimer>
#include <QByteArrayList>
#include <QEventLoop>
#include "IotServerTypes.h"

class IotServerConnection;

class IotServerCommandCall
	:public QObject
{
	Q_OBJECT
public:
	explicit IotServerCommandCall(IotServerConnection *conn,CmDataCallback onCmData,const QByteArray &cmd,
		const QByteArrayList &args,QObject *parent=nullptr);
	void call();
	bool ok();
	const QByteArrayList& returnValue();

private slots:
	void onMessage(const Message &m);
	void onDisconnected();
	void onTimeout();

private:
	Q_INVOKABLE void setDone();

protected:
	QTimer tmr;
	IotServerConnection *srvConn;
	QByteArray mCmd;
	QByteArrayList mArgs,retVal;
	CmDataCallback mOnCmData;
	QByteArray callId;
	bool done;
	bool mOk;
};

#endif // IOTSERVERCOMMANDCALL_H
