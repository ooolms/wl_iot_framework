#ifndef PROGRAMTHREAD_H
#define PROGRAMTHREAD_H

#include <QThread>
#include "GDIL/core/Program.h"
#include "GDIL/core/IEngineHelper.h"

class ProgramThread
	:public QThread
{
	Q_OBJECT
public:
	explicit ProgramThread(IEngineHelper *hlp,QObject *parent=nullptr);
	void setProgram(Program *p);

protected:
	virtual void run()override;

private:
	IEngineHelper *helper;
	Program *prg;
};

#endif // PROGRAMTHREAD_H
