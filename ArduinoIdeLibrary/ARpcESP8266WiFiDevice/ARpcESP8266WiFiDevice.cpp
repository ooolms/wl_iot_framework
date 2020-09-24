#include "ARpcESP8266WiFiDevice.h"

const int ARpcESP8266WiFiDevice::port=4081;
static const unsigned long waitTimerDelta=8000;//msec

ARpcESP8266WiFiDevice::ARpcESP8266WiFiDevice(unsigned long devBufSize,unsigned long bcastBufSize,
	const ARpcUuid *deviceId,const char *deviceName)
	:ARpcDevice(devBufSize,&cb,deviceId,deviceName)
	,cb(this)
	,server(port)
	,srvReadyCb(this)
	,srvReadyParser(bcastBufSize,&srvReadyCb)
{
	zeroSsidKey();
	connecting=false;
	wasSync=true;
	waitWiFi=false;
	debugInfoFunc=0;
	eventsCb=0;
	prevSyncMSec=millis();
}

void ARpcESP8266WiFiDevice::zeroSsidKey()
{
	memset(ssid,0,ssidMaxLen+1);
	memset(key,0,keyMaxLen+1);
}

void ARpcESP8266WiFiDevice::setDebugOutputFunction(void (*f)(const char *))
{
	debugInfoFunc=f;
}

void ARpcESP8266WiFiDevice::setEventsCb(ARpcESP8266WiFiDevice::IWiFiEventsCb *cb)
{
	eventsCb=cb;
}

void ARpcESP8266WiFiDevice::setup()
{
	reconnectWiFi();
	server.begin();
	debug("Server started");
}

void ARpcESP8266WiFiDevice::loop()
{
	unsigned long m=millis();
	if((m-prevSyncMSec)>waitTimerDelta)
	{
		prevSyncMSec=m;
		onWaitSyncTimer();
	}
	checkWiFi();
	checkBCastCli();
	checkWiFiClient();
}

void ARpcESP8266WiFiDevice::reconnectWiFi()
{
	WiFi.disconnect();
	if(eventsCb)
		eventsCb->onWiFiDisconnected();
	if(strlen(ssid)==0)return;
	WiFi.begin((const char*)ssid,(const char*)key);
	waitWiFi=true;
}

void ARpcESP8266WiFiDevice::onSyncMsg()
{
	wasSync=true;
}

bool ARpcESP8266WiFiDevice::isConnected()
{
	return client.connected();
}

void ARpcESP8266WiFiDevice::checkWiFi()
{
	if(WiFi.status()==WL_CONNECT_FAILED||
		WiFi.status()==WL_CONNECTION_LOST)
	{
		bCastCli.stop();
		if(eventsCb)
			eventsCb->onWiFiDisconnected();
		reconnectWiFi();
	}
	else if(waitWiFi&&WiFi.status()==WL_CONNECTED)
	{
		waitWiFi=false;
		bCastCli.begin(port);
		debug("WiFi connected");
		debug(WiFi.localIP().toString().c_str());
		if(eventsCb)
			eventsCb->onWiFiConnected();
	}
}

void ARpcESP8266WiFiDevice::checkBCastCli()
{
	static int sz=0;
	sz=bCastCli.parsePacket();
	if(sz>0)
	{
		bCastSenderIp=bCastCli.remoteIP();
		for(int i=0;i<sz;++i)
		{
			char c=bCastCli.read();
			srvReadyParser.putByte(c);
		}
	}
}

void ARpcESP8266WiFiDevice::checkWiFiClient()
{
	if(client.status()==CLOSED)
	{
		client=server.available();
		if(client.connected())
		{
			wasSync=true;
			debug("Take next pending incoming connection");
			if(eventsCb)
				eventsCb->onServerConnected();
		}
		delay(100);
	}
	else
	{
		while(client.available())
		{
			char c=client.read();
			putByte(c);
		}
	}
}

void ARpcESP8266WiFiDevice::onWaitSyncTimer()
{
	if(wasSync)
	wasSync=false;
	else if(client.connected())
	{
		debug("Connection timeout");
		client.stop();
		if(eventsCb)
			eventsCb->onSyncTimeout();
		checkWiFiClient();
	}
}

void ARpcESP8266WiFiDevice::debug(const char *str)
{
	if(debugInfoFunc)
		debugInfoFunc(str);
}

ARpcESP8266WiFiDevice::NetWriteCb::NetWriteCb(ARpcESP8266WiFiDevice *d)
{
	dev=d;
}

void ARpcESP8266WiFiDevice::NetWriteCb::writeData(const char *data,unsigned long sz)
{
	if(dev->client.connected())
		dev->client.write(data,sz);
}

void ARpcESP8266WiFiDevice::NetWriteCb::writeStr(const char *str)
{
	if(dev->client.connected())
		dev->client.print(str);
}

void ARpcESP8266WiFiDevice::NetWriteCb::writeStr(const __FlashStringHelper *str)
{
	if(dev->client.connected())
		dev->client.print(str);
}

ARpcESP8266WiFiDevice::SrvReadyCb::SrvReadyCb(ARpcESP8266WiFiDevice *d)
{
	dev=d;
}

void ARpcESP8266WiFiDevice::SrvReadyCb::processSrvReadyMsg(const ARpcUuid &srvId,const char *srvName)
{
	if(dev->client.status()!=CLOSED||dev->connecting)return;
	dev->debug((String("Server detected")+dev->bCastSenderIp.toString()+String(srvName)).c_str());
	if(dev->eventsCb&&!dev->eventsCb->onServerFound(srvId,srvName))
		return;
	dev->debug("Connecting to server...");
	dev->connecting=true;
	dev->client.connect(dev->bCastSenderIp,port);
	for(int i=0;i<20;++i)
	{
		if(dev->client.connected())
			break;
		delay(100);
	}
	dev->debug("Connected to server");
	dev->connecting=false;
	dev->wasSync=true;
	if(dev->eventsCb)
		dev->eventsCb->onServerConnected();
}

void ARpcESP8266WiFiDevice::IWiFiEventsCb::onWiFiConnected()
{
}

void ARpcESP8266WiFiDevice::IWiFiEventsCb::onWiFiDisconnected()
{
}

bool ARpcESP8266WiFiDevice::IWiFiEventsCb::onServerFound(const ARpcUuid &srvId, const char *srvName)
{
	return true;
}

void ARpcESP8266WiFiDevice::IWiFiEventsCb::onServerConnected()
{
}

void ARpcESP8266WiFiDevice::IWiFiEventsCb::onServerDisconnected()
{
}

void ARpcESP8266WiFiDevice::IWiFiEventsCb::onSyncTimeout()
{
}
