#ifndef VDILPROGRAMCONFIGDB_H
#define VDILPROGRAMCONFIGDB_H

#include "../BaseDataProcessing/BaseProgramConfigDb.h"
#include "VDIL/core/Program.h"

class VDILProgramConfigDb
	:public BaseProgramConfigDb
{
public:
	explicit VDILProgramConfigDb(const QString &programPath,const QByteArray &programId);
	virtual void setup(BaseProgramEngine *e)override;
	virtual void cleanup(BaseProgramEngine *e, const QByteArray &oldData) override;
	void setConfigOption(const WLIOTVDIL::ConfigOptionId &id,const WLIOTVDIL::DataUnit &v);
	void setTimerConfig(quint32 blockId,const WLIOTVDIL::TimerBlock::TimerConfig &cfg);

protected:
	virtual void loadOther(QDomElement &rootElem)override;
	virtual void storeOther(QDomElement &rootElem)override;

private:
	QMap<WLIOTVDIL::ConfigOptionId,WLIOTVDIL::DataUnit> configOptions;
	QMap<quint32,WLIOTVDIL::TimerBlock::TimerConfig> timers;
};

#endif // VDILPROGRAMCONFIGDB_H
