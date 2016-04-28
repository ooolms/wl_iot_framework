#ifndef ARPCTTYDEVICE_H
#define ARPCTTYDEVICE_H

#include "ARpcStreamParser.h"
#include "ARpcMessageParser.h"
#include "ARpcDevice.h"
#include <QFileSystemWatcher>
#include <QSocketNotifier>
#include <QFile>

class ARpcTtyDevice
	:public ARpcDevice
{
	Q_OBJECT
public:
	explicit ARpcTtyDevice(const QString &path,const ARpcConfig &cfg,QObject *parent=0);
	virtual ~ARpcTtyDevice();
	virtual bool writeMsg(const ARpcMessage &m)override;
	virtual bool isConnected()override;

private slots:
	void onWatcherFileChanged(const QString &filePath);
	void onWatcherDirChanged(const QString &dirPath);
	void onReadyRead();

private:
	void tryOpen();
	void closeTty();
	void setupSerialPort();

private:
	QString ttyPath;
	QFileSystemWatcher watcher;
	bool connectedFlag;
	int fd;
	QFile *file;
	QSocketNotifier *notif;
};

#endif // ARPCTTYDEVICE_H
