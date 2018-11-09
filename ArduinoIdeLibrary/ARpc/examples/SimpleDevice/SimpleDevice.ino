//подключаем библиотеку ARpc
#include <ARpcDevice.h>

int ledPin=13;//пин светодиода
unsigned int blinksCount=0;//число миганий
const char *deviceName="led_blink_test";//имя устройства
const ARpcUuid deviceId("f84526c15e88431581f8f7da45daa09d");//идентификатор устройства

//Описание интерфейса управления
const char *interfaceStr=
"<controls>"
//начало основной группы команд
"<group title=\"Device controls\">"
//команда мигания светодиодом blink
"<control command=\"blink\" title=\"blink\">"
//параметр команды #0 - время горения светодиода
"<param type=\"slider\" title=\"delay\"><attributes max=\"1000\" min=\"100\"/></param>"
"</control>"
//команда get_blinks_count - заставляет устройство сгенерировать измерение,
//содержащее количество миганий светодиодом с момента запуска устройства
"<control command=\"get_blinks_count\" title=\"get_blinks_count\"/>"
"</group></controls>";

//Описание датчиков
const char *sensorsDef="<sensors>"
//первый датчик - blinks_count, содержит количество миганий светодиодом с момента запуска устройства
"<sensor name=\"blinks_count\" type=\"f32_sv\"/>"
//второй датчик - sin_x, содержит двумерное значение со значениями sin и cos, меняющимися со временем
"<sensor name=\"sin_x\" type=\"f32_sv_d2\"/>"
"</sensors>";

class WriteCallback
    :public ARpcIWriteCallback
{
public:
    //callback-функции, вызываемые библиотекой, когда нужно передать данные от устройства
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
ARpcDevice dev(300,&wcb,&deviceId,deviceName);

class CommandCallback
    :public ARpcIDevEventsCallback
{
public:
    //мигание светодиодом заданное время
    void blink(int dl)
    {
        digitalWrite(ledPin,HIGH);
        delay(dl);
        digitalWrite(ledPin,LOW);
    }

    //callback-функция, вызываемая библиотекой, когда на устройство приходит пользовательская команда
    virtual void processCommand(const char *cmd,const char *args[],unsigned char argsCount)
    {
        if(strcmp(cmd,"blink")==0&&argsCount>=1)//команда blink, проверяем что есть аргумент
        {
            //аргумент - время горения светодиода в мс
            int dl=String(args[0]).toInt();
            //правим - от 100 до 1000 мс
            if(dl<100)dl=100;
            else if(dl>1000)dl=1000;
            //мигаем
            blink(dl);
            //инкрементируем число миганий
            ++blinksCount;
            //выдаем новое измерение количества миганий
            dev.disp().writeMeasurement("blinks_count",String(blinksCount).c_str());
            //сообщаем об успешном выполнении команды
            dev.disp().writeOk();
        }
        else if(strcmp(cmd,"get_blinks_count")==0)//команда get_blinks_count
        {
            //выдаем измерение количества миганий
            dev.disp().writeMeasurement("blinks_count",String(blinksCount).c_str());
            //сообщаем об успешном выполнении команды
            dev.disp().writeOk();
        }
        else dev.disp().writeErr("Unknown cmd",cmd);//неизвестная команда
    }
}ccb;

//setup
void setup()
{
    Serial.begin(9600);//запускаем Serial
    pinMode(ledPin,OUTPUT);//настраиваем пин для мигания
    dev.disp().installDevEventsHandler(&ccb);//устанавливаем обработчик команд
    dev.disp().setControls(interfaceStr);//указываем строку с описанием интерфейса управления
    dev.disp().setSensors(sensorsDef);//указываем строку с описанием сенсоров
}

//генерация отсчетов sin и cos
int t=0;
float sinCos[2];
void writeSinVal()
{
    //Генерируем строки со значениями
    sinCos[0]=sin(0.1*t);
    sinCos[1]=cos(0.1*t);
    //отправляем измерение
    dev.disp().writeMeasurementB("sin_x",sinCos,2);
    //увеличиваем "время"
    ++t;
}

void loop()
{
    //проверяем, нет ли данных в Serial
    while(Serial.available())
        dev.putByte(Serial.read());//если данные есть, передаем в объект библиотеки
    writeSinVal();//генерируем следующий отсчет sin и cos
    delay(500);//пауза пол-секунды
}

