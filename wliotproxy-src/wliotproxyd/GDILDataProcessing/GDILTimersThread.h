#ifndef GDILTIMERSTHREAD_H
#define GDILTIMERSTHREAD_H

#include <QThread>
#include <QVector>
#include "GDIL/core/Program.h"
#include "GDILProgramThread.h"

class GDILTimersThread
	:public QThread
{
	Q_OBJECT
public:
	GDILTimersThread(Program *p,GDILProgramThread *thr,QObject *parent=0);
	~GDILTimersThread();

protected:
	virtual void run()override;

private:
	Program *prg;
	GDILProgramThread *thrd;
	QVector<int> timerFds;
	QMap<int,TimerBlock*> timersMap;
};

#endif // GDILTIMERSTHREAD_H
