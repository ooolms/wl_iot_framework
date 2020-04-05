#ifndef LOGICALBLOCKS_H
#define LOGICALBLOCKS_H

#include "GDIL/core/BaseBlock.h"

class NotBoolBlock
	:public BaseBlock
{
public:
	explicit NotBoolBlock(quint32 bId=0);
	virtual QUuid typeId()const override;

protected:
	virtual void eval()override;

public:
	static const QUuid mTypeId;

private:
	BlockInput *in;
	BlockOutput *out;
};

class Base2InputsBoolBlock
	:public BaseBlock
{
public:
	explicit Base2InputsBoolBlock(quint32 bId);

protected:
	virtual void eval()override;
	virtual quint8 calc(quint8 v1,quint8 v2)=0;

public:
	static const QUuid mTypeId;

private:
	BlockInput *in1;
	BlockInput *in2;
	BlockOutput *out;
};

class AndBoolBlock
	:public Base2InputsBoolBlock
{
public:
	explicit AndBoolBlock(quint32 bId=0);
	virtual QUuid typeId()const override;

public:
	static const QUuid mTypeId;

protected:
	virtual quint8 calc(quint8 v1,quint8 v2)override;
};

class OrBoolBlock
	:public Base2InputsBoolBlock
{
public:
	explicit OrBoolBlock(quint32 bId=0);
	virtual QUuid typeId()const override;

public:
	static const QUuid mTypeId;

protected:
	virtual quint8 calc(quint8 v1,quint8 v2)override;
};

class XorBoolBlock
	:public Base2InputsBoolBlock
{
public:
	explicit XorBoolBlock(quint32 bId=0);
	virtual QUuid typeId()const override;

public:
	static const QUuid mTypeId;

protected:
	virtual quint8 calc(quint8 v1,quint8 v2)override;
};

class AndNotBoolBlock
	:public Base2InputsBoolBlock
{
public:
	explicit AndNotBoolBlock(quint32 bId=0);
	virtual QUuid typeId()const override;

public:
	static const QUuid mTypeId;

protected:
	virtual quint8 calc(quint8 v1,quint8 v2)override;
};

class OrNotBoolBlock
	:public Base2InputsBoolBlock
{
public:
	explicit OrNotBoolBlock(quint32 bId=0);
	virtual QUuid typeId()const override;

public:
	static const QUuid mTypeId;

protected:
	virtual quint8 calc(quint8 v1,quint8 v2)override;
};

#endif // LOGICALBLOCKS_H
