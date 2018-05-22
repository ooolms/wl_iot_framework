#ifndef ARPCSERIALDRIVER_H
#define ARPCSERIALDRIVER_H

#include <QObject>
#include <QFile>
#include <QSocketNotifier>

#ifdef Q_OS_WIN
#include <windows.h>
#else
#include <errno.h>
#endif

class ARpcSerialDriver
	:public QObject
{
	Q_OBJECT
public:
	enum Error
	{
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
	QSocketNotifier *readNotif,*exceptNotif;
	Error lastError;
#ifdef Q_OS_WIN
	HANDLE fd;
#else
	int fd;
#endif
};

#endif // ARPCSERIALDRIVER_H
