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

#include "VDIL/support/MathExp.h"
#include "VDIL/support/MathExpFunctions.h"
#include <exception>
#include <stdexcept>
#include <QScopedPointer>
#include <cmath>

using namespace WLIOTVDIL;

bool MathExp::parse(const QString &eq,const QMap<QString,double*> &varsMap)
{
	mainSubExpr.clear();
	return mainSubExpr.parse(eq,varsMap);
}

void MathExp::clear()
{
	mainSubExpr.clear();
}

bool MathExp::calc(double &result)
{
	try
	{
		result=mainSubExpr.calc();
		auto r=std::fpclassify(result);
		if(r==FP_NORMAL||r==FP_ZERO)
			return true;
		else
		{
			result=0;
			return false;
		}
	}
	catch(std::runtime_error &e)
	{
		//TODO error processing
		return false;
	}
}

MathSubExp::~MathSubExp()
{
	clear();
}

double MathSubExp::add(double v1,double v2)
{
	return v1+v2;
}

double MathSubExp::sub(double v1,double v2)
{
	return v1-v2;
}

double MathSubExp::mult(double v1,double v2)
{
	return v1*v2;
}

double MathSubExp::div(double v1,double v2)
{
	return v1/v2;
}

bool MathSubExp::parse(QString eq,const QMap<QString,double*> &varsMap)
{
	QScopedPointer<MathSubExpParser> parser(new MathSubExpParser(this,eq,varsMap));
	return parser->parse();
}

void MathSubExp::checkOpsForPriorities()
{
	bool hasAddSubOps=false,hasMultDivOps=false;
	for(auto o:operands)
		o->checkOpsForPriorities();
	for(auto o:operations)
	{
		if(o==&MathSubExp::add||o==&MathSubExp::sub)
		{
			hasAddSubOps=true;
			break;
		}
	}
	for(auto o:operations)
	{
		if(o==&MathSubExp::mult||o==&MathSubExp::div)
		{
			hasMultDivOps=true;
			break;
		}
	}
	if(!hasAddSubOps||!hasMultDivOps)return;
	for(int i=0;i<operations.count();++i)
	{
		auto oi=operations[i];
		if(oi==&MathSubExp::add||oi==&MathSubExp::sub)
			continue;
		QList<MathExpIOperand*> extractedOperands;
		QList<SimpleOperation> extractedOperations;
		extractedOperands.append(operands[i]);
		for(int j=i;j<operations.count();++j)
		{
			auto oj=operations[j];
			if(oj==&MathSubExp::add||oj==&MathSubExp::sub)
				break;
			extractedOperands.append(operands[j+1]);
			extractedOperations.append(operations[j]);
		}
		int c=extractedOperations.count();
		operands.removeAt(i);
		for(int j=0;j<c;++j)
		{
			operands.removeAt(i);
			operations.removeAt(i);
		}
		MathExpSubExpOperand *o=new MathExpSubExpOperand;
		o->exp->operands=extractedOperands;
		o->exp->operations=extractedOperations;
		operands.insert(i,o);
	}
}

double MathSubExp::calc()
{
	if(operands.isEmpty())
		return 0;
	double v=operands[0]->calc();
	for(int i=0;i<operations.count();++i)
		v=operations[i](v,operands[i+1]->calc());
	return v;
}

void MathSubExp::clear()
{
	for(auto o:operands)
		delete o;
	operands.clear();
	operations.clear();
}

MathSubExpParser::MathSubExpParser(MathSubExp *e,const QString &eq,const QMap<QString,double*> &varsMap)
	:vars(varsMap)
{
	equation=eq;
	exp=e;
	nextIndex=0;
	state=WAIT_OPERAND;
}

