#ifndef ARPCSERIALDRIVER_H
#define ARPCSERIALDRIVER_H

#include <QObject>
#include <QFile>
#include <QThread>
#include <QTimer>
#include <QMutex>

#ifdef Q_OS_WIN
#include <windows.h>
#else
#include <errno.h>
#include <signal.h>
#endif

#ifdef Q_OS_WIN
	typedef HANDLE FileDescrType;
#else
	typedef int FileDescrType;
#endif

class ARpcCommReader
	:public QThread
{
	Q_OBJECT
public:
	explicit ARpcCommReader(FileDescrType f,QObject *parent=0);
	virtual ~ARpcCommReader();

signals:
	void newData(const QByteArray &data);
	void readError();

protected:
	virtual void run();

private:
	void onTimer();

private:
	QTimer *t;
	QMutex m;
	FileDescrType fd;
	QByteArray data;
};

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
	qint64 write(const QByteArray &data);
	QString portName();
	void startReader();

signals:
	void newData(const QByteArray &data);
	void error();

private:
	void setupSerialPort();

private:
	QString mPortName;
//	QFile mFile;
	Error lastError;
	FileDescrType fd;
	ARpcCommReader *reader;
};

#endif // ARPCSERIALDRIVER_H
