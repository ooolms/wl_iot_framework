#ifndef BLOCKOUTPUT_H
#define BLOCKOUTPUT_H

#include "GIDL/core/DataUnit.h"
#include <QSet>

class BlockInput;

class BlockOutput
{
public:
	BlockOutput();
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

	//for work
	//???
};

#endif // BLOCKOUTPUT_H
