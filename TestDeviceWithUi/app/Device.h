#ifndef DEVICE_H
#define DEVICE_H

#include <QObject>
#include <QUuid>
#include <QUdpSocket>
#include <QTcpServer>
#include <QTcpSocket>
#include <QTimer>
#include "wliot/devices/StreamParser.h"
#include "wliot/devices/Message.h"
#include "DeviceConfig.h"
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
	void save(const QString &path);
	void load(const QString &path);
	void sendSensorValue(const QByteArray &sensor,const QByteArrayList &value);

signals:
	void connected();
	void disconnected();
	void commandForManualReaction(const QByteArray &cmd,const QByteArrayList &args,CommandReactionConfig &cfg);
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
	QMap<QByteArray,CommandReactionConfig> commands;
	QByteArray sensorsStr,controlsStr;
	QUuid devId;
	QByteArray devName;
	static const QByteArray jsScriptTemplate;
	DeviceStateMap startupState;

private:
	bool working;
	StreamParser parser;
	StreamParser srvReadyParser;
	DeviceState state;
	QUdpSocket *bCastCli;
	QHostAddress bCastSerderAddr;
	QTcpSocket *socket;
	QTimer *syncTimer;
	QByteArray callIdStr;
};

#endif // DEVICE_H
