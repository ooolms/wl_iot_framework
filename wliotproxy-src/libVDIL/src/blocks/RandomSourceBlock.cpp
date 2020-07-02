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

#include "VDIL/blocks/RandomSourceBlock.h"
#include "VDIL/core/Program.h"
#include "VDIL/core/CoreBlocksGroupFactory.h"
#include <stdlib.h>
#include <limits>

using namespace WLIOT;
using namespace WLIOTVDIL;

const QString RandomSourceBlock::mBlockName=QString("random_source");

RandomSourceBlock::RandomSourceBlock(quint32 bId)
	:SourceBlock(bId)
{
	mBounds.append(qMakePair(std::numeric_limits<int>::min(),std::numeric_limits<int>::max()));
	out=mkOutput(DataUnit::SINGLE,1,"out");
}

void RandomSourceBlock::setBounds(const QList<QPair<qint32,qint32>> &bounds)
{
	mBounds=bounds;
	if(mBounds.isEmpty())
		mBounds.append(qMakePair(std::numeric_limits<int>::min(),std::numeric_limits<int>::max()));
	for(auto &i:mBounds)
	{
		if(i.second<=i.first)
		{
			i.first=0;
			i.second=RAND_MAX;
		}
	}
	if(out->dim()!=mBounds.count())
		out->replaceTypeAndDim(DataUnit::SINGLE,mBounds.count());
}

QString RandomSourceBlock::groupName()const
{
	return CoreBlocksGroupFactory::mGroupName;
}

QString RandomSourceBlock::blockName()const
{
	return mBlockName;
}

const QList<QPair<qint32,qint32>>& RandomSourceBlock::bounds()const
{
	return mBounds;
}

DataUnit WLIOTVDIL::RandomSourceBlock::extractDataInternal()
{
	QVector<qint64> vals;
	vals.resize(mBounds.size());
	for(int i=0;i<mBounds.count();++i)
	{
		auto &b=mBounds[i];
		vals[i]=(qint64)b.first+(qint64)qrand()*(qint64)(b.second-b.first)/(qint64)RAND_MAX;
	}
	return DataUnit(vals);
}
