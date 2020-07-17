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

#ifndef MATHEXPFUNCTIONS_H
#define MATHEXPFUNCTIONS_H

#include "VDIL/support/MathExp.h"

namespace WLIOTVDIL
{
	class MathExpFuncSin
		:public MathExpIFunction
	{
	public:
		virtual int argsCount()override;
		virtual double calc(QList<MathSubExp*> &argsExpList)override;
	};

	class MathExpFuncCos
		:public MathExpIFunction
	{
	public:
		virtual int argsCount()override;
		virtual double calc(QList<MathSubExp*> &argsExpList)override;
	};

	class MathExpFuncTan
		:public MathExpIFunction
	{
	public:
		virtual int argsCount()override;
		virtual double calc(QList<MathSubExp*> &argsExpList)override;
	};

	class MathExpFuncAsin
		:public MathExpIFunction
	{
	public:
		virtual int argsCount()override;
		virtual double calc(QList<MathSubExp*> &argsExpList)override;
	};

	class MathExpFuncAcos
		:public MathExpIFunction
	{
	public:
		virtual int argsCount()override;
		virtual double calc(QList<MathSubExp*> &argsExpList)override;
	};

	class MathExpFuncAtan
		:public MathExpIFunction
	{
	public:
		virtual int argsCount()override;
		virtual double calc(QList<MathSubExp*> &argsExpList)override;
	};

	class MathExpFuncAtan2
		:public MathExpIFunction
	{
	public:
		virtual int argsCount()override;
		virtual double calc(QList<MathSubExp*> &argsExpList)override;
	};

	class MathExpFuncExp
		:public MathExpIFunction
	{
	public:
		virtual int argsCount()override;
		virtual double calc(QList<MathSubExp*> &argsExpList)override;
	};

	class MathExpFuncAbs
		:public MathExpIFunction
	{
	public:
		virtual int argsCount()override;
		virtual double calc(QList<MathSubExp*> &argsExpList)override;
	};

	class MathExpFuncLn
		:public MathExpIFunction
	{
	public:
		virtual int argsCount()override;
		virtual double calc(QList<MathSubExp*> &argsExpList)override;
	};

	class MathExpFuncPow
		:public MathExpIFunction
	{
	public:
		virtual int argsCount()override;
		virtual double calc(QList<MathSubExp*> &argsExpList)override;
	};

	class MathExpFuncSqrt
		:public MathExpIFunction
	{
	public:
		virtual int argsCount()override;
		virtual double calc(QList<MathSubExp*> &argsExpList)override;
	};

	class MathExpBuiltinFunctions
	{
	public:
		static MathExpBuiltinFunctions& inst();
		MathExpIFunction* func(const QString &name);

	private:
		MathExpBuiltinFunctions();
		~MathExpBuiltinFunctions();

	private:
		QMap<QString,MathExpIFunction*> funcMap;
	};
}

#endif // MATHEXPFUNCTIONS_H
