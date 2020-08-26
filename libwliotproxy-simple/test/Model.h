#ifndef MODEL_H
#define MODEL_H

#include "wliot/simple/IDevEventsCallback.h"
#include "wliot/simple/Device.h"

class Model
	:public IDevEventsCallback
{
public:
	explicit Model(Device *d);
	void updateTempHum();

public:
    //callback-функция, вызываемая библиотекой, когда на устройство приходит пользовательская команда
    virtual void processCommand(const char *cmd,const char *args[],unsigned char argsCount)override;
	virtual void onSyncMsg()override;

private:
	Device *dev;
	bool heaterOn,condOn,humidOn;
	float tempSens,humSens;
};

#endif // MODEL_H
