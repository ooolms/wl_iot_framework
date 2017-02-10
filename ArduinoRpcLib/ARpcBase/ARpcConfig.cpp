#include "ARpcConfig.h"

const QString ARpcConfig::funcCallMsg=QString("call");
const QString ARpcConfig::funcAnswerErrMsg=QString("err");
const QString ARpcConfig::funcAnswerOkMsg=QString("ok");
const QString ARpcConfig::funcSyncMsg=QString("sync");
const QString ARpcConfig::measurementMsg=QString("meas");
const QString ARpcConfig::infoMsg=QString("info");
const QString ARpcConfig::identifyMsg=QString("identify");
const QString ARpcConfig::deviceInfoMsg=QString("deviceinfo");
const QChar ARpcConfig::msgDelim=QChar('\n');
const QChar ARpcConfig::argDelim=QChar('|');
const int ARpcConfig::syncCallWaitTime=2000;
const int ARpcConfig::identifyWaitTime=6000;

const QString ARpcConfig::getSensorsCommand=QString("#sensors");
const QString ARpcConfig::getControlsCommand=QString("#controls");
