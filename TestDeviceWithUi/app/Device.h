#ifndef DEVICE_H
#define DEVICE_H

#include <QObject>
#include <QUdpSocket>
#include <QTcpServer>
#include <QTcpSocket>
#include <QTimer>
#include "ARpcDevice.h"
#include "ARpcSrvReady.h"
#include "ARpcStreamParser.h"
#include "ARpcStreamWriter.h"
#include "DeviceReactionConfig.h"
#include "ARpcDeviceState.h"

class Device
	:public QObject
	,public ARpcISrvReadyCallback
	,public ARpcIWriteCallback
	,public ARpcIMessageCallback

{
	Q_OBJECT
public:
	explicit Device(QObject *parent=nullptr);
	bool connectToServer(const QHostAddress &addr);
	void processSrvReadyMsg(const ARpcUuid &serverId,const char *serverName);
	void writeData(const char *data,unsigned long sz);
	void writeStr(const char *str);
	void processMsg(const char *msg,const char **args,unsigned char argsCount);

signals:
	void connected();
	void disconnected();

private slots:
	void onBCastCliReadyRead();
	void onSocketReadyRead();
	void onSyncTimer();
	void onSocketConnected();
	void onSocketDisconnected();

private:
	void writeOk(const char *arg1=0,const char *arg2=0,const char *arg3=0,const char *arg4=0);
	void writeErr(const char *arg1=0,const char *arg2=0,const char *arg3=0,const char *arg4=0);
	void writeOkNoEscape(const char *str);
	void writeErrNoEscape(const char *str);

public:
	bool disconnectOnSyncTimeout;
	bool answerSyncMsgs;
	DeviceReactionConfig *reactionCfg;
	bool working;
	QByteArray sensorsStr,controlsStr;

private:
	ARpcSrvReady srvReady;
	ARpcStreamParser parser;
	ARpcStreamWriter writer;
	ARpcDeviceState state;
	QUdpSocket *bCastCli;
	QHostAddress bCastSerderAddr;
	QTcpSocket *socket;
	QTimer *syncTimer;
	QByteArray callIdStr;
};

#endif // DEVICE_H
