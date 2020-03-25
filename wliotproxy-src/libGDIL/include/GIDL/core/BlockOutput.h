#ifndef BLOCKOUTPUT_H
#define BLOCKOUTPUT_H

#include "GIDL/core/DataUnit.h"
#include <QSet>

//CRIT make "relink" operation to other output block,
//that move all links from this block to other one (with type checks, unmet inputs must be unlinked)

class BlockInput;

class BlockOutput
{
public:
	explicit BlockOutput(DataUnit::Type t,quint32 dim);
	~BlockOutput();

	//for program editing
	DataUnit::Type outputType()const;
	bool linkTo(BlockInput *input);
	void unlinkFrom(BlockInput *input);

	//for work
	void setData(DataUnit u);

private:
	//for program editing
	QSet<BlockInput*> linkedInputs;
	DataUnit::Type mType;
	quint32 mDim;

	//for work
	//???
};

#endif // BLOCKOUTPUT_H
