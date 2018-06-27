#ifndef ARPCSERIALDRIVER_H
#define ARPCSERIALDRIVER_H

#include <QObject>
#include <QFile>
#include <QSocketNotifier>

#ifdef Q_OS_WIN
#include <windows.h>
#include <QWinEventNotifier>
#else
#include <errno.h>
#endif

#ifdef Q_OS_WIN
#include <QThread>
class ARpcWinCommEventListener
	:public QThread
{
	Q_OBJECT
public:
	explicit ARpcWinCommEventListener(HANDLE f,QObject *parent=0);
	virtual ~ARpcWinCommEventListener();

signals:
	void readyRead();

protected:
	virtual void run();

private:
	HANDLE fd;
};
#endif

class ARpcSerialDriver
	:public QObject
{
	Q_OBJECT
public:
	enum Error
	{
		NoError,
		AccessError,
		ReadError,
		WriteError,
		UnknownError
	};

public:
	explicit ARpcSerialDriver(const QString &portName,QObject *parent=0);
	virtual ~ARpcSerialDriver();
	bool open();
	bool isOpened();
	void close();
	Error errorCode();
	QString errorString();
	QByteArray readAll();
	QByteArray read(qint64 sz);
	qint64 write(const QByteArray &data);
	QString portName();

signals:
	void readyRead();
	void error();

private:
	void setupSerialPort();

private:
	QString mPortName;
//	QFile mFile;
	Error lastError;
#ifdef Q_OS_WIN
	HANDLE fd;
	ARpcWinCommEventListener *readNotif;
#else
	int fd;
	QSocketNotifier *readNotif;
#endif
};

#endif // ARPCSERIALDRIVER_H
