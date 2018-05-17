#ifndef ARPCSERIALNOTIFICATOR_H
#define ARPCSERIALNOTIFICATOR_H

#include <QObject>
#include <QFileSystemWatcher>

class ARpcSerialNotificator
	:public QObject
{
	Q_OBJECT
private:
	explicit ARpcSerialNotificator();
	virtual ~ARpcSerialNotificator();
	ARpcSerialNotificator(const ARpcSerialNotificator&);
	ARpcSerialNotificator& operator=(const ARpcSerialNotificator&);

public:
	static ARpcSerialNotificator& inst();

signals:
	void checkSerialPorts();

private:
	QFileSystemWatcher w;
};

#endif // ARPCSERIALNOTIFICATOR_H
