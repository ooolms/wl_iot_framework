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

#include "VDIL/support/MathExpFunctions.h"
#include <QtMath>

using namespace WLIOTVDIL;

int MathExpFuncSin::argsCount()
{
	return 1;
}

double MathExpFuncSin::calc(QList<MathSubExp *> &argsExpList)
{
	return qSin(argsExpList[0]->calc());
}

int MathExpFuncCos::argsCount()
{
	return 1;
}

double MathExpFuncCos::calc(QList<MathSubExp *> &argsExpList)
{
	return qCos(argsExpList[0]->calc());
}

int MathExpFuncTan::argsCount()
{
	return 1;
}

double MathExpFuncTan::calc(QList<MathSubExp *> &argsExpList)
{
	return qTan(argsExpList[0]->calc());
}

int MathExpFuncAsin::argsCount()
{
	return 1;
}

double MathExpFuncAsin::calc(QList<MathSubExp *> &argsExpList)
{
	return qAsin(argsExpList[0]->calc());
}

int MathExpFuncAcos::argsCount()
{
	return 1;
}

double MathExpFuncAcos::calc(QList<MathSubExp *> &argsExpList)
{
	return qAcos(argsExpList[0]->calc());
}

int MathExpFuncAtan::argsCount()
{
	return 1;
}

double MathExpFuncAtan::calc(QList<MathSubExp *> &argsExpList)
{
	return qAtan(argsExpList[0]->calc());
}

int MathExpFuncAtan2::argsCount()
{
	return 2;
}

double MathExpFuncAtan2::calc(QList<MathSubExp *> &argsExpList)
{
	return qAtan2(argsExpList[0]->calc(),argsExpList[1]->calc());
}

int MathExpFuncExp::argsCount()
{
	return 1;
}

double MathExpFuncExp::calc(QList<MathSubExp *> &argsExpList)
{
	return qExp(argsExpList[0]->calc());
}

int MathExpFuncAbs::argsCount()
{
	return 1;
}

double MathExpFuncAbs::calc(QList<MathSubExp *> &argsExpList)
{
	return qFabs(argsExpList[0]->calc());
}

int MathExpFuncLn::argsCount()
{
	return 1;
}

double MathExpFuncLn::calc(QList<MathSubExp *> &argsExpList)
{
	return qLn(argsExpList[0]->calc());
}

int MathExpFuncPow::argsCount()
{
	return 2;
}

double MathExpFuncPow::calc(QList<MathSubExp *> &argsExpList)
{
	return qPow(argsExpList[0]->calc(),argsExpList[1]->calc());
}

int MathExpFuncSqrt::argsCount()
{
	return 1;
}

double MathExpFuncSqrt::calc(QList<MathSubExp *> &argsExpList)
{
	return qSqrt(argsExpList[0]->calc());
}

MathExpBuiltinFunctions& MathExpBuiltinFunctions::inst()
{
	static MathExpBuiltinFunctions var;
	return var;
}

MathExpIFunction *MathExpBuiltinFunctions::func(const QString &name)
{
	return funcMap.value(name);
}

MathExpBuiltinFunctions::MathExpBuiltinFunctions()
{
	funcMap["sin"]=new MathExpFuncSin;
	funcMap["cos"]=new MathExpFuncCos;
	funcMap["tan"]=new MathExpFuncTan;
	funcMap["asin"]=new MathExpFuncAsin;
	funcMap["acos"]=new MathExpFuncAcos;
	funcMap["atan"]=new MathExpFuncAtan;
	funcMap["atan2"]=new MathExpFuncAtan2;
	funcMap["abs"]=new MathExpFuncAbs;
	funcMap["exp"]=new MathExpFuncExp;
	funcMap["ln"]=new MathExpFuncLn;
	funcMap["pow"]=new MathExpFuncPow;
	funcMap["sqrt"]=new MathExpFuncSqrt;
}

MathExpBuiltinFunctions::~MathExpBuiltinFunctions()
{
	for(auto f:funcMap)
		delete f;
}
