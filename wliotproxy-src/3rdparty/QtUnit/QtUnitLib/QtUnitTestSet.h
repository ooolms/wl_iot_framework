#ifndef QTUNITTESTSET_H
#define QTUNITTESTSET_H

#include <QList>
#include <QObject>

/*TODO добавить требования к окружению для набора тестов + иконка в UI, показывающая наличие требований +
 * возможность посмотреть в UI требования к отдельному набору тестов и ко всем имеющимся наборам
*/
//TODO попырецца на assert (можно будет делать стэк-трейсы?)
namespace QtUnit
{
	class QtUnitTestSet;

	#define FAIL(msg) {result=false;\
		summaryMessage=QString("Failed: ")+\
			QString(__FILE__)+":"+QString::number(__LINE__)+\
			QString(" - ")+(msg);\
		return;}
	#define SUCCESS_MSG(msg) {result=true;\
		summaryMessage=QString("Info: ")+\
			QString(__FILE__)+":"+QString::number(__LINE__)+\
			QString(" - ")+(msg);\
		return;}
	#define VERIFY(equ) if(!(equ))\
		{result=false;\
		summaryMessage=QString("Verify failed: ")+\
			QString(__FILE__)+":"+QString::number(__LINE__)+\
			QString(" - equantion "#equ" is false");\
		return;}
	#define VERIFY_MESSAGE(equ,msg) if(!(equ))\
		{result=false;\
		summaryMessage=QString("Verify failed: ")+\
			QString(__FILE__)+":"+QString::number(__LINE__)+\
			QString(" - ")+(msg);\
		return;}
	#define COMPARE(v1,v2) if(!((v1)==(v2)))\
		{result=false;\
		summaryMessage=QString("Compare failed: ")+\
			QString(__FILE__)+":"+QString::number(__LINE__)+\
			QString(" - values "#v1" and "#v2" are not equal");\
		return;}
	#define COMPARE_MESSAGE(v1,v2,msg) if(!((v1)==(v2)))\
		{result=false;\
		summaryMessage=QString("Compare failed: ")+\
			QString(__FILE__)+":"+QString::number(__LINE__)+\
			QString(" - ")+(msg);\
		return;}
	#define COMPARE_NE(v1,v2) if(((v1)==(v2)))\
		{result=false;\
		summaryMessage=QString("CompareNE failed: ")+\
			QString(__FILE__)+":"+QString::number(__LINE__)+\
			QString(" - values "#v1" and "#v2" are equal");\
		return;}
	#define COMPARE_NE_MESSAGE(v1,v2,msg) if(!((v1)==(v2)))\
		{result=false;\
		summaryMessage=QString("Compare failed: ")+\
			QString(__FILE__)+":"+QString::number(__LINE__)+\
			QString(" - ")+(msg);\
		return;}

	//#define QT_UNIT_ADD_TEST(func,name) addTest((TestFunction)func,name)

	class QtUnitTestSet
		:public QObject
	{
		Q_OBJECT
	public:
		typedef void (QtUnitTestSet::*TestFunction)();
		struct TestDescr
		{
			int id;
			QString name;
			TestFunction func;
			bool result;
			QString message;
			QString log;

			TestDescr()
			{
				id=-1;
				func=0;
			}
		};

	public:
		explicit QtUnitTestSet(QObject *parent=0);
		QtUnitTestSet(const QString &name,QObject *parent=0);
		virtual ~QtUnitTestSet();

	public:
		int addTest(TestFunction func,const QString &testName);//returns id
		void runTests();
		const QList<TestDescr>& tests();
		const TestDescr* test(int id);
		bool hasTestFails();
		bool hasInitFails();
		bool processed();
		void skipTestSet();
		void runSingleTest(int index);

	public:
		virtual bool init();
		virtual void cleanup();
		virtual bool testInit();
		virtual void testCleanup();

	signals:
		void testSetBegin();
		void testBegin(int testId);
		void testEnd(int testId,bool result,const QString &message,const QString &log);
		void testSetEnd();
		void testSetSkipped();

	protected:
		void writeLogMessage(const QString &html);

	private:
		void runTest(int i);

	public:
		QString testSetName;

	protected:
		bool result;
		bool failsWhileRunning;
		bool initFails;
		QString summaryMessage;

	private:
		QList<TestDescr> testsList;
		QString testLog;
		static int nextId;
		bool wasProcessed;
	};
}

#endif // QTUNITTESTSET_H
