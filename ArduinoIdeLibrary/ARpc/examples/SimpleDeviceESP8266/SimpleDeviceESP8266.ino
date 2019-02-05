#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <WiFiUdp.h>
#include <ARpcDevice.h>
#include <ARpcSrvReady.h>

uint16_t port=4081;
WiFiServer server(port);
WiFiClient client;
IPAddress bCastSenderIp;
WiFiUDP bCastCli;

bool connecting=false;
int ledPin=13;//пин светодиода
unsigned int blinksCount=0;//число миганий
const char* ssid="wifi_name";
const char* password="wifi_key";

const char *deviceName="led_blink_test";//имя устройства
const ARpcUuid deviceId("f84526c15e88431581f8f7da45daa09d");//идентификатор устройства
unsigned long lastSyncMillis=0;

//Описание интерфейса управления
const char *interfaceStr="<controls><group title=\"Device controls\">"
"<control command=\"blink\" sync=\"0\" title=\"blink\" force_button=\"1\">"//команда blink
"<param type=\"slider\" title=\"delay\"><attributes max=\"1000\" min=\"100\"/></param>"//параметр - время горения в мс
"</control>"
"<control command=\"get_blinks_count\" title=\"get_blinks_count\"/>"//команда get_blinks_count
"</group></controls>";

//Описание датчиков
const char *sensorsDef="<sensors>"
"<sensor name=\"blinks_count\" type=\"f32_sv\"/>"//датчик blinks_count
"</sensors>";

class NetWriteCb
    :public ARpcIWriteCallback
{
public:
    void writeData(const char *d,unsigned long sz)
    {
        client.write(d,sz);
    }
    void writeStr(const char *str)
    {
        client.print(str);
    }
}netCb;

class SerialWriteCb
    :public ARpcIWriteCallback
{
public:
    void writeData(const char *d,unsigned long sz)
    {
        Serial.write(d,sz);
    }
    void writeStr(const char *str)
    {
        Serial.print(str);
    }
}serialCb;

ARpcDevice wifiDev(300,&netCb,&deviceId,deviceName);
ARpcDevice serialDev(300,&serialCb,&deviceId,deviceName);

//callback-класс для обработки команд, вызывается библиотекой ARpc
class EventsCb
    :public ARpcIDevEventsCallback
{
public:
    explicit EventsCb(ARpcDevice *d)
    {
        dev=d;
    }
    
    //мигание светодиодом
    void blink(int dl)
    {
        digitalWrite(ledPin,HIGH);
        delay(dl);
        digitalWrite(ledPin,LOW);
    }

    virtual void processCommand(const char *cmd,const char *args[],unsigned char argsCount)
    {
        if(strcmp(cmd,"blink")==0&&argsCount>=1)//команда blink, проверяем что есть аргумент
        {
            int dl=String(args[0]).toInt();//аргумент - время горения светодиода в мс
            //правим - от 100 до 1000 мс
            if(dl<100)dl=100;
            else if(dl>1000)dl=1000;
            //мигаем
            blink(dl);
            //инкрементируем число миганий
            ++blinksCount;
            //выдаем измерение
            dev->disp().writeMeasurement("blinks_count",String(blinksCount).c_str());
            //сообщаем об успешном выполнении команды
            dev->disp().writeOk();
        }
        else if(strcmp(cmd,"get_blinks_count")==0)//команда get_blinks_count
        {
            //выдаем измерение
            dev->disp().writeMeasurement("blinks_count",String(blinksCount).c_str());
            //сообщаем об успешном выполнении команды
            dev->disp().writeOk();
        }
        else dev->disp().writeErr("Unknown cmd");//неизвестная команда
    }

    virtual void onSyncMsg()
    {
        lastSyncMillis=millis();
    }

private:
    ARpcDevice *dev;
}serialEcb(&serialDev),wifiEcb(&wifiDev);

class SrcReadyCb
    :public ARpcISrvReadyCallback
{
public:
    void processSrvReadyMsg(const ARpcUuid &srvId,const char *srvName)
    {
        serialDev.disp().writeInfo("Server detected: ",bCastSenderIp.toString().c_str(),srvName);
        if(client.connected()||connecting)return;
        serialDev.disp().writeInfo("Connecting to server...");
        connecting=true;
        client.connect(bCastSenderIp,port);
        for(int i=0;i<20;++i)
        {
            if(client.connected())
                break;
            delay(100);
        }
        serialDev.disp().writeInfo("Connected to server");
        connecting=false;
        wifiDev.resetParser();
        lastSyncMillis=millis();
    }
}srvReadyCb;

ARpcSrvReady srvReadyParser(200,&srvReadyCb);

void connectWifi()
{
    WiFi.begin(ssid,password);
    while(WiFi.status()!=WL_CONNECTED)
    {
        delay(500);
        serialDev.disp().writeInfo("Wifi connecting");
    }
    serialDev.disp().writeInfo("WiFi connected");
}

void setup()
{
    Serial.begin(9600);
    wifiDev.disp().installDevEventsHandler(&wifiEcb);
    wifiDev.disp().setControls(interfaceStr);
    wifiDev.disp().setSensors(sensorsDef);
    serialDev.disp().installDevEventsHandler(&serialEcb);
    serialDev.disp().setControls(interfaceStr);
    serialDev.disp().setSensors(sensorsDef);

    connectWifi();

    bCastCli.begin(port);
    
    // Start the server
    server.begin();
    serialDev.disp().writeInfo("Server started");
    
    // Print the IP address
    serialDev.disp().writeInfo(WiFi.localIP().toString().c_str());
    serialDev.resetStream();
}

void checkBCastCli()
{
    static int sz=0;
    sz=bCastCli.parsePacket();
    if(sz>0)
    {
        bCastSenderIp=bCastCli.remoteIP();
        for(int i=0;i<sz;++i)
            srvReadyParser.putByte(bCastCli.read());
    }
}

void checkWifiClient()
{
    if(client)
    {
        if(!client.connected())
        {
            serialDev.disp().writeInfo("Client connection lost");
            client=server.available();
            if(client)
            {
                serialDev.disp().writeInfo("Take next pending incoming connection");
                lastSyncMillis=millis();
                wifiDev.resetStream();
                wifiDev.resetParser();
            }
            delay(100);
        }
        else
        {
            while(client.available())
                wifiDev.putByte(client.read());
        }
    }
    else
    {
        client=server.available();
        if(client)
        {
            serialDev.disp().writeInfo("Take next pending incoming connection");
            lastSyncMillis=millis();
            wifiDev.resetStream();
            wifiDev.resetParser();
        }
        delay(100);
    }
}

void loop()
{
    while(Serial.available())
        serialDev.putByte(Serial.read());
    if(WiFi.status()!=WL_CONNECTED)
        connectWifi();
    checkBCastCli();
    checkWifiClient();
    if((millis()-lastSyncMillis)>12000)&&client.connected())
    {
        client.stop();
        checkWifiClient();
    }
    delay(1);
}
