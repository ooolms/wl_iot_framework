#ifndef TESTDEVICE_H
#define TESTDEVICE_H

#include <QObject>
#include <QUuid>
#include <QUdpSocket>
#include <QTcpServer>
#include <QTcpSocket>
#include <QTimer>
#include <QScriptEngine>
#include "wliot/devices/StreamParser.h"
#include "wliot/devices/Message.h"
#include "TestDeviceConfig.h"
#include "TestDeviceState.h"
#include "JSTimers.h"

class TestDevice
	:public QObject
{
	Q_OBJECT
public:
	explicit TestDevice(QObject *parent=nullptr);
	virtual ~TestDevice();
	bool connectToServer(const QHostAddress &addr);
	void disconnectFromServer();
	void processMsg(const char *msg,const char **args,unsigned char argsCount);
	void setWorking(bool w);
	bool isWorking();
	bool isConnected();
	void writeMsg(const Message &m);
	void setupUidAndName(const QUuid &id,const QByteArray &name);
	void setSensors(const QByteArray &s);
	void setControls(const QByteArray &s);
	QUuid id()const;
	QByteArray name()const;
	QByteArray sensors();
	QByteArray controls();
	QHostAddress serverAddr();
	void save(const QString &path);
	void load(const QString &path);
	void sendSensorValue(const QByteArray &sensor,const QByteArrayList &value);
	Q_INVOKABLE void resetDevice();

signals:
	void connected();
	void disconnected();
	void commandForManualReaction(const QByteArray &cmd,const QByteArrayList &args,CommandReactionConfig &cfg);
	void dbgMessage(const QByteArray &msg);
	void infoMessage(const QByteArray &msg);
	void warningMessage(const QByteArray &msg);
	void errorMessage(const QByteArray &msg);
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
		const QByteArray &cmd,quint32 paramIndex,QByteArray &value)> toCmdState,
			std::function<void(const QByteArray &name,QByteArray &value)> toAdditState);
	void applyFromJsToStateMap(QScriptValue &val,DeviceStateMap &map);

public:
	bool disconnectOnSyncTimeout;
	bool answerSyncMsgs;
	QMap<QByteArray,CommandReactionConfig> commands;
	static const QByteArray jsScriptTemplate;
	DeviceStateMap startupState;

private:
	bool working;
	QUuid devId;
	QByteArray devName;
	QByteArray sensorsStr,controlsStr;
	StreamParser parser;
	StreamParser srvReadyParser;
	TestDeviceState state;
	QUdpSocket *bCastCli;
	QHostAddress bCastSerderAddr;
	QTcpSocket *socket;
	QTimer *syncTimer;
	QByteArray callIdStr;
	QScriptEngine *scriptEng;
	JSTimers *timers;
};

#endif // TESTDEVICE_H
