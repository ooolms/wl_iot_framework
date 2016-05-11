#ifndef ARPCSIMPLEMSGDISPATCH_H
#define ARPCSIMPLEMSGDISPATCH_H

#include "ARpcBase/ARpcDevice.h"
#include <QObject>

class ARpcSimpleMsgDispatch
	:public QObject
{
	Q_OBJECT
public:
	explicit ARpcSimpleMsgDispatch(const ARpcConfig &cfg,ARpcDevice *dev,QObject *parent=0);

signals:
	void infoMsg(const QString &str);
	void measurementMsg(const QString &sensor,const QString &value);

private slots:
	void onRawMsg(const ARpcMessage &m);

private:
	ARpcConfig config;
	ARpcDevice *device;
};

#endif // ARPCSIMPLEMSGDISPATCH_H
