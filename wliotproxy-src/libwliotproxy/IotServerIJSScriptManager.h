#ifndef IOTSERVERIJSSCRIPTMANAGER_H
#define IOTSERVERIJSSCRIPTMANAGER_H

#include <QObject>
#include <QByteArray>

class IotServerIJSScriptManager
	:public QObject
{
	Q_OBJECT
public:
	explicit IotServerIJSScriptManager(QObject *parent=nullptr);
	virtual QByteArrayList scripts()=0;
	virtual bool get(const QByteArray &scriptName,QByteArray &text)=0;
	virtual bool isWorking(const QByteArray &scriptName)=0;
	virtual bool setText(const QByteArray &scriptName,const QByteArray &text)=0;
	virtual bool remove(const QByteArray &scriptName)=0;
	virtual void start(const QByteArray &scriptName)=0;
	virtual void stop(const QByteArray &scriptName)=0;
	virtual void restart(const QByteArray &scriptName)=0;

signals:
	void stateChanged(const QByteArray &scriptName,bool isWorking);
};

#endif // IOTSERVERIJSSCRIPTMANAGER_H
