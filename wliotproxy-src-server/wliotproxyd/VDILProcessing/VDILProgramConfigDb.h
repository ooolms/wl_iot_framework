#ifndef VDILPROGRAMCONFIGDB_H
#define VDILPROGRAMCONFIGDB_H

#include <VDIL/core/Program.h>
#include "../Processing/BaseProgramConfigDb.h"

class VDILProgramConfigDb
	:public BaseProgramConfigDb
{
public:
	explicit VDILProgramConfigDb(const QString &programPath,const QByteArray &programId);
	void setConfigOption(const WLIOTVDIL::ConfigOptionId &id,const WLIOTVDIL::DataUnit &v);
	void setTimerConfig(quint32 blockId,const WLIOTVDIL::TimerBlock::TimerConfig &cfg);
	void cleanup(WLIOTVDIL::Program *p);
	const QMap<WLIOTVDIL::ConfigOptionId,WLIOTVDIL::DataUnit>& configOptions()const;
	const QMap<quint32,WLIOTVDIL::TimerBlock::TimerConfig>& timers()const;

protected:
	virtual void loadOther(QDomElement &rootElem)override;
	virtual void storeOther(QDomElement &rootElem)override;

private:
	QMap<WLIOTVDIL::ConfigOptionId,WLIOTVDIL::DataUnit> mConfigOptions;
	QMap<quint32,WLIOTVDIL::TimerBlock::TimerConfig> mTimers;
};

#endif // VDILPROGRAMCONFIGDB_H
