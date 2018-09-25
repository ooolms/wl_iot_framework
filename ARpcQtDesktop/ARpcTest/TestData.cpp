#include "TestData.h"

const qint64 someTimestamp=1122;

const QVector<float> singleData1={12.0f,621.124f,2845.125626f};
const QByteArray singleData1Binary=QByteArray((const char*)singleData1.constData(),singleData1.size()*sizeof(float));
const QByteArray singleData1BinaryWithTs=QByteArray((const char*)&someTimestamp,sizeof(qint64)).append(
	QByteArray((const char*)singleData1.constData(),singleData1.size()*sizeof(float)));
const QByteArrayList singleData1MsgArgs={"12.0","621.124","2845.125626"};
const QByteArrayList singleData1MsgArgsWithTs={"1122","12.0","621.124","2845.125626"};

const QVector<float> singleData2={12.0f,715.14f,34.11f};
const QByteArray singleData2Binary=QByteArray((const char*)singleData2.constData(),singleData2.size()*sizeof(float));
const QByteArrayList singleData2MsgArgs={"12.0","715.14","34.11"};
const QByteArrayList singleData2MsgArgsWithTs={"1122","12.0","715.14","34.11"};

const QVector<float> packetData1F={12.0f,621.124f,2845.125626f,156.6f,124.1f,0.1f};
const QVector<double> packetData1D={12.0L,621.124L,2845.125626L,156.6L,124.1L,0.1L};
const QByteArray packetData1BinaryF=QByteArray((const char*)packetData1F.constData(),packetData1F.size()*sizeof(float));
const QByteArray packetData1BinaryD=QByteArray((const char*)packetData1D.constData(),
	packetData1D.size()*sizeof(double));
const QByteArrayList packetData1MsgArgs={"12.0","621.124","2845.125626","156.6","124.1","0.1"};
const QByteArrayList packetData1MsgArgsWithTs={"1122","12.0","621.124","2845.125626","156.6","124.1","0.1"};
const QVector<float> packetData1Sample0F={12.0f,621.124f,2845.125626f};
const QVector<float> packetData1Sample1F={156.6f,124.1f,0.1f};
const QVector<double> packetData1Sample0D={12.0L,621.124L,2845.125626L};
const QVector<double> packetData1Sample1D={156.6L,124.1L,0.1L};

const QByteArrayList textValueMsgArgs={"text0","text1"};
const QByteArrayList textValueMsgArgsWithTs={"1122","text0","text1"};

const ARpcSensorDef singleNT=ARpcSensorDef(
	ARpcSensorDef::Type(ARpcSensorDef::F32,ARpcSensorDef::SINGLE,ARpcSensorDef::NO_TIME,3),"test_sensor","","",{});
const ARpcSensorDef singleLT=ARpcSensorDef(
	ARpcSensorDef::Type(ARpcSensorDef::F32,ARpcSensorDef::SINGLE,ARpcSensorDef::LOCAL_TIME,3),"test_sensor","","",{});
const ARpcSensorDef singleGT=ARpcSensorDef(
	ARpcSensorDef::Type(ARpcSensorDef::F32,ARpcSensorDef::SINGLE,ARpcSensorDef::GLOBAL_TIME,3),"test_sensor","","",{});
const ARpcSensorDef packetNT=ARpcSensorDef(
	ARpcSensorDef::Type(ARpcSensorDef::F32,ARpcSensorDef::PACKET,ARpcSensorDef::NO_TIME,3),"test_sensor","","",{});
const ARpcSensorDef packetGT=ARpcSensorDef(
	ARpcSensorDef::Type(ARpcSensorDef::F32,ARpcSensorDef::PACKET,ARpcSensorDef::GLOBAL_TIME,3),"test_sensor","","",{});

const QString storPath=QString("/tmp/ARpcTestStorage");
const QUuid deviceId=QUuid("{9e693b9e-a6ef-4260-a5dd-0e1812fdf514}");
const QByteArray deviceName="test_device";
