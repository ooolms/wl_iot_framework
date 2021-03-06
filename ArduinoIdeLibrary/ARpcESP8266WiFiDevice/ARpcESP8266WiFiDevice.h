#include <Arduino.h>
#include <ARpcDevice.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <WiFiUdp.h>
#include <ARpcSrvReady.h>

class ARpcESP8266WiFiDevice
	:public ARpcDevice
{
	class NetWriteCb
		:public ARpcIWriteCallback
	{
	public:
		explicit NetWriteCb(ARpcESP8266WiFiDevice *d);
		virtual void writeData(const char *data,unsigned long sz);
		virtual void writeStr(const char *str);
		virtual void writeStr(const __FlashStringHelper *str);

	private:
		ARpcESP8266WiFiDevice *dev;
	};

	class SrvReadyCb
		:public ARpcISrvReadyCallback
	{
	public:
		explicit SrvReadyCb(ARpcESP8266WiFiDevice *d);
		virtual void processSrvReadyMsg(const ARpcUuid &srvId,const char *srvName);

	private:
		ARpcESP8266WiFiDevice *dev;
	};

public:
	class IWiFiEventsCb
	{
	public:
		virtual void onWiFiConnected();
		virtual void onWiFiDisconnected();
		virtual bool onServerFound(const ARpcUuid &srvId,const char *srvName);
		virtual void onServerConnected();
		virtual void onServerDisconnected();
		virtual void onSyncTimeout();
	};

public:
	explicit ARpcESP8266WiFiDevice(unsigned long devBufSize,unsigned long bcastBufSize,
		const ARpcUuid *deviceId,const char *deviceName);
	void zeroSsidKey();
	void setDebugOutputFunction(void (*f)(const char*));
	void setEventsCb(IWiFiEventsCb *cb);
	void setup();
	void loop();
	void reconnectWiFi();//call when essid and key changed
	void onSyncMsg();//call when sync msg appeared
	bool isConnected();

private:
	void checkWiFi();
	void checkBCastCli();
	void checkWiFiClient();
	void onWaitSyncTimer();
	void debug(const char *str);

public:
	char ssid[33];
	char key[65];
	static const int ssidMaxLen=32;
	static const int keyMaxLen=64;

private:
	static const int port;
	bool connecting;
	bool wasSync;
	bool waitWiFi;
	NetWriteCb cb;
	WiFiServer server;
	WiFiClient client;
	IPAddress bCastSenderIp;
	WiFiUDP bCastCli;
	unsigned long prevSyncMSec;
	SrvReadyCb srvReadyCb;
	ARpcSrvReady srvReadyParser;
	void (*debugInfoFunc)(const char *);
	IWiFiEventsCb *eventsCb;
};
