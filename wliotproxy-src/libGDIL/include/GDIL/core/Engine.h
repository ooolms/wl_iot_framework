#ifndef ENGINE_H
#define ENGINE_H

#include <QObject>
#include "GDIL/core/ProgramThread.h"

class Engine
	:public QObject
{
	Q_OBJECT
public:
	explicit Engine(IEngineHelper *hlp,QObject *parent=nullptr);
	virtual ~Engine();
	void setProgram(Program *p);
	void start();
	void stop();

private:
	IEngineHelper *helper;
	ProgramThread *trd;
	Program *prg;
};

#endif // ENGINE_H
