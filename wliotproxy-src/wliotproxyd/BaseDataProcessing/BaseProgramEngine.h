#ifndef BASEPROGRAMENGINE_H
#define BASEPROGRAMENGINE_H

#include <QObject>
#include "../AccessManagement/AccessPolicyTypes.h"

class BaseProgramEngine
	:public QObject
{
	Q_OBJECT
public:
	explicit BaseProgramEngine(IdType uid,QObject *parent=nullptr);
	virtual void start()=0;
	virtual void stop()=0;
	virtual bool isRunning()=0;
	virtual bool setData(const QByteArray &data)=0;
	virtual QByteArray data()=0;
	void setProgramName(const QByteArray &n);
	QByteArray programName();

protected:
	virtual void onProgramNameChanged();
	IdType uid();

private:
	QByteArray mProgramName;
	IdType mUid;
};

#endif // BASEPROGRAMENGINE_H
