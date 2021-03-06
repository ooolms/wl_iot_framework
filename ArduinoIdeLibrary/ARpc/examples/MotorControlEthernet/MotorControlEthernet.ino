/*
 * Remotely controlled step engine. Used Ethernet2 library for W5500 module (change to Ethernet library for W5100 module).
 * Device can be controlled wia network (TCP port 4081) or usb tty (Serial on 9600).
 * 
 * This example code is in the public domain.
*/

#include <Stepper.h>
#include <ARpcDevice.h>
#include <ARpcSrvReady.h>
#include <Ethernet2.h>
#include <EthernetUdp2.h>
#include <IPAddress.h>

Stepper motor(200,2,3,4,5);
byte mac[]={0x00, 0xcc, 0x44, 0xCC, 0xDE, 0x02};
uint16_t port=4081;
IPAddress bCastSenderIp;
bool connecting=false;
EthernetUDP bCastCli;
EthernetServer server(port);
EthernetClient client;
unsigned long lastSyncMillis=0;

const char *deviceName="engine_control";//имя устройства
const ARpcUuid deviceId("4578a2f4518e4215972677ba837f9a62");//идентификатор устройства

const char *interfaceStr="<cls><g t=\"Device controls\">"
"<c c=\"fwd\" t=\"Forward\"/>"
"<c c=\"bck\" t=\"Backward\"/>"
"</g></cls>";

//callback for writing data to Serial
class SerialWriteCallback
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
}serialCb;

//callback for writing data to network client
class NetWriteCallback
    :public ARpcIWriteCallback
{
public:
    virtual void writeData(const char *data,unsigned long sz)
    {
        client.write(data,sz);
    }

    virtual void writeStr(const char *str)
    {
        client.print(str);
    }
};
NetWriteCallback netCb;

//Serial parser
ARpcDevice serialDev(100,&serialCb,&deviceId,deviceName);

//network parser
ARpcDevice netDev(100,&netCb,&deviceId,deviceName);

//command callback for commands processing
class CommandCallback
    :public ARpcIDevEventsCallback
{
public:
    explicit CommandCallback(ARpcDevice *dev)
    {
        d=dev;
    }
    
    virtual void processCommand(const char *cmd,const char *args[],unsigned char argsCount)
    {
        if(strcmp(cmd,"fwd")==0)//"fwd" command
        {
            for(int i=0;i<8;++i)
            {
                motor.step(30);
                delay(1);
            }
            d->disp().writeOk();
        }
        else if(strcmp(cmd,"bck")==0)//"bck" command
        {
            for(int i=0;i<8;++i)
            {
                motor.step(-30);
                delay(1);
            }
            d->disp().writeOk();
        }
        else d->disp().writeErr("Unknown cmd");
    }

    virtual void onSyncMsg()
    {
        lastSyncMillis=millis();
    }

private:
    ARpcDevice *d;//device to work with
};
CommandCallback serialCmdCb(&serialDev);//serial command callback
CommandCallback netCmdCb(&netDev);//network command callback

//callback for server_ready broadcast UDP messages
class SrvReadyCallback
    :public ARpcISrvReadyCallback
{
public:
    virtual void processSrvReadyMsg(const ARpcUuid &srvId,const char *srvName)
    {
        serialDev.disp().writeInfo("Server detected: ",String(bCastSenderIp).c_str(),srvName);
        //if client already connected, ignoring
        if(client.connected()||connecting)return;
        serialDev.disp().writeInfo("Connecting to server...");
        connecting=true;
        //connecting to server found
        client.connect(bCastSenderIp,port);
        for(int i=0;i<20;++i)
        {
            if(client.connected())
                break;
            delay(100);
        }
        serialDev.disp().writeInfo("Connected to server");
        connecting=false;
    }
};
SrvReadyCallback srvReadyCb;

//parser for server_ready broadcast UDP messages
ARpcSrvReady srvReadyParser(40,&srvReadyCb);

void setup()
{
    delay(2000);
    Serial.begin(9600);
    //setting motor speed
    motor.setSpeed(90);
    //setting controls description
    serialDev.disp().setControls(interfaceStr);
    netDev.disp().setControls(interfaceStr);
    //installing command callbacks
    serialDev.disp().installDevEventsHandler(&serialCmdCb);
    netDev.disp().installDevEventsHandler(&netCmdCb);
    //starting Ethernet
    Ethernet.begin(mac);
    bCastCli.begin(port);
    server.begin();
    serialDev.disp().writeInfo("Ethernet started");
    serialDev.disp().writeInfo(String(Ethernet.localIP()).c_str());
    lastSyncMillis=millis();
	serialDev.resetStream();
}

//check for incoming UDP server_ready messages
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

void checkNetClient()
{
    if(client)//client was connected
    {
        if(!client.connected())//check if client was disconnected
        {
            serialDev.disp().writeInfo("Client connection lost");
            //trying next incoming connection
            client=server.available();
            if(client)
            {
                serialDev.disp().writeInfo("Take next pending incoming connection");
                lastSyncMillis=millis();
				netDev.resetParser();
                netDev.resetStream();
            }
            delay(100);
        }
        else
        {
            //check if new data appeared
            while(client.available())
                netDev.putByte(client.read());
        }
    }
    else
    {
        //trying next incoming connection
        client=server.available();
        if(client)
        {
            serialDev.disp().writeInfo("Take next pending incoming connection");
            lastSyncMillis=millis();
			netDev.resetParser();
			netDev.resetStream();
        }
        delay(100);
    }
}

void loop()
{
    //check if new data appeared on Serial
    while(Serial.available())
        serialDev.putByte(Serial.read());
        
    checkBCastCli();
    checkNetClient();

    if(millis()>(lastSyncMillis+6000)&&client.connected())
    {
        client.stop();
        checkNetClient();
    }
    delay(1);
}
