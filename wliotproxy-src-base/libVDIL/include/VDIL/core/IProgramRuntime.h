#ifndef IPROGRAMRUNTIME_H
#define IPROGRAMRUNTIME_H

#include <QObject>

namespace WLIOTVDIL
{
	class Program;

	class IProgramRuntimeInstance
		:public QObject
	{
		Q_OBJECT
	public:
		explicit IProgramRuntimeInstance(Program *p);
		virtual ~IProgramRuntimeInstance(){}
		virtual void prepareToStart()=0;
		virtual void cleanupAfterStop()=0;

	signals:
		void activateProgram();

	protected:
		Program *prg;
	};

	class IProgramRuntime
	{
	public:
		virtual ~IProgramRuntime(){}
		virtual IProgramRuntimeInstance* mkRuntime(Program *p)=0;
	};
}

#endif // IPROGRAMRUNTIME_H
