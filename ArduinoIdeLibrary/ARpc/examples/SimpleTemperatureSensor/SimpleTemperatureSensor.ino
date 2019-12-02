//подключаем библиотеку ARpc
#include <ARpcDevice.h>

const char *deviceName="led_blink_test";//имя устройства
const ARpcUuid deviceId("f84526c15e88431581f8f7da45daa09d");//идентификатор устройства

int sensPin=A0;//пин датчика

//Описание датчиков
const char *sensorsDef="<sensors>"
"<sensor name=\"temperature\" type=\"f32_sv\"/>"//температурный датчик
"</sensors>";

class WriteCallback
    :public ARpcIWriteCallback
{
public:
    //callback-функции, вызываемые библиотекой, когда нужно передать данные от устройства
    virtual void writeData(const char *data,unsigned long sz)override
    {
        Serial.write(data,sz);
    }
    virtual void writeStr(const char *str)override
    {
        Serial.print(str);
    }
    virtual void writeStr(const __FlashStringHelper *str)override
    {
        Serial.print(str);
    }
}wcb;

//объект парсера ARpc, 300 - объем буфера для сообщения
ARpcDevice dev(300,&wcb,&deviceId,deviceName);

//setup
void setup()
{
    Serial.begin(9600);//запускаем Serial
    dev.disp().setSensors(sensorsDef);//указываем строку с описанием сенсоров
    dev.resetStream();
}

unsigned long newTime=0,prevSinTime=0;
void loop()
{
    //проверяем, нет ли данных в Serial
    while(Serial.available())
        dev.putByte(Serial.read());//если данные есть, передаем в объект библиотеки
    newTime=millis();
    if((newTime-prevSinTime)>=10000)
    {
    	float v=(float)analogRead(A0)*50.0/1023.0;//считываем значение с АЦП и нормируем его так, чтобы получить от 0 до 50 градусов
        dev.disp().writeMeasurement("temperature",String(v).c_str());
        prevSinTime=newTime;
    }
    delay(1);
}
