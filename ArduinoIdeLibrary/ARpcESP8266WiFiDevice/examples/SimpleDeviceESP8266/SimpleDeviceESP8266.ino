#include <ARpcESP8266WiFiDevice.h>

int ledPin=13;//пин светодиода
unsigned int blinksCount=0;//число миганий
const char* ssid="wifi_name";
const char* key="wifi_key";

const char *deviceName="led_blink_test";//имя устройства
const ARpcUuid deviceId("f84526c15e88431581f8f7da45daa09d");//идентификатор устройства

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

class SerialWriteCb
    :public ARpcIWriteCallback
{
public:
    void writeData(const char *d,unsigned long sz)override
    {
        Serial.write(d,sz);
    }
    void writeStr(const char *str)override
    {
        Serial.print(str);
    }
    void writeStr(const __FlashStringHelper *str)override
    {
        Serial.print(str);
    }
}serialCb;

ARpcESP8266WiFiDevice wifiDev(300,200,&deviceId,deviceName);
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

    virtual void processCommand(const char *cmd,const char *args[],unsigned char argsCount)override
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
            dev->disp().writeOk(String(dl).c_str());
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
        if(dev==&wifiDev)
            wifiDev.onSyncMsg();
    }

private:
    ARpcDevice *dev;
}serialEcb(&serialDev),wifiEcb(&wifiDev);

void wifiDbgFunc(const char *str)
{
    serialDev.disp().writeInfo(str);
}

void setup()
{
    Serial.begin(9600);
    wifiDev.setDebugOutputFunction(&wifiDbgFunc);
    wifiDev.disp().installDevEventsHandler(&wifiEcb);
    wifiDev.disp().setControls(interfaceStr);
    wifiDev.disp().setSensors(sensorsDef);
    serialDev.disp().installDevEventsHandler(&serialEcb);
    serialDev.disp().setControls(interfaceStr);
    serialDev.disp().setSensors(sensorsDef);
    strcpy(wifiDev.ssid,ssid);
    strcpy(wifiDev.key,key);

    wifiDev.setup();
    serialDev.resetStream();
}

void loop()
{
    while(Serial.available())
        serialDev.putByte(Serial.read());
    wifiDev.loop();
    delay(1);
}
