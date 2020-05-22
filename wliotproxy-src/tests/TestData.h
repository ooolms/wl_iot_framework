#ifndef TESTDATA_H
#define TESTDATA_H

#include "wliot/devices/SensorDef.h"
#include <QUuid>
#include <QVector>

extern const qint64 someTimestamp;

extern const QVector<float> singleData1;
extern const QByteArray singleData1Binary;
extern const QByteArray singleData1BinaryWithTs;
extern const QByteArrayList singleData1MsgArgs;
extern const QByteArrayList singleData1MsgArgsWithTs;

extern const QVector<float> singleData2;
extern const QByteArray singleData2Binary;
extern const QByteArrayList singleData2MsgArgsWithTs;

extern const QVector<float> packetData1F;
extern const QByteArray packetData1BinaryF;
extern const QByteArrayList packetData1MsgArgs;
extern const QByteArrayList packetData1MsgArgsWithTs;
extern const QVector<float> packetData1Sample0F;
extern const QVector<float> packetData1Sample1F;
extern const QVector<double> packetData1Sample0D;
extern const QVector<double> packetData1Sample1D;

extern const QByteArrayList textValueMsgArgs;
extern const QByteArrayList textValueMsgArgsWithTs;

extern const WLIOT::SensorDef singleNT;
extern const WLIOT::SensorDef singleLT;
extern const WLIOT::SensorDef singleGT;
extern const WLIOT::SensorDef packetNT;
extern const WLIOT::SensorDef packetGT;

extern const QString storPath;
extern const QUuid deviceId;
extern const QByteArray deviceName;

#endif // TESTDATA_H
