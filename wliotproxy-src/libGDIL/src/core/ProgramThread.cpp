#include "GDIL/core/ProgramThread.h"
#include "GDIL/core/ProgramObject.h"

ProgramThread::ProgramThread(IEngineHelper *hlp,QObject *parent)
	:QThread(parent)
{
	helper=hlp;
	prg=0;
}

void ProgramThread::setProgram(Program *p)
{
	prg=p;
}

void ProgramThread::run()
{
	ProgramObject *obj=new ProgramObject(helper);
	obj->prepare();
	QThread::exec();
	delete obj;
}
