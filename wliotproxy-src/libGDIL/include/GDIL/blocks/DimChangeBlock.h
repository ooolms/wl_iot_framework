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

#ifndef DIMCHANGEBLOCK_H
#define DIMCHANGEBLOCK_H

#include "GDIL/core/BaseBlock.h"

class DimChangeBlock
	:public BaseBlock
{
public:
	explicit DimChangeBlock(quint32 bId=0);
	void setDim(quint32 d);
	quint32 dim()const;
	virtual QUuid typeId()const override;

protected:
	virtual void eval()override;
	virtual void onInputTypeSelected(BlockInput *b)override;

public:
	static const QUuid mTypeId;

private:
	BlockInput *in;
	BlockOutput *out;
	quint32 mDim;
};

#endif // DIMCHANGEBLOCK_H
