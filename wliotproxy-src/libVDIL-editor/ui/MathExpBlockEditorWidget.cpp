#include "MathExpBlockEditorWidget.h"
#include "ui_MathExpBlockEditorWidget.h"
#include "VDIL/support/MathExp.h"
#include <QStatusBar>

using namespace WLIOTVDIL;

MathExpBlockEditorWidget::MathExpBlockEditorWidget(QWidget *parent)
	:QWidget(parent)
{
	ui=new Ui::MathExpBlockEditorWidget;
	ui->setupUi(this);
	testStatus=new QStatusBar(ui->toolbar);
	ui->toolbar->layout()->addWidget(testStatus);
	connect(ui->testExprBtn,&QPushButton::clicked,this,&MathExpBlockEditorWidget::onTestExprClicked);
}

MathExpBlockEditorWidget::~MathExpBlockEditorWidget()
{
	delete ui;
}

void MathExpBlockEditorWidget::setParams(const QStringList &vars,const QString &expr)
{
	ui->varsEdit->setText(vars.join(','));
	ui->exprEdit->setPlainText(expr);
}

QString MathExpBlockEditorWidget::expr()
{
	return ui->exprEdit->toPlainText();
}

QStringList MathExpBlockEditorWidget::vars()
{
	QStringList vars=ui->varsEdit->text().split(',');
	for(QString &s:vars)
		s=s.trimmed();
	vars.removeAll(QString());
	return vars;
}

void MathExpBlockEditorWidget::onTestExprClicked()
{
	QStringList v=vars();
	QMap<QString,double*> t;
	for(const QString &s:v)
		t[s]=new double;
	MathExp e;
	if(!e.parse(expr(),t))
		testStatus->showMessage("Failed",5000);
	else testStatus->showMessage("Ok",5000);
	for(auto d:t)
		delete d;
}
