#ifndef JSENGINE_H
#define JSENGINE_H

#include <QObject>
#include "JSThread.h"

class JSEngine
	:public BaseProgramEngine
{
	Q_OBJECT
public:
	explicit JSEngine(IdType uid,QObject *parent=nullptr);
	virtual ~JSEngine();
	virtual void start()override;
	virtual void stop()override;
	virtual bool isRunning()override;
	virtual bool setData(const QByteArray &data)override;
	virtual QByteArray data()override;

private:
	JSThread *thread;
};

#endif // JSENGINE_H
