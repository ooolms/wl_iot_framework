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

#ifndef MATHEEXPBLOCK_H
#define MATHEEXPBLOCK_H

#include "VDIL/core/BaseBlock.h"

namespace WLIOTVDIL
{
	class MathExp;

	class MathExpBlock
		:public BaseBlock
	{
	public:
		explicit MathExpBlock(quint32 bId=0);
		virtual ~MathExpBlock();
		virtual QString groupName()const override;
		virtual QString blockName()const override;
		void setParams(const QStringList &varNames,const QString &expr);
		QString expr()const;
		QStringList vars()const;

	protected:
		virtual void eval()override;

	public:
		static const QString mBlockName;

	private:
		QString mExpr;
		QStringList mVarNames;
		BlockOutput *out;
		QMap<QString,BlockInput*> varInputs;
		QMap<QString,double*> varMap;
		MathExp *expCalc;
		bool eqParsed;
	};
}

#endif // MATHEEXPBLOCK_H
