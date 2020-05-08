/*******************************************
Copyright 2017 OOO "LMS"

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

	http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.*/

#ifndef CONDITIONTRANSACTIONBLOCK_H
#define CONDITIONTRANSACTIONBLOCK_H

#include "GDIL/core/BaseBlock.h"

class ConditionTransactionBlock
	:public BaseBlock
{
public:
	explicit ConditionTransactionBlock(quint32 bId=0);
	virtual QString groupName()const override;
	virtual QString blockName()const override;

protected:
	virtual void eval()override;
	virtual void onInputTypeSelected(BlockInput *b)override;

public:
	static const QString mBlockName;

private:
	BlockInput *boolIn;
	BlockInput *in;
	BlockOutput *out;
};

#endif // CONDITIONTRANSACTIONBLOCK_H