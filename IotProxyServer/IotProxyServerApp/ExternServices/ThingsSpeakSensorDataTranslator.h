#ifndef THINGSSPEAKSENSORDATATRANSLATOR_H
#define THINGSSPEAKSENSORDATATRANSLATOR_H

#include <QObject>
#include "../ISensorDataTranslator.h"
#include <QNetworkAccessManager>

class ThingsSpeakSensorDataTranslator
	:public ISensorDataTranslator
{
	Q_OBJECT
public:
	explicit ThingsSpeakSensorDataTranslator(const QUuid &devId,const ARpcSensorDef &sens,
	const ARpcISensorStorage::DataExportConfig &cfg,QObject *parent=0);
	virtual void writeSensorValue(ARpcSensorValue *val) override;
	virtual bool checkConfig(const ARpcISensorStorage::DataExportConfig &cfg) override;
	virtual QByteArray type()const override;

public:
	static const QByteArray mType;

private:
	QNetworkAccessManager mgr;
};

#endif // THINGSSPEAKSENSORDATATRANSLATOR_H
