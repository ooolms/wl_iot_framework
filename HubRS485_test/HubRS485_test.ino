#include <SoftwareSerial.h>

/*Leonardo:
 * Serial - to PC
 * Serial1 - to ADM488 RS-485 device (fullduplex)
 * serial2 - to ADM488 on softserial pins rx=8,tx=9;
 */

SoftwareSerial serial2(8,9);

void setup()
{
    Serial.begin(9600);
    Serial1.begin(9600);
    serial2.begin(9600);
}

void loop()
{
    while(Serial.available())
    {
        char c=Serial.read();
        Serial1.write(&c,1);
        serial2.write(&c,1);
    }
    while(Serial1.available())
    {
        char c=Serial1.read();
        Serial.write(&c,1);
    }
    while(serial2.available())
    {
        char c=serial2.read();
        Serial.write(&c,1);
    }
    delay(10);
}
