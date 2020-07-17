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

#include "VDIL/blocks/MathExpBlock.h"
#include "VDIL/core/CoreBlocksGroupFactory.h"
#include "VDIL/support/MathExp.h"

using namespace WLIOT;
using namespace WLIOTVDIL;

const QString MathExpBlock::mBlockName=QString("math_exp");

MathExpBlock::MathExpBlock(quint32 bId)
	:BaseBlock(bId)
{
	out=mkOutput(DataUnit::SINGLE,1,"result");
	expCalc=new MathExp;
	eqParsed=false;
}

MathExpBlock::~MathExpBlock()
{
	delete expCalc;
	for(double *d:varMap)
		delete d;
	varMap.clear();
}

QString MathExpBlock::groupName()const
{
	return CoreBlocksGroupFactory::mGroupName;
}

QString MathExpBlock::blockName()const
{
	return mBlockName;
}

void MathExpBlock::setParams(const QStringList &varNames,const QString &expr)
{
	mExpr=expr;
	mVarNames=varNames;
	expCalc->clear();
	for(double *d:varMap)
		delete d;
	varMap.clear();
	for(QString k:varInputs.keys())
	{
		if(!mVarNames.contains(k))
		{
			rmInput(varInputs[k]);
			varInputs.remove(k);
		}
	}
	for(const QString &v:mVarNames)
		if(!varInputs.contains(v))
			varInputs[v]=mkInput(TypeConstraints(DataUnit::SINGLE,1),DataUnit::SINGLE,v);
	for(const QString &v:mVarNames)
		varMap[v]=new double(0.0);
	eqParsed=expCalc->parse(expr,varMap);
}

QString MathExpBlock::expr()const
{
	return mExpr;
}

QStringList MathExpBlock::vars()const
{
	return mVarNames;
}

void MathExpBlock::eval()
{
	for(QString k:varInputs.keys())
		*(varMap[k])=varInputs[k]->data().value()->valueToDouble(0);
	double v;
	if(eqParsed&&expCalc->calc(v))
		out->setData(DataUnit(v));
}
