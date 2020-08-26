#include <wliot/simple/Device.h>
#include <wliot/simple/SrvReady.h>
#include <sys/timerfd.h>
#include <unistd.h>
#include <stdio.h>
#include "Model.h"

const char *deviceName="fake_room_climatics";//имя устройства
const Uuid deviceId("5a059a0216934e71bf753e4afe26e6f6");//идентификатор устройства

const char *interfaceStr=
"<controls>"
"<group title=\"Device controls\">"
"<control command=\"heater\" title=\"heater\">"
"<param type=\"checkbox\" title=\"heater\"/>"
"</control>"
"<control command=\"cond\" title=\"conditioner\">"
"<param type=\"checkbox\" title=\"conditioner\"/>"
"</control>"
"<control command=\"humid\" title=\"humidifier\">"
"<param type=\"checkbox\" title=\"humidifier\"/>"
"</control>"
"</group></controls>";

const char *sensorsDef="<sensors>"
"<sensor name=\"temp\" type=\"f32_sv\"/>"
"<sensor name=\"hum\" type=\"f32_sv\"/>"
"<sensor name=\"heater\" type=\"u8_sv\"/>"
"<sensor name=\"humid\" type=\"u8_sv\"/>"
"<sensor name=\"cond\" type=\"u8_sv\"/>"
"</sensors>";

class SrvReadyCb
	:public ISrvReadyCallback
{
public:
	explicit SrvReadyCb(Device *d)
	{
		dev=d;
	}

	virtual void processSrvReadyMsg(const Uuid &serverId,const char *serverName,in_addr addr)override
	{
		if(!dev->sock().isConnected()&&dev->sock().connectToAddr(addr))
		{
			char idStr[Uuid::idStringLen];
			serverId.toString(idStr);
			printf("connected to server: %i.%i.%i.%i; %s:%s\n",(addr.s_addr%(1<<8)),(addr.s_addr%(1<<16))/(1<<8),
				(addr.s_addr%(1<<24))/(1<<16),addr.s_addr/(1<<24),idStr,serverName);
		}
	}

private:
	Device *dev;
};

class ConnCb
	:public IConnectionCallback
{
public:
	virtual void onConnected()override
	{
		printf("connected\n");
	}

	virtual void onDisconnected()override
	{
		printf("disconnected\n");
	}

	virtual void onSyncTimeout()override
	{
		printf("sync timeout\n");
	}
};

int main(int argc,char **argv)
{
	Device dev(300,&deviceId,deviceName);
	ConnCb ccb;
	SrvReadyCb srvRdyCb(&dev);
	SrvReady rdy(120,&srvRdyCb);
	Model mdl(&dev);
	int counter=0;
	dev.setConnectionCallback(&ccb);
	dev.disp().setControls(interfaceStr);
	dev.disp().setSensors(sensorsDef);
	dev.setDevEventsCallback(&mdl);
	int tmrFd=timerfd_create(CLOCK_REALTIME,0);
	if(tmrFd==-1)return 1;
	itimerspec s;
	s.it_value.tv_sec=1;
	s.it_value.tv_nsec=0;
	s.it_interval.tv_sec=1;
	s.it_interval.tv_nsec=0;
	timerfd_settime(tmrFd,0,&s,0);
	uint64_t r;
	while(read(tmrFd,&r,sizeof(r))>0)
	{
		rdy.checkForMessages();
		dev.sock().checkForNewData();
		dev.checkTimer();
		++counter;
		if(counter==5)
		{
			counter=0;
			mdl.updateTempHum();
		}
	}
}
