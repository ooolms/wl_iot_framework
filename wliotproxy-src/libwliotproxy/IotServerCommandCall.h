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
	bool ok();
	const QByteArrayList& returnValue();

private slots:
	void onMessage(const Message &m);
	void onDisconnected();
	void onTimeout();

protected:
	QTimer tmr;
	QByteArrayList retVal;
	QEventLoop loop;
	CmDataCallback mOnCmData;
	QByteArray callId;
	bool done;
	bool mOk;
};

#endif // IOTSERVERCOMMANDCALL_H
