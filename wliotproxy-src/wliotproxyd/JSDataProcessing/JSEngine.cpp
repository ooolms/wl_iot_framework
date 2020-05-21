#include "JSEngine.h"

JSEngine::JSEngine(IdType uid,QObject *parent)
	:BaseProgramEngine(parent)
{
	thread=new JSThread(uid,this);
}

JSEngine::~JSEngine()
{
	stop();
}

void JSEngine::start()
{
	if(thread->isRunning())
		return;
	thread->setFileName(QString::fromUtf8(programName()));
	thread->setup();
}

void JSEngine::stop()
{
	thread->quit();
	thread->wait(3000);
	thread->terminate();
	thread->wait(200);
	thread->cleanupAfterTerminated();
}

bool JSEngine::isRunning()
{
	return thread->isRunning();
}

bool JSEngine::setData(const QByteArray &data)
{
	if(thread->isRunning())
		return false;
	thread->setScriptText(QString::fromUtf8(data));
	return true;
}

QByteArray JSEngine::data()
{
	return thread->scriptText().toUtf8();
}
