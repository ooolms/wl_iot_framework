#ifndef ARPCSIMPLEMSGDISPATCH_H
#define ARPCSIMPLEMSGDISPATCH_H

#include "ARpcBase/ARpcDevice.h"
#include <QObject>

class ARpcSimpleMsgDispatch
	:public QObject
{
	Q_OBJECT
public:
	explicit ARpcSimpleMsgDispatch(ARpcDevice *dev,QObject *parent=0);

signals:
	void infoMsg(const QString &str);
	void measurementMsg(const QString &sensor,const QString &value);
	void commandStateChanged(const QString &command,int index,const QString &value);
	void additionalStateChanged(const QString &key,const QString &value);

private slots:
	void onRawMsg(const ARpcMessage &m);

private:
	ARpcDevice *device;
};

#endif // ARPCSIMPLEMSGDISPATCH_H
