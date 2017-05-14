#ifndef ARPCTTYDEVICE_H
#define ARPCTTYDEVICE_H

#include "ARpcBase/ARpcStreamParser.h"
#include "ARpcBase/ARpcMessageParser.h"
#include "ARpcBase/ARpcDevice.h"
#include <QFileSystemWatcher>
#include <QSerialPort>
#include <QTimer>

class ARpcTtyDevice
	:public ARpcDevice
{
	Q_OBJECT
public:
	explicit ARpcTtyDevice(const QString &portName,QObject *parent=0);
	virtual ~ARpcTtyDevice();
	virtual bool writeMsg(const ARpcMessage &m)override;
	virtual bool isConnected()override;
	QString portName()const;

private slots:
//	void onWatcherFileChanged(const QString &filePath);
//	void onWatcherDirChanged(const QString &dirPath);
	void onReadyRead();
	void onPortError(QSerialPort::SerialPortError err);
	void tryOpen();

private:
	void closeTty();
	void setupSerialPort();

private:
//	QFileSystemWatcher watcher;
	bool connectedFlag;
	QTimer reconnectTimer;
//	int fd;
//	QFile *file;
//	QSocketNotifier *notif;
	QSerialPort *ttyPort;
};

#endif // ARPCTTYDEVICE_H
