#ifndef SOURCEBLOCK_H
#define SOURCEBLOCK_H

#include "GIDL/core/BaseBlock.h"

/*
 * две копии data решают проблему с многозадачностью (хранилище не thread safe),
 * extractData вызывается из основного потока, затем вызывается prepareWorkData,
 * которая просто копирует next в work (это должно быть защищено мьютексом),
 * которая потом используется в рабочем потоке
*/
class SourceBlock
	:public BaseBlock
{
public:
	explicit SourceBlock(quint32 id);
	virtual ~SourceBlock();
	bool extractData();//call from main thread
	bool prepareWorkData();//call from work thread

protected:
	virtual void eval()override;
	virtual DataUnit extractDataInternal()=0;
	void setOutput(BlockOutput *o);

private:
	DataUnit mNextData,mWorkData;
	BlockOutput *out;
};

#endif // SOURCEBLOCK_H
