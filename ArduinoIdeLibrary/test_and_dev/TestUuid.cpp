#include "TestUuid.h"
#include "ARpcUuid.h"
#include <QUuid>
#include <QDebug>

bool testUuid()
{
	qDebug()<<"TEST UUID";
	QUuid id=QUuid::createUuid();
	qDebug()<<"UUID: "<<id;
	qDebug()<<"TEST 0:";
	ARpcUuid id2(id.toByteArray().constData());
	if(!id2.isValid())return false;
	qDebug()<<"parse from string ok";
	char str[80];
	memset(str,0,80);
	id2.toString(str+4);
	qDebug()<<"str1: "<<QByteArray(str+4);
	if(QByteArray(str+4)!=id.toByteArray())return false;
	memset(str,0,80);
	id2.toHex(str+4);
	qDebug()<<"str2: "<<QByteArray(str+4);
	if(QByteArray(str+4)!=id.toRfc4122().toHex())return false;

	qDebug()<<"TEST 1:";
	ARpcUuid id3(id.toRfc4122().toHex());
	if(!id3.isValid())return false;
	qDebug()<<"parse from hex ok";
	memset(str,0,80);
	id3.toString(str+4);
	qDebug()<<QByteArray(str+4);
	if(QByteArray(str+4)!=id.toByteArray())return false;
	memset(str,0,80);
	id3.toHex(str+4);
	qDebug()<<QByteArray(str+4);
	if(QByteArray(str+4)!=id.toRfc4122().toHex())return false;
	return true;
}
