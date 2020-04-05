#ifndef PROGRAMOBJECT_H
#define PROGRAMOBJECT_H

#include "GDIL/core/Program.h"
#include "GDIL/core/IEngineHelper.h"
#include "GDIL/core/ICommandCallback.h"
#include <QObject>

class ProgramObject
	:public QObject
	,public ICommandCallback
{
	Q_OBJECT
public:
	explicit ProgramObject(IEngineHelper *hlp,QObject *parent=0);
	virtual ~ProgramObject();
	void setProgram(Program *p);
	void prepare();
	virtual void commandCallback(const QUuid &devId,const QByteArray &cmd,const QByteArrayList &args)override;

signals:
	void execCommand(const QUuid &devId,const QByteArray &cmd,const QByteArrayList &args);

private slots:
	void activateProgram();

private:
	IEngineHelper *helper;
	Program *prg;
};

#endif // PROGRAMOBJECT_H
