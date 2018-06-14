#include "TestNetWriters.h"
#include "ARpcStarNetDevice.h"
#include "ARpcStarNetEndPoint.h"
#include <QDebug>

TestWriterBase::TestWriterBase(const QByteArray &fromName,const QByteArray &toName)
{
	mFromName=fromName;
	mToName=toName;
}

void TestWriterBase::writeData(const char *data, unsigned long size)
{
	for(unsigned long i=0;i<size;++i)
	{
		b1.append(data[i]);
		if(data[i]=='\n')
		{
			b2=b1;
			b1.clear();
			qDebug()<<"Data "<<mFromName<<" to "<<mToName<<": "<<b2;
		}
		writeB2ToDev();
	}
}

void TestWriterBase::writeStr(const char *str)
{
	unsigned long size=strlen(str);
	for(unsigned long i=0;i<size;++i)
	{
		b1.append(str[i]);
		if(str[i]=='\n')
		{
			b2=b1;
			b1.clear();
			qDebug()<<"Data "<<mFromName<<" to "<<mToName<<": "<<b2;
			writeB2ToDev();
		}
	}
}

TestWriterStarNetDevice::TestWriterStarNetDevice(
	const QByteArray &fromName,const QByteArray &toName,int devInput)
	:TestWriterBase(fromName,toName)
{
	input=devInput;
}

void TestWriterStarNetDevice::setDevice(ARpcStarNetDevice *dev)
{
	mDevice=dev;
}

void TestWriterStarNetDevice::writeB2ToDev()
{
	if(input==0)
		mDevice->putData1(b2.constData(),b2.size());
	else mDevice->putData2(b2.constData(),b2.size());
}

TestWriterStarNetEndPoint::TestWriterStarNetEndPoint(const QByteArray &fromName,const QByteArray &toName)
	:TestWriterBase(fromName,toName)
{
}

void TestWriterStarNetEndPoint::setDevice(ARpcStarNetEndPoint *dev)
{
	mDevice=dev;
}

void TestWriterStarNetEndPoint::writeB2ToDev()
{
	mDevice->putData(b2.constData(),b2.size());
}

void TestNullWriter::writeData(const char *data,unsigned long size)
{
	Q_UNUSED(data)
	Q_UNUSED(size)
}

void TestNullWriter::writeStr(const char *str)
{
	Q_UNUSED(str)
}
