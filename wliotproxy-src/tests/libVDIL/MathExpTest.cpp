#include "MathExpTest.h"
#include "VDIL/support/MathExp.h"
#include <QtMath>

using namespace QtUnit;
using namespace WLIOTVDIL;

MathExpTest::MathExpTest(QObject *parent)
	:QtUnit::QtUnitTestSet("MathExpTest",parent)
{
	addTest((TestFunction)&MathExpTest::testOperationsPriority,"Test check operation priorities");
	addTest((TestFunction)&MathExpTest::testOperationsPriority,"Test parse with functions");
	addTest((TestFunction)&MathExpTest::testExceptionHandling,"Test exception handling");
}

void MathExpTest::testOperationsPriority()
{
	//2+2*2*2-2*2 - valid 6, invalid 28
	MathSubExp e;
	e.operands.append(new MathExpConstOperand(2));
	e.operations.append(&MathSubExp::add);
	e.operands.append(new MathExpConstOperand(2));
	e.operations.append(&MathSubExp::mult);
	e.operands.append(new MathExpConstOperand(2));
	e.operations.append(&MathSubExp::mult);
	e.operands.append(new MathExpConstOperand(2));
	e.operations.append(&MathSubExp::sub);
	e.operands.append(new MathExpConstOperand(2));
	e.operations.append(&MathSubExp::mult);
	e.operands.append(new MathExpConstOperand(2));

	COMPARE(e.calc(),28.0)
	e.checkOpsForPriorities();
	COMPARE(e.calc(),6.0)
}

void MathExpTest::testParseWithFunctions()
{
	MathExp exp;
	QMap<QString,double*> vars;
	double x=34;
	vars["x"]=&x;
	VERIFY(exp.parse("10-sin(3)+(x-7*2)-pow(2.0,1)",vars))
	double res=0;
	VERIFY(exp.calc(res))
	VERIFY(qFabs(res-27.859)<0.01)

	VERIFY(exp.parse("(x+10)/25",vars))

	VERIFY(exp.parse("10*sin(exp(x))",vars))
}

void MathExpTest::testExceptionHandling()
{
	MathExp exp;
	QMap<QString,double*> vars;
	VERIFY(exp.parse("10/0",vars))
	double r;
	VERIFY(!exp.calc(r))
}
