#ifndef GDILPROGRAMCONFIGDB_H
#define GDILPROGRAMCONFIGDB_H

#include "GDIL/core/Program.h"

class GDILProgramConfigDb
{
public:
	explicit GDILProgramConfigDb(const QString &programPath);
	void load(Program *p);
	void cleanup(Program *p);
	void setConfigOption(const ConfigOptionId &id,const DataUnit &v);
	void setTimerConfig(quint32 blockId,const TimerBlock::TimerConfig &cfg);
	QString dbPath();

private:
	void storeDb();

private:
	QMap<ConfigOptionId,DataUnit> configOptions;
	QMap<quint32,TimerBlock::TimerConfig> timers;
	QString mDbPath;
};

#endif // GDILPROGRAMCONFIGDB_H
