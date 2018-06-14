#ifndef TESTNETWRITERS_H
#define TESTNETWRITERS_H

#include "ARpcIWriteCallback.h"
#include <QByteArray>

class ARpcStarNetDevice;
class ARpcStarNetEndPoint;

class TestWriterBase
	:public ARpcIWriteCallback
{
public:
	TestWriterBase(const QByteArray &fromName,const QByteArray &toName);
	virtual void writeData(const char *data,unsigned long size)override;
	virtual void writeStr(const char *str)override;

protected:
	virtual void writeB2ToDev()=0;

protected:
	QByteArray b2;

private:
	QByteArray mFromName,mToName;
	QByteArray b1;
};

class TestWriterStarNetDevice
	:public TestWriterBase
{
public:
	TestWriterStarNetDevice(const QByteArray &fromName,const QByteArray &toName,int devInput);
	void setDevice(ARpcStarNetDevice *dev);

protected:
	virtual void writeB2ToDev()override;

private:
	ARpcStarNetDevice *mDevice;
	int input;
};

class TestWriterStarNetEndPoint
	:public TestWriterBase
{
public:
	TestWriterStarNetEndPoint(const QByteArray &fromName,const QByteArray &toName);
	void setDevice(ARpcStarNetEndPoint *dev);

protected:
	virtual void writeB2ToDev()override;

private:
	ARpcStarNetEndPoint *mDevice;
};

class TestNullWriter
	:public ARpcIWriteCallback
{
public:
	virtual void writeData(const char *data,unsigned long size)override;
	virtual void writeStr(const char *str)override;
};

#endif // TESTNETWRITERS_H
