#ifndef VDILTIMERSTHREAD_H
#define VDILTIMERSTHREAD_H

#include <QThread>
#include <QVector>
#include "VDIL/core/Program.h"
#include "VDILProgramThread.h"

class VDILTimersThread
	:public QThread
{
	Q_OBJECT
public:
	VDILTimersThread(WLIOTVDIL::Program *p,VDILProgramThread *thr,QObject *parent=0);
	~VDILTimersThread();

protected:
	virtual void run()override;

private:
	WLIOTVDIL::Program *prg;
	VDILProgramThread *thrd;
	QVector<int> timerFds;
	QMap<int,WLIOTVDIL::TimerBlock*> timersMap;
};

#endif // VDILTIMERSTHREAD_H