bool MathSubExpParser::parse()
{
	//check if string start from unary operator + or -
	if(equation[0]=='-')
	{
		exp->operands.append(new MathExpConstOperand(0.0));
		exp->operations.append(&MathSubExp::sub);
		++nextIndex;
	}
	else if(equation[0]=='+')
	{
		exp->operands.append(new MathExpConstOperand(0.0));
		exp->operations.append(&MathSubExp::add);
		++nextIndex;
	}
	bool endOfParsing=false;
	while(!endOfParsing)
	{
		QChar c;
		if(nextIndex>=equation.length())
			c=QChar(0);
		else c=equation[nextIndex];
		++nextIndex;//после этого функции парсинга могут поменять индекс на нужный им
		if(c.isSpace())continue;
		switch(state)
		{
		case WAIT_OPERAND:
			if(!parseCharWaitOperand(c))return false;
			break;
		case WAIT_SIMPLE_OPERATION:
			if(!parseCharWaitSimpleOperation(c))return false;
			break;
		case READ_VAR_OR_FUNCTION_NAME:
			if(!parseCharReadVarOrFunctionName(c))return false;
			break;
		case READ_NUM:
		case READ_NUM_AFTER_E:
		case READ_NUM_WAS_E:
		case READ_NUM_WAS_DOT:
			if(!parseCharReadNum(c))return false;
			break;
		case END_OF_PARSING:
			endOfParsing=true;
			break;
		}
	}

	return true;
}

bool MathSubExpParser::parseCharWaitOperand(QChar c)
{
	if(c.isDigit())//начинается число
	{
		currentReadWord.append(c);
		state=READ_NUM;
	}
	else if(isValidNameChar(c))//начинается название параметра или функции
	{
		currentReadWord.append(c);
		state=READ_VAR_OR_FUNCTION_NAME;
	}
	else if(c=='(')//начинается подвыражение в скобках
	{
		int closeBracketPos=0;
		if(!findBracketsContent(nextIndex,closeBracketPos))
			return false;
		QString expr=equation.mid(nextIndex,closeBracketPos-nextIndex);
		if(expr.isEmpty())
			return false;
		nextIndex=closeBracketPos+1;
		MathExpSubExpOperand *o=new MathExpSubExpOperand;
		if(!o->exp->parse(expr,vars))
		{
			delete o;
			return false;
		}
		exp->operands.append(o);
		state=WAIT_SIMPLE_OPERATION;
	}
	else return false;
	return true;
}

bool MathSubExpParser::parseCharWaitSimpleOperation(QChar c)
{
	if(isSimpleOperationChar(c))//is operation
		addOp(c);
	else if(c.isNull())//end of equation
		state=END_OF_PARSING;
	else return false;
	return true;
}

bool MathSubExpParser::parseCharReadVarOrFunctionName(QChar c)
{
	if(c.isDigit()||isValidNameChar(c))//continue name parsing
		currentReadWord.append(c);
	else if(isSimpleOperationChar(c)||c.isNull())//end of var
	{
		if(!vars.contains(currentReadWord))
			return false;
		exp->operands.append(new MathExpVarOperand(vars[currentReadWord]));
		currentReadWord.clear();
		if(c.isNull())//last operand in equation
			state=END_OF_PARSING;
		else addOp(c);
	}
	else if(c=='(')//start of function params
	{
		//check functions by name?
		QString funcName=currentReadWord;
		MathExpIFunction *func=MathExpBuiltinFunctions::inst().func(funcName);
		if(!func)return false;
		QStringList args;
		int closeBracketPos;
		if(!findFunctionArgs(++nextIndex,args,closeBracketPos))
			return false;
		if(args.count()!=func->argsCount())return false;
		MathExpFuncOperand *o=new MathExpFuncOperand;
		o->func=func;
		for(QString &a:args)
		{
			MathSubExp *e=new MathSubExp;
			o->args.append(e);
			if(!e->parse(a,vars))
			{
				delete o;
				return false;
			}
		}
		exp->operands.append(o);
		state=WAIT_SIMPLE_OPERATION;
	}
	else return false;
	return true;
}

