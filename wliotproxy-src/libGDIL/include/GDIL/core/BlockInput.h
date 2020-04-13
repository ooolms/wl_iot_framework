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

#ifndef BLOCKINPUT_H
#define BLOCKINPUT_H

#include "GDIL/core/BlockPort.h"

class BlockOutput;
class BaseBlock;

//supported dim: 0 - any dim, >=1 - only specified dim
class BlockInput
	:public BlockPort
{
public:
	//for program editing
	DataUnit::Types supportedTypes()const;
	quint32 supportedDim()const;
	bool canConnectType(DataUnit::Type t,quint32 dim)const;
	virtual DataUnit::Type type()const override;
	virtual quint32 dim()const override;
	BaseBlock* block();
	const BaseBlock* block()const;
	BlockOutput* linkedOutput();

	//for work
	void reset();
	bool setData(DataUnit u);
	bool isReady();
	const DataUnit& data();

private:
	explicit BlockInput(BaseBlock *b,DataUnit::Types suppTypes,DataUnit::Type currType,quint32 supportedDim,
		const QString &title);
	virtual ~BlockInput();
	virtual BlockInput& operator=(const BlockInput &b)=delete;

private:
	//for program editing
	DataUnit::Types mSupportedTypes;
	quint32 mSupportedDim;
	DataUnit::Type mCurrentType;
	quint32 mCurrentDim;
	BlockOutput *mLinkedOutput;
	BaseBlock *mBlock;

	//for work
	bool mDataIsSet;
	DataUnit mData;

	friend class BaseBlock;
	friend class BlockOutput;
};

#endif // BLOCKINPUT_H
