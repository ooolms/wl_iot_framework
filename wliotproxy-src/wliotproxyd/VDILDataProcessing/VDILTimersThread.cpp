#include "VDILTimersThread.h"
#include "VDILProgramThread.h"
#include <sys/timerfd.h>
#include <poll.h>
#include <unistd.h>

using namespace WLIOTVDIL;

VDILTimersThread::VDILTimersThread(Program *p,VDILProgramThread *thr,QObject *parent)
	:QThread(parent)
{
	prg=p;
	thrd=thr;
	QDateTime curr=QDateTime::currentDateTime();
	timerFds.reserve(prg->timerBlocks().count());
	for(TimerBlock *b:prg->timerBlocks())
	{
		int fd=timerfd_create(CLOCK_REALTIME,0);
		QDateTime next=b->nextTimeout(curr);
		if(!next.isNull())
		{
			itimerspec s;
			s.it_value.tv_nsec=0;
			s.it_interval.tv_nsec=0;
			s.it_value.tv_sec=next.toSecsSinceEpoch();
			s.it_interval.tv_sec=b->repeatInSec();
			timerfd_settime(fd,TFD_TIMER_ABSTIME,&s,0);
		}
		timerFds.append(fd);
		timersMap[fd]=b;
	}
	if(!timerFds.isEmpty())
		start();
}

VDILTimersThread::~VDILTimersThread()
{
	for(int i=0;i<timerFds.size();++i)
		close(timerFds[i]);
}

void VDILTimersThread::run()
{
	int fd;
	pollfd *fds=new pollfd[timerFds.size()];
	for(int i=0;i<timerFds.size();++i)
	{
		fds[i].fd=timerFds[i];
		fds[i].events=POLLIN;
	}
	while(!isInterruptionRequested())
	{
		if(poll(fds,(nfds_t)timerFds.size(),100)<=0)
			continue;
		QDateTime curr=QDateTime::currentDateTime();
		for(int i=0;i<timerFds.size();++i)
		{
			if(!(fds[i].revents&POLLIN))
				continue;
			fd=timerFds[i];
			quint64 bb;
			read(fd,&bb,8);
			TimerBlock *b=timersMap.value(fd);
			if(!b)
				continue;
			b->setTimerTriggered();
			if(b->repeatInSec()==0)
			{
				QDateTime next=b->nextTimeout(curr);
				if(!next.isNull())
				{
					itimerspec s;
					s.it_value.tv_nsec=0;
					s.it_interval.tv_nsec=0;
					s.it_value.tv_sec=next.toSecsSinceEpoch();
					s.it_interval.tv_sec=b->repeatInSec();
					timerfd_settime(fd,TFD_TIMER_ABSTIME,&s,0);
				}
			}
			QMetaObject::invokeMethod(thrd,"activateProgram",Qt::QueuedConnection);
		}
	}
	delete[] fds;
}
