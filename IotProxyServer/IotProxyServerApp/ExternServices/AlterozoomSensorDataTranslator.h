#ifndef ALTEROZOOMSENSORDATATRANSLATOR_H
#define ALTEROZOOMSENSORDATATRANSLATOR_H

#include <QObject>
#include "../ISensorDataTranslator.h"
#include "AlterozoomApi.h"

class AlterozoomSensorDataTranslator
	:public ISensorDataTranslator
{
	Q_OBJECT
public:
	explicit AlterozoomSensorDataTranslator(
		const QUuid &devId,const ARpcSensorDef &sens,
		const ARpcISensorStorage::DataExportConfig &cfg,QObject *parent=nullptr);

public:
	virtual void writeSensorValue(ARpcSensorValue *val)override;
	virtual bool checkConfig(const ARpcISensorStorage::DataExportConfig &cfg)override;
	virtual QByteArray type()const override;

public:
	static const QByteArray mType;

private:
	bool ready;
	AlterozoomApi api;
	QByteArray host,email;
};

#endif // ALTEROZOOMSENSORDATATRANSLATOR_H
