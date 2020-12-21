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

#ifndef MATHEQUATION_H
#define MATHEQUATION_H

#include <QString>
#include <QMap>

namespace WLIOTVDIL
{
	class MathSubExp;

	enum class MathExpSimpleOperation
	{
		ADD,
		SUBSTR,
		MULT,
		DIV
	};

	class MathExpIOperand
	{
	public:
		virtual ~MathExpIOperand(){}
		virtual double calc()=0;
		virtual void checkOpsForPriorities();
	};

	class MathExpIFunction
	{
	public:
		virtual ~MathExpIFunction(){}
		virtual int argsCount()=0;
		virtual double calc(QList<MathSubExp*> &argsExpList)=0;
	};

	class MathExpConstOperand
		:public MathExpIOperand
	{
	public:
		explicit MathExpConstOperand(double v);
		virtual double calc()override;

	public:
		double val;
	};

	class MathExpVarOperand
		:public MathExpIOperand
	{
	public:
		explicit MathExpVarOperand(double *v);
		virtual double calc()override;

	public:
		double *val;
	};

	class MathExpSubExpOperand
		:public MathExpIOperand
	{
	public:
		explicit MathExpSubExpOperand();
		virtual ~MathExpSubExpOperand();
		virtual double calc()override;
		virtual void checkOpsForPriorities()override;

	public:
		MathSubExp *exp;
	};

	class MathExpFuncOperand
		:public MathExpIOperand
	{
	public:
		virtual ~MathExpFuncOperand();
		virtual double calc()override;
		virtual void checkOpsForPriorities()override;

	public:
		MathExpIFunction *func;
		QList<MathSubExp*> args;
	};

	class MathSubExp
	{
	public:
		typedef double (*SimpleOperation)(double v1,double v2);

	public:
		~MathSubExp();
		bool parse(QString eq,const QMap<QString,double*> &varsMap);
		void checkOpsForPriorities();
		double calc();
		void clear();

	public:
		static double add(double v1,double v2);
		static double sub(double v1,double v2);
		static double mult(double v1,double v2);
		static double div(double v1,double v2);

	public:
		QList<MathExpIOperand*> operands;
		QList<SimpleOperation> operations;
	};

	class MathSubExpParser
	{
	private:
		enum ParserState
		{
			WAIT_OPERAND,
			WAIT_SIMPLE_OPERATION,
			READ_VAR_OR_FUNCTION_NAME,
			READ_NUM,
			READ_NUM_WAS_DOT,
			READ_NUM_AFTER_E,
			READ_NUM_WAS_E,
			END_OF_PARSING
		};

	public:
		explicit MathSubExpParser(MathSubExp *e,const QString &eq,const QMap<QString,double*> &varsMap);
		bool parse();

	private:
		//state functions
		bool parseCharWaitOperand(QChar c);
		bool parseCharWaitSimpleOperation(QChar c);
		bool parseCharReadVarOrFunctionName(QChar c);
		bool parseCharReadNum(QChar c);

		//support
		bool findBracketsContent(int startFrom,int &closeBracketIndex);
		bool findFunctionArgs(int startFrom,QStringList &args,int &closeBracketIndex);

		//char analyze
		bool isValidNameChar(QChar c);
		bool isSimpleOperationChar(QChar c);
		void addOp(QChar c);

	private:
		QString equation;
		const QMap<QString,double*> &vars;
		MathSubExp *exp;
		int nextIndex;
		QString currentReadWord;
		ParserState state;
	};

	class MathExp
	{
	public:
		bool parse(const QString &eq,const QMap<QString,double*> &varsMap);
		void clear();
		bool calc(double &result);

	private:
		MathSubExp mainSubExpr;
		QList<double*> mConstantsList;
	};
}

#endif // MATHEQUATION_H
