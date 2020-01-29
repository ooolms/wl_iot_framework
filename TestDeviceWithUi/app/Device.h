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
#include "DeviceState.h"

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
	void setWorking(bool w);
	bool isWorking();
	bool isConnected();
	void setStartupState(const DeviceStateMap &m);

signals:
	void connected();
	void disconnected();
	void commandForManualReaction(const QByteArray &cmd,const QByteArrayList &args,
		DeviceReactionConfig::CommandCfg &reaction);

private slots:
	void onBCastCliReadyRead();
	void onSocketReadyRead();
	void onSyncTimer();
	void onSocketConnected();
	void onSocketDisconnected();

private:
	void processMsg2(QByteArray &msg,QByteArrayList &args);
	void writeOk(const QByteArrayList &args);
	void writeErr(const QByteArrayList &args);
	void writeOkNoEscape(const char *str);
	void writeErrNoEscape(const char *str);
	void applyToStateMap(DeviceStateMap &map,std::function<void(
		const QByteArray &cmd,int paramIndex,QByteArray &value)> toCmdState,
			std::function<void(const QByteArray &name,QByteArray &value)> toAdditState);

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
	DeviceStateMap startupState;
	DeviceState state;
	QUdpSocket *bCastCli;
	QHostAddress bCastSerderAddr;
	QTcpSocket *socket;
	QTimer *syncTimer;
	QByteArray callIdStr;
};

#endif // DEVICE_H
