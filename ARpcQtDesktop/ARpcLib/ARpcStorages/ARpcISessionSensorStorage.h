#ifndef ARPCISESSIONSENSORSTORAGE_H
#define ARPCISESSIONSENSORSTORAGE_H

#include <QObject>
#include <QByteArrayList>
#include <QUuid>
#include "ARpcStorages/ARpcISensorStorage.h"
#include "ARpcBase/ARpcSensorValue.h"

class ARpcISessionSensorStorage
{
public:
	virtual ~ARpcISessionSensorStorage(){}
	virtual bool listSessions(QList<QUuid> &ids,QByteArrayList &titles)=0;
	virtual bool createSession(const QByteArray &title,QUuid &sessionId)=0;
	virtual bool openSession(const QUuid &sessionId)=0;
	virtual bool closeSession(const QUuid &sessionId)=0;
	virtual bool removeSession(const QUuid &sessionId)=0;
	virtual bool setSessionAttribute(const QUuid &sessionId,const QByteArray &key,const QByteArray &val)=0;
	virtual bool removeSessionAttribute(const QUuid &sessionId,const QByteArray &key)=0;
	virtual bool getSessionAttribute(const QUuid &sessionId,const QByteArray &key,QByteArray &val)=0;
	virtual bool listSessionAttributes(const QUuid &sessionId, QMap<QByteArray,QByteArray> &map)=0;
	virtual quint64 valuesCount(const QUuid &sessionId)=0;
	virtual ARpcSensorValue* valueAt(const QUuid &sessionId,quint64 index)=0;
	virtual bool isSessionOpened(const QUuid &sessionId)const=0;
	virtual bool openMainWriteSession(const QUuid &sessionId)=0;
	virtual bool isMainWriteSessionOpened()const=0;
	virtual bool closeMainWriteSession()=0;
	virtual QUuid getMainWriteSessionId()const=0;
	virtual bool setMainReadSessionId(const QUuid &id)=0;
};

#endif // ARPCISESSIONSENSORSTORAGE_H
