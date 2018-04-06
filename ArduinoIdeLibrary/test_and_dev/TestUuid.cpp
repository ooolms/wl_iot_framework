#include "TestUuid.h"
#include "ARpcUuid.h"
#include <QUuid>
#include <QDebug>

QUuid id=QUuid::createUuid();

bool testUuid0()
{
	ARpcUuid id2(id.toByteArray().constData());
	if(!id2.isValid())return false;
	qDebug()<<"parse from string ok";
	char str[39];
	memset(str,0,39);
	id2.toString(str);
	qDebug()<<"str1: "<<str;
	if(str!=id.toByteArray())return false;
	memset(str,0,39);
	id2.toHex(str);
	qDebug()<<"str2: "<<str;
	if(str!=id.toRfc4122().toHex())return false;
	return true;
}

bool testUuid1()
{
	ARpcUuid id2(id.toRfc4122().toHex());
	if(!id2.isValid())return false;
	qDebug()<<"parse from hex ok";
	char str[39];
	memset(str,0,39);
	id2.toString(str);
	qDebug()<<str;
	if(str!=id.toByteArray())return false;
	memset(str,0,39);
	id2.toHex(str);
	qDebug()<<str;
	if(str!=id.toRfc4122().toHex())return false;
	return true;
}

bool testUuid()
{
	bool ok=true,testOk;
	qDebug()<<"TEST UUID";
	qDebug()<<"UUID: "<<id;

	QList<bool(*)()> tests=QList<bool(*)()>()<<&testUuid0<<&testUuid1;
	for(int i=0;i<tests.count();++i)
	{
		qDebug()<<"TEST "<<i<<":";
		testOk=tests[i]();
		if(!testOk)
			qDebug()<<"FAILED";
		else qDebug()<<"OK";
		ok=ok&&testOk;
	}

	return ok;
}
