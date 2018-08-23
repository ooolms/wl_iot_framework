//подключаем библиотеку ARpc
#include <ARpcDevice.h>

int ledPin=13;//пин светодиода
unsigned int blinksCount=0;//число миганий
const char *deviceName="led_blink_test";//имя устройства
const ARpcUuid deviceId("f84526c15e88431581f8f7da45daa09d");//идентификатор устройства

//Описание интерфейса управления
const char *interfaceStr="<controls><group title=\"Device controls\">"
"<control command=\"blink\" title=\"blink\">"//команда blink
"<param type=\"slider\" title=\"delay\"><attributes max=\"1000\" min=\"100\"/></param>"//параметр - время горения в мс
"</control>"
"<control command=\"get_blinks_count\" title=\"get_blinks_count\"/>"//команда get_blinks_count
"</group></controls>";

//Описание датчиков
const char *sensorsDef="<sensors>"
"<sensor name=\"blinks_count\" type=\"f32_sv\"/>"//датчик blinks_count
"<sensor name=\"sin_x\" type=\"f32_sv_d2\"/>"//датчик sin_x (двумерный)
"</sensors>";

class WriteCallback
    :public ARpcIWriteCallback
{
public:
    virtual void writeData(const char *data,unsigned long sz)
    {
        Serial.write(data,sz);
    }
    virtual void writeStr(const char *str)
    {
        Serial.print(str);
    }
}wcb;

//объект парсера ARpc, 300 - объем буфера для сообщения
ARpcDevice parser(300,&wcb,&deviceId,deviceName);

class CommandCallback
    :public ARpcIDevEventsCallback
{
public:
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
            parser.disp().writeMeasurement("blinks_count",String(blinksCount).c_str());
            //сообщаем об успешном выполнении команды
            parser.disp().writeOk();
        }
        else if(strcmp(cmd,"get_blinks_count")==0)//команда get_blinks_count
        {
            //выдаем измерение
            parser.disp().writeMeasurement("blinks_count",String(blinksCount).c_str());
            //сообщаем об успешном выполнении команды
            parser.disp().writeOk();
        }
        else parser.disp().writeErr("Unknown cmd",cmd);//неизвестная команда
    }
}ccb;

//setup
void setup()
{
    Serial.begin(9600);//запускаем Serial
    pinMode(ledPin,OUTPUT);//настраиваем пин для мигания
    parser.disp().installDevEventsHandler(&ccb);
    parser.disp().setControls(interfaceStr);//указываем строку с описанием интерфейса управления
    parser.disp().setSensors(sensorsDef);//указываем строку с описанием сенсоров
}

//мигание светодиодом

//генерация отсчетов sin и cos
int t=0;
float sinCos[2];
void writeSinVal()
{
    //Генерируем строки со значениями
    sinCos[0]=sin(0.1*t);
    sinCos[1]=cos(0.1*t);
    //отправляем измерение
    parser.disp().writeMeasurementB("sin_x",sinCos,2);
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

