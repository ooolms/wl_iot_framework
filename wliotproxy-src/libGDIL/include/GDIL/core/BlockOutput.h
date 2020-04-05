#ifndef BLOCKOUTPUT_H
#define BLOCKOUTPUT_H

#include "GDIL/core/BlockPort.h"
#include <QList>

//CRIT make "relink" operation to other output block,
//that move all links from this block to other one (with type checks, unmet inputs must be unlinked)

class BlockInput;
class BaseBlock;

class BlockOutput
	:public BlockPort
{
public:
	explicit BlockOutput(BaseBlock *b,DataUnit::Type t,quint32 dim,const QString &title);
	virtual ~BlockOutput();

	//for program editing
	virtual DataUnit::Type type()const override;
	virtual quint32 dim()const override;
	bool linkTo(BlockInput *input);
	void unlinkFrom(BlockInput *input);
	int linksCount()const;
	const BlockInput* link(int index)const;
	BaseBlock* block();

	//for work
	void setData(DataUnit u);

private:
	//for program editing
	QList<BlockInput*> linkedInputs;
	DataUnit::Type mType;
	quint32 mDim;
	BaseBlock *mBlock;

	//for work
	//???
};

#endif // BLOCKOUTPUT_H
