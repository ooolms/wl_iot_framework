#ifndef DEVICE_H
#define DEVICE_H

#include <QObject>
#include <QUuid>
#include <QUdpSocket>
#include <QTcpServer>
#include <QTcpSocket>
#include <QTimer>
#include "StreamParser.h"
#include "Message.h"
#include "DeviceReactionConfig.h"
#include "DeviceState.h"

class Device
	:public QObject
{
	Q_OBJECT
public:
	explicit Device(QObject *parent=nullptr);
	bool connectToServer(const QHostAddress &addr);
	void processMsg(const char *msg,const char **args,unsigned char argsCount);
	void setWorking(bool w);
	bool isWorking();
	bool isConnected();
	void setStartupState(const DeviceStateMap &m);
	void setSensors(const QByteArray &str);
	void setControls(const QByteArray &str);
	void writeMsg(const Message &m);
	void setupUidAndName(const QUuid &id,const QByteArray &name);
	QHostAddress serverAddr();

signals:
	void connected();
	void disconnected();
	void commandForManualReaction(const QByteArray &cmd,const QByteArrayList &args,
		DeviceReactionConfig::CommandCfg &reaction);
	void dbgMessage(const QString &msg);
	void infoMessage(const QString &msg);
	void warningMessage(const QString &msg);
	void errorMessage(const QString &msg);
	void serverFound(const QHostAddress &addr,const QUuid &uid,const QByteArray &name);
	void workingChanged();

private slots:
	void onBCastCliReadyRead();
	void onSocketReadyRead();
	void onSyncTimer();
	void onSocketConnected();
	void onSocketDisconnected();
	void onNewMessage(Message m);
	void onNewSrvReadyMessage(Message m);

private:
	void writeOk(const QByteArrayList &args);
	void writeErr(const QByteArrayList &args);
	void applyToStateMap(DeviceStateMap &map,std::function<void(
		const QByteArray &cmd,int paramIndex,QByteArray &value)> toCmdState,
			std::function<void(const QByteArray &name,QByteArray &value)> toAdditState);

public:
	bool disconnectOnSyncTimeout;
	bool answerSyncMsgs;
	DeviceReactionConfig *reactionCfg;
	bool working;
	QByteArray sensorsStr,controlsStr;
	QUuid devId;
	QByteArray devName;

private:
	StreamParser parser;
	StreamParser srvReadyParser;
	DeviceStateMap startupState;
	DeviceState state;
	QUdpSocket *bCastCli;
	QHostAddress bCastSerderAddr;
	QTcpSocket *socket;
	QTimer *syncTimer;
	QByteArray callIdStr;
};

#endif // DEVICE_H
