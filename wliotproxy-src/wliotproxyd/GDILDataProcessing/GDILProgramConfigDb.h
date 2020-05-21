#ifndef GDILPROGRAMCONFIGDB_H
#define GDILPROGRAMCONFIGDB_H

#include "../BaseDataProcessing/BaseProgramConfigDb.h"
#include "GDIL/core/Program.h"

class GDILProgramConfigDb
	:public BaseProgramConfigDb
{
public:
	explicit GDILProgramConfigDb(const QString &programPath);
	virtual void setup(BaseProgramEngine *e)override;
	virtual void cleanup(BaseProgramEngine *e, const QByteArray &oldData) override;
	void setConfigOption(const ConfigOptionId &id,const DataUnit &v);
	void setTimerConfig(quint32 blockId,const TimerBlock::TimerConfig &cfg);

protected:
	virtual void loadOther(QDomElement &rootElem)override;
	virtual void storeOther(QDomElement &rootElem)override;

private:
	QMap<ConfigOptionId,DataUnit> configOptions;
	QMap<quint32,TimerBlock::TimerConfig> timers;
};

#endif // GDILPROGRAMCONFIGDB_H
