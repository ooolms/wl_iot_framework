#include "ShowLogDialog.h"
#include <QTextBrowser>
#include <QLayout>
#include <QPushButton>

ShowLogDialog::ShowLogDialog(const QString &html,QWidget *parent)
	:QDialog(parent)
{
	QTextBrowser *view=new QTextBrowser(this);
	view->setHtml(html);
	view->setReadOnly(true);
	QPushButton *closeBtn=new QPushButton("Close",this);

	QVBoxLayout *mainLayout=new QVBoxLayout(this);
	mainLayout->addWidget(view,1);
	mainLayout->addWidget(closeBtn);

	connect(closeBtn,&QPushButton::clicked,this,&ShowLogDialog::reject);
}
