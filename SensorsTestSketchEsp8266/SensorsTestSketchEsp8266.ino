//подключаем библиотеку ARpc
#include <ARpc.h>
#include <ARpcSrvReady.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

int ledPin=13;//пин светодиода
unsigned int blinksCount=0;//число миганий
const char *deviceName="led_blink_test";//имя устройства
const char *deviceId="{f84526c1-5e88-4315-81f8-f7da45daa09d}";//идентификатор устройства

//Описание интерфейса управления
const char *interfaceStr="<controls><group title=\"Device controls\">"
"<control command=\"blink\" sync=\"0\" title=\"blink\">"//команда blink
"<param type=\"slider\" title=\"delay\"><constraints max=\"1000\" min=\"100\"/></param>"//параметр - время горения в мс
"</control>"
"<control command=\"get_blinks_count\" title=\"get_blinks_count\"/>"//команда get_blinks_count
"</group></controls>";

//Описание датчиков
const char *sensorsDef="<sensors>"
"<sensor name=\"blinks_count\" type=\"single\"/>"//датчик blinks_count
"<sensor name=\"sin_x\" type=\"single\"><constraints dims=\"2\"/></sensor>"//датчик sin_x (двумерный)
"</sensors>";

const char *essid="WL3";
const char *pass="11111111";
uint16_t port=4081;
IPAddress bCastSenderIp;
WiFiClient wifi;
WiFiUDP bCastCli;

//callback-функция для обработки команд, вызывается библиотекой ARpc
void processCommand(const char *cmd,const char *args[],int argsCount,ARpc *parser)
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
        parser->writeMeasurement("blinks_count",String(blinksCount).c_str());
        //сообщаем об успешном выполнении команды
        parser->writeOk();
    }
    else if(strcmp(cmd,"get_blinks_count")==0)//команда get_blinks_count
    {
        //выдаем измерение
        parser->writeMeasurement("blinks_count",String(blinksCount).c_str());
        //сообщаем об успешном выполнении команды
        parser->writeOk();
    }
    else parser->writeErr("Unknown cmd");//неизвестная команда
}

void ptpWriteCallback(const char *str)
{
    wifi.write(str);
}

void srvReadyWriteCallback(const char *str)
{
    //ignoring
}

void srvReadyCallback(char *args[],int argsCount,ARpcSrvReady *obj)
{
    Serial.print("Server detected: ");
    Serial.println(bCastSenderIp.toString());
    if(wifi.connected())return;
    Serial.println("Connecting...");
    wifi.connect(bCastSenderIp,port);
    //TODO timeout
}

ARpc parser(300,&processCommand,&ptpWriteCallback,deviceId,deviceName);
ARpcSrvReady srvReadyParser(40,&srvReadyWriteCallback,&srvReadyCallback);

//setup
void setup()
{
    Serial.begin(9600);
    WiFi.begin(essid,pass);
    while(WiFi.status()!=WL_CONNECTED)
    {
        delay(200);
        Serial.print(".");
    }
    Serial.print("WiFi connected: ");
    Serial.println(WiFi.localIP().toString());
    bCastCli.begin(port);
    pinMode(ledPin,OUTPUT);//настраиваем пин для мигания
    parser.setControlsInterface(interfaceStr);//указываем строку с описанием интерфейса управления
    parser.setSensorsDescription(sensorsDef);//указываем строку с описанием сенсоров
}

//мигание светодиодом
void blink(int dl)
{
    digitalWrite(ledPin,HIGH);
    delay(dl);
    digitalWrite(ledPin,LOW);
}

//генерация отсчетов sin и cos
int t=0;
void writeSinVal()
{
    //Генерируем строку со значениями, разделенными символом "|"
    String sinVal;
    sinVal+=String(sin(0.1*t));
    sinVal+="|";
    sinVal+=String(cos(0.1*t));
    //отправляем измерение
    parser.writeMeasurement("sin_x",sinVal.c_str());
    //увеличиваем "время"
    ++t;
}

char buf[301];
int av=0;
void loop()
{
    int sz=bCastCli.parsePacket();
    if(sz>0)
    {
        bCastSenderIp=bCastCli.remoteIP();
        for(int i=0;i<sz;++i)
            srvReadyParser.putChar(bCastCli.read());
    }
    while(wifi.available())
    {
        av=std::min(wifi.available(),300);
        wifi.readBytes(buf,av);
        buf[av]=0;
        Serial.println(buf);
        for(int i=0;i<av;++i)
            parser.putChar(buf[i]);
    }
    writeSinVal();//генерируем следующий отсчет sin и cos
    delay(500);//пауза пол-секунды
}

