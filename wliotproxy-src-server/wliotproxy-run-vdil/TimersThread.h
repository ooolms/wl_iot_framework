#ifndef TIMERSTHREAD_H
#define TIMERSTHREAD_H

#include <QThread>
#include <QVector>
#include "VDIL/core/Program.h"

class TimersThread
	:public QThread
{
	Q_OBJECT
public:
	TimersThread(WLIOTVDIL::Program *p,QObject *parent=0);
	~TimersThread();

protected:
	virtual void run()override;

signals:
	void activate();

private:
	WLIOTVDIL::Program *prg;
	QVector<int> timerFds;
	QMap<int,WLIOTVDIL::TimerBlock*> timersMap;
};

#endif // TIMERSTHREAD_H
