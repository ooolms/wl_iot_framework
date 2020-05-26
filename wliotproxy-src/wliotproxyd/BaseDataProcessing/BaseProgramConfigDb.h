#ifndef BASEPROGRAMCONFIGDB_H
#define BASEPROGRAMCONFIGDB_H

#include "BaseProgramEngine.h"

class QDomElement;

class BaseProgramConfigDb
{
public:
	explicit BaseProgramConfigDb(const QString &programPath,const QByteArray &programId);
	virtual ~BaseProgramConfigDb(){}
	virtual void setup(BaseProgramEngine *e)=0;
	virtual void cleanup(BaseProgramEngine *e,const QByteArray &oldData)=0;//called after program data changed
	void setProgramName(const QByteArray &name);
	QString programPath();
	QByteArray programName();
	QByteArray programId();
	void rmDb();
	bool isRunning();
	void setRunning(bool r);

protected:
	void storeDb();
	void load();//call from child constructor
	virtual void loadOther(QDomElement &rootElem)=0;
	virtual void storeOther(QDomElement &rootElem)=0;

private:
	QByteArray mProgramId;
	QByteArray mProgramName;
	QString mProgramPath;
	bool mIsRunning;
	bool mIsLoaded;
};

#endif // BASEPROGRAMCONFIGDB_H
