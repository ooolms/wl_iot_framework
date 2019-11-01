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
	ssid[0]=key[0]=0;
	connecting=false;
	wasSync=true;
	waitWiFi=false;
	debugInfoFunc=0;
	prevSyncMSec=millis();
}

void ARpcESP8266WiFiDevice::setDebugOutputFunction(void (*f)(const char *))
{
	debugInfoFunc=f;
}

void ARpcESP8266WiFiDevice::setup()
{
	reconnectWiFi();

	// Start the server
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
	if(strlen(ssid)==0)return;
	WiFi.begin((const char*)ssid,(const char*)key);
	waitWiFi=true;
}

void ARpcESP8266WiFiDevice::onSyncMsg()
{
	wasSync=true;
}

void ARpcESP8266WiFiDevice::checkWiFi()
{
	if(WiFi.status()==WL_CONNECT_FAILED||WiFi.status()==WL_CONNECTION_LOST)
	{
		bCastCli.stop();
		WiFi.begin((const char*)ssid,(const char*)key);
	}
	else if(waitWiFi&&WiFi.status()==WL_CONNECTED)
	{
		waitWiFi=false;
		bCastCli.begin(port);
		debug("WiFi connected");
		debug(WiFi.localIP().toString().c_str());
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
	if(client)
	{
		if(!client.connected())
		{
			debug("Client connection lost");
			client=server.available();
			if(client)
			{
				wasSync=true;
				debug("Take next pending incoming connection");
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
	else
	{
		client=server.available();
		if(client)
		{
			wasSync=true;
			debug("Take next pending incoming connection");
		}
		delay(100);
	}
}

void ARpcESP8266WiFiDevice::onWaitSyncTimer()
{
	if(wasSync)
	wasSync=false;
	else if(client.connected())
	{
		debug("Connection lost");
		client.stop();
		checkWiFiClient();
	}
}

void ARpcESP8266WiFiDevice::debug(const char *str)
{
	if(debugInfoFunc)debugInfoFunc(str);
}

ARpcESP8266WiFiDevice::NetWriteCb::NetWriteCb(ARpcESP8266WiFiDevice *d)
{
	dev=d;
}

void ARpcESP8266WiFiDevice::NetWriteCb::writeData(const char *data,unsigned long sz)
{
	dev->client.write(data,sz);
}

void ARpcESP8266WiFiDevice::NetWriteCb::writeStr(const char *str)
{
	dev->client.print(str);
}

void ARpcESP8266WiFiDevice::NetWriteCb::writeStr(const __FlashStringHelper *str)
{
	dev->client.print(str);
}

ARpcESP8266WiFiDevice::SrvReadyCb::SrvReadyCb(ARpcESP8266WiFiDevice *d)
{
	dev=d;
}

void ARpcESP8266WiFiDevice::SrvReadyCb::processSrvReadyMsg(const ARpcUuid &srvId,const char *srvName)
{
	if(dev->client.connected()||dev->connecting)return;
	dev->debug((String("Server detected")+dev->bCastSenderIp.toString()+String(srvName)).c_str());
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
}
