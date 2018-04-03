//подключаем библиотеку ARpc
#include <ARpcDevice.h>

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


//объект парсера ARpc, 300 - объем буфера для сообщения
void processCommand(void *data,const char *cmd,const char *args[],int argsCount);
void arpcWriteCallback(void *data,const char *str,unsigned long sz);
ARpcDevice parser(300,&processCommand,&arpcWriteCallback,deviceId,deviceName);

//callback-функция для обработки команд, вызывается библиотекой ARpc
void processCommand(void *data,const char *cmd,const char *args[],int argsCount)
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
        parser.writeMeasurement("blinks_count",String(blinksCount).c_str());
        //сообщаем об успешном выполнении команды
        parser.writeOk();
    }
    else if(strcmp(cmd,"get_blinks_count")==0)//команда get_blinks_count
    {
        //выдаем измерение
        parser.writeMeasurement("blinks_count",String(blinksCount).c_str());
        //сообщаем об успешном выполнении команды
        parser.writeOk();
    }
    else parser.writeErr("Unknown cmd");//неизвестная команда
}

//callback-функция для отправки данных "на другой конец провода"
void arpcWriteCallback(void *data,const char *str,unsigned long sz)
{
    Serial.write(str,sz);//пишем в Serial
}

//setup
void setup()
{
    Serial.begin(9600);//запускаем Serial
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

void loop()
{
    //проверяем, нет ли данных в Serial
    while(Serial.available())
        parser.putByte(Serial.read());//если данные есть, записываем побайтово в объект парсера
    writeSinVal();//генерируем следующий отсчет sin и cos
    delay(500);//пауза пол-секунды
}

