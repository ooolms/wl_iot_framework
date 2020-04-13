#include <QApplication>
#include "GDIL/core/Program.h"
#include "GDIL/core/BlockOutput.h"
#include "GDIL/blocks/AllBlocks.h"
#include "GDIL/xml/ProgramXmlParser.h"
#include "GDIL/xml/BlocksXmlParserFactory.h"
#include "GDIL/editor/BlocksEditingFactory.h"
#include "GDIL/core/BlocksFactory.h"
#include "GDIL/editor/Editor.h"
#include "StdQFile.h"
#include "MainWindow.h"
#include <QDebug>

void myMessageHandler(QtMsgType t,const QMessageLogContext &,const QString &s)
{
	QByteArray m=s.toUtf8();
	if(t==QtDebugMsg||t==QtInfoMsg)
	{
		StdQFile::stdout()->write(m.constData(),m.size());
		StdQFile::stdout()->write("\n",1);
		StdQFile::stdout()->flush();
	}
	else
	{
		StdQFile::stderr()->write(m.constData(),m.size());
		StdQFile::stderr()->write("\n",1);
		StdQFile::stderr()->flush();
	}
}

int main(int argc,char *argv[])
{
	QApplication app(argc,argv);
	qInstallMessageHandler(&myMessageHandler);
	/*Program p;
	BlocksXmlParserFactory f;
	BlocksFactory bf;
	StaticSourceBlock *sb1=new StaticSourceBlock;
	sb1->setValue(DataUnit(12.0));
	sb1->title="src1";
	sb1->position=QPointF(0,0);
	StaticSourceBlock *sb2=new StaticSourceBlock;
	sb2->setValue(DataUnit(15.0));
	sb2->title="src2";
	sb2->position=QPointF(0,200);
	ComparationBlock *gtb=new ComparationBlock;
	gtb->setParams(false,0,ComparationBlock::GT);
	gtb->title="compare";
	gtb->position=QPointF(200,100);
	CommandBlock *cb=new CommandBlock;
	cb->setParams(QUuid("{e46b18fc-94ef-465b-86b6-fbb907f0aa0f}"),"some_dev","flow",QByteArrayList()<<"11",1);
	cb->title="exec if gt";
	cb->position=QPointF(400,100);
	p.addBlock(sb1);
	p.addBlock(sb2);
	p.addBlock(gtb);
	p.addBlock(cb);
	sb1->output(0)->linkTo(gtb->input(0));
	sb2->output(0)->linkTo(gtb->input(1));
	gtb->output(0)->linkTo(cb->input(0));
	QByteArray xml=ProgramXmlParser::toXml(&f,&p);
	qDebug().noquote()<<QString::fromUtf8(xml);
	Program *pp=ProgramXmlParser::fromXml(&f,&bf,xml);
	qDebug()<<(pp?"ok":"err");
	if(pp)delete pp;
	BlocksEditingFactory bef;*/
	MainWindow w;
//	w.editor()->setProgram(xml);
	w.showMaximized();
	return app.exec();
}
