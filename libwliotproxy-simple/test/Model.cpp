#include "Model.h"
#include <string.h>
#include <stdio.h>

static float outdoorTemp=15;
static float outdoorHum=20;
static float globalMinTemp=0;
static float globalMaxTemp=50;
static float globalMinHum=0;
static float globalMaxHum=100;

Model::Model(Device *d)
{
	dev=d;
	heaterOn=condOn=humidOn=false;
	dev->disp().state()->prepareCommands(3);
	dev->disp().state()->prepareCommand(0,"heater",1);
	dev->disp().state()->setCommandParamState(0,0,"0");
	dev->disp().state()->prepareCommand(1,"humid",1);
	dev->disp().state()->setCommandParamState(1,0,"0");
	dev->disp().state()->prepareCommand(2,"cond",1);
	dev->disp().state()->setCommandParamState(2,0,"0");
	tempSens=outdoorTemp;
	humSens=outdoorHum;
}

void Model::updateTempHum()
{
	float tempDelta=0,humDelta=0;
	if(heaterOn)
	{
		tempDelta+=0.3f;
		humDelta-=0.6f;
	}
	if(humidOn)
		humDelta+=2;
	if(condOn)
	{
		tempDelta-=0.2f;
		humDelta-=1;
	}
	//outdoor
	{
		if(tempSens<outdoorTemp)
			tempDelta+=0.1f;
		else if(tempSens>outdoorTemp)
			tempDelta-=0.1f;
		if(humSens<outdoorHum)
			humDelta+=0.2f;
		else if(humSens>outdoorHum)
			humDelta-=0.2f;
	}
	tempSens+=tempDelta;
	humSens+=humDelta;
	if(tempSens<globalMinTemp)
		tempSens=globalMinTemp;
	else if(tempSens>globalMaxTemp)
		tempSens=globalMaxTemp;
	if(humSens<globalMinHum)
		humSens=globalMinHum;
	else if(humSens>globalMaxHum)
		humSens=globalMaxHum;
	//output
	printf("temp: %f\n",(double)tempSens);
	printf("hum: %f\n",(double)humSens);
	dev->disp().writeMeasurementB("temp",&tempSens,1);
	dev->disp().writeMeasurementB("hum",&humSens,1);
}

void Model::processCommand(const char *cmd,const char *args[],unsigned char argsCount)
{
	if(strcmp(cmd,"heater")==0&&argsCount>=1)//команда blink, проверяем что есть аргумент
	{
		heaterOn=strcmp(args[0],"1")==0;
		dev->disp().writeOk();
		dev->disp().writeMeasurement("heater",heaterOn?"1":"0");
	}
	else if(strcmp(cmd,"cond")==0&&argsCount>=1)//команда blink, проверяем что есть аргумент
	{
		condOn=strcmp(args[0],"1")==0;
		dev->disp().writeOk();
		dev->disp().writeMeasurement("cond",condOn?"1":"0");
	}
	else if(strcmp(cmd,"humid")==0&&argsCount>=1)//команда blink, проверяем что есть аргумент
	{
		humidOn=strcmp(args[0],"1")==0;
		dev->disp().writeOk();
		dev->disp().writeMeasurement("humid",humidOn?"1":"0");
	}
}

void Model::onSyncMsg()
{
	printf("sync msg\n");
}