bool MathSubExpParser::parseCharReadNum(QChar c)
{
	if(c.isDigit())
		currentReadWord.append(c);
	else if(c=='.')//'.' char for a floating-point number
	{
		if(state!=READ_NUM)//second '.' in one number or '.' after 'e'
			return false;
		currentReadWord.append(c);
		state=READ_NUM_WAS_DOT;
	}
	else if(c=='e')//'e' notaion of a floating-point number (ex.: 0.1e3)
	{
		if(state!=READ_NUM&&state!=READ_NUM_WAS_DOT)//second 'e' in one number
			return false;
		currentReadWord.append(c);
		state=READ_NUM_AFTER_E;
	}
	else if(isSimpleOperationChar(c)||c.isNull())
	{
		if(state==READ_NUM_AFTER_E)//sign after 'e' char (ex.: 0.1e-12, 0.2e+3)
		{
			if(c=='*'||c=='/')return false;//'*' or '/' operation just after 'e' char
			if(c=='-')currentReadWord.append(c);//'+' can be avoided
			state=READ_NUM_WAS_E;
		}
		else//end of num or end of equation
		{
			bool ok=false;
			double v=currentReadWord.toUtf8().toDouble(&ok);
			currentReadWord.clear();
			if(!ok)return false;
			exp->operands.append(new MathExpConstOperand(v));
			if(!c.isNull())
			{
				state=WAIT_OPERAND;
				addOp(c);
			}
			else state=END_OF_PARSING;
		}
	}
	else return false;
	return true;
}

bool MathSubExpParser::findBracketsContent(int startFrom,int &closeBracketIndex)
{
	int bracketLvl=1;
	for(int i=startFrom;i<equation.length();++i)
	{
		QChar c=equation[i];
		if(c=='(')
			++bracketLvl;
		else if(c==')')
		{
			--bracketLvl;
			if(bracketLvl==0)
			{
				closeBracketIndex=i;
				return true;
			}
		}
	}
	return false;
}

bool MathSubExpParser::findFunctionArgs(int startFrom,QStringList &args,int &closeBracketIndex)
{
	args.clear();
	QString arg;
	int bracketLvl=1;
	for(int i=startFrom;i<equation.length();++i)
	{
		QChar c=equation[i];
		if(c==','&&bracketLvl==1)
		{
			if(arg.isEmpty())
				return false;
			args.append(arg);
			continue;
		}
		arg.append(c);
		if(c=='(')
			++bracketLvl;
		else if(c==')')
		{
			--bracketLvl;
			if(bracketLvl==0)
			{
				if(arg.isEmpty())
					return false;
				args.append(arg);
				closeBracketIndex=i;
				return true;
			}
		}
	}
	return false;
}

bool MathSubExpParser::isValidNameChar(QChar c)
{
	QChar lc=c.toLower();
	return lc>QChar('a')&&lc<QChar('z');
}

bool MathSubExpParser::isSimpleOperationChar(QChar c)
{
	return c=='+'||c=='-'||c=='*'||c=='/';
}

void MathSubExpParser::addOp(QChar c)
{
	if(c=='+')
		exp->operations.append(&MathSubExp::add);
	else if(c=='-')
		exp->operations.append(&MathSubExp::sub);
	else if(c=='*')
		exp->operations.append(&MathSubExp::mult);
	else if(c=='/')
		exp->operations.append(&MathSubExp::div);
	state=WAIT_OPERAND;
}

MathExpConstOperand::MathExpConstOperand(double v)
{
	val=v;
}

double MathExpConstOperand::calc()
{
	return val;
}

MathExpSubExpOperand::MathExpSubExpOperand()
{
	exp=new MathSubExp;
}

MathExpSubExpOperand::~MathExpSubExpOperand()
{
	delete exp;
}

double MathExpSubExpOperand::calc()
{
	return exp->calc();
}

void MathExpSubExpOperand::checkOpsForPriorities()
{
	exp->checkOpsForPriorities();
}

MathExpFuncOperand::~MathExpFuncOperand()
{
	for(auto p:args)
		delete p;
}

double MathExpFuncOperand::calc()
{
	return func->calc(args);
}

void MathExpFuncOperand::checkOpsForPriorities()
{
	for(MathSubExp *e:args)
		e->checkOpsForPriorities();
}

MathExpVarOperand::MathExpVarOperand(double *v)
{
	val=v;
}

double MathExpVarOperand::calc()
{
	return *val;
}

void MathExpIOperand::checkOpsForPriorities()
{
}
