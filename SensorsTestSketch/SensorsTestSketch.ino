//подключаем библиотеку ARpc
#include <ARpcDevice.h>

ARpcUuid deviceId("f84526c15e88431581f8f7da45daa09d");//идентификатор устройства
int ledPin=13;//пин светодиода
unsigned int blinksCount=0;//число миганий
const char *deviceName="led_blink_test";//имя устройства

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

void processCommand(void *data,const char *cmd,const char *args[],int argsCount);
void writeCallback(void *data,const char *str,unsigned long size);
//объект парсера ARpc, 300 - объем буфера для сообщения
ARpcDevice parser(300,&writeCallback,0,deviceId,deviceName);

//callback-функция для обработки команд, вызывается библиотекой ARpc
void processCommand(void *data,const char *cmd,const char *args[],int argsCount)
{
    if(strcmp(cmd,"blink")==0)//команда blink, проверяем что есть аргумент
    {
        int dl=200;
        if(argsCount>=1)
            dl=String(args[0]).toInt();//аргумент - время горения светодиода в мс
        //правим - от 100 до 1000 мс
        if(dl<100)dl=100;
        else if(dl>1000)dl=1000;
        //мигаем
        blink(dl);
        //инкрементируем число миганий
        ++blinksCount;
        //выдаем измерение
        parser.writeMeasurement("blinks_count",1,String(blinksCount).c_str());
        //сообщаем об успешном выполнении команды
        parser.writeOk();
    }
    else if(strcmp(cmd,"get_blinks_count")==0)//команда get_blinks_count
    {
        //выдаем измерение
        parser.writeMeasurement("blinks_count",1,String(blinksCount).c_str());
        //сообщаем об успешном выполнении команды
        parser.writeOk();
    }
    else parser.writeErr("Unknown cmd");//неизвестная команда
}

//callback-функция для отправки данных "на другой конец провода"
void writeCallback(void *data,const char *str,unsigned long size)
{
    Serial.write(str,size);//пишем в Serial
}


//setup
void setup()
{
    Serial.begin(9600);//запускаем Serial
    pinMode(ledPin,OUTPUT);//настраиваем пин для мигания
    parser.installCommandHandler(&processCommand,0);
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
    //отправляем измерение
    String s1(sin(0.1*t));
    String s2(cos(0.1*t));
    char *s[2]={s1.c_str(),s2.c_str()};
    parser.writeMeasurement("sin_x",2,s);
    //увеличиваем "время"
    ++t;
}

void loop()
{
    //проверяем, нет ли данных в Serial
    while(Serial.available())
        parser.putByte(Serial.read());//если данные есть, записываем побайтово в объект парсера
    writeSinVal();//генерируем следующий отсчет sin и cos
    delay(500);//пауза пол-секунды
}

