#include "SubProgramBlockEditorWidget.h"
#include "ui_SubProgramBlockEditorWidget.h"
#include "VDIL/editor/DataUnitTypeEdit.h"
#include <QInputDialog>

using namespace WLIOTVDIL;

SubProgramBlockEditorWidget::SubProgramBlockEditorWidget(QWidget *parent)
	:QWidget(parent)
{
	ui=new Ui::SubProgramBlockEditorWidget;
	ui->setupUi(this);
	ui->inputsList->setHeaderLabels(QStringList()<<"title"<<"type");
	ui->outputsList->setHeaderLabels(QStringList()<<"title"<<"type");

	connect(ui->addInputBtn,&QPushButton::clicked,this,&SubProgramBlockEditorWidget::onAddInputClicked);
	connect(ui->delInputBtn,&QPushButton::clicked,this,&SubProgramBlockEditorWidget::onDelInputClicked);
	connect(ui->addOutputBtn,&QPushButton::clicked,this,&SubProgramBlockEditorWidget::onAddOutputClicked);
	connect(ui->delOutputBtn,&QPushButton::clicked,this,&SubProgramBlockEditorWidget::onDelOutputClicked);
	connect(ui->editSubProgramBtn,&QPushButton::clicked,this,&SubProgramBlockEditorWidget::editContent);
	connect(ui->inputsList,&QTreeWidget::itemDoubleClicked,this,&SubProgramBlockEditorWidget::onInputsListDClicked);
	connect(ui->outputsList,&QTreeWidget::itemDoubleClicked,this,&SubProgramBlockEditorWidget::onOutputsListDClicked);
}

SubProgramBlockEditorWidget::~SubProgramBlockEditorWidget()
{
	delete ui;
}

const QList<TypeAndDim>& SubProgramBlockEditorWidget::inputsTypes()
{
	return mInputsTypes;
}

const QStringList& SubProgramBlockEditorWidget::inputsTitles()
{
	return mInputsTitles;
}

const QList<TypeAndDim>& SubProgramBlockEditorWidget::outputsTypes()
{
	return mOutputsTypes;
}

const QStringList& SubProgramBlockEditorWidget::outputsTitles()
{
	return mOutputsTitles;
}

void SubProgramBlockEditorWidget::setParams(const QList<TypeAndDim> &iTypes,
	const QStringList &iTitles,const QList<TypeAndDim> &oTypes,const QStringList &oTitles)
{
	ui->inputsList->clear();
	ui->outputsList->clear();
	if(iTypes.count()!=iTitles.count())
		return;
	if(oTypes.count()!=oTitles.count())
		return;
	mInputsTypes=iTypes;
	mInputsTitles=iTitles;
	mOutputsTypes=oTypes;
	mOutputsTitles=oTitles;
	for(int i=0;i<iTypes.count();++i)
	{
		QTreeWidgetItem *item=new QTreeWidgetItem(ui->inputsList);
		item->setText(0,iTitles[i]);
		item->setText(1,typeStr(iTypes[i]));
	}
	for(int i=0;i<oTypes.count();++i)
	{
		QTreeWidgetItem *item=new QTreeWidgetItem(ui->outputsList);
		item->setText(0,oTitles[i]);
		item->setText(1,typeStr(oTypes[i]));
	}
}

void SubProgramBlockEditorWidget::onAddInputClicked()
{
	mInputsTypes.append(TypeAndDim(DataUnit::SINGLE,1));
	mInputsTitles.append("input");
	QTreeWidgetItem *item=new QTreeWidgetItem(ui->inputsList);
	item->setText(0,mInputsTitles.last());
	item->setText(1,typeStr(mInputsTypes.last()));
}

void SubProgramBlockEditorWidget::onDelInputClicked()
{
	if(!ui->inputsList->selectedItems().contains(ui->inputsList->currentItem()))return;
	int index=ui->inputsList->indexOfTopLevelItem(ui->inputsList->currentItem());
	if(index==-1)return;
	mInputsTypes.removeAt(index);
	mInputsTitles.removeAt(index);
	delete ui->inputsList->currentItem();
}

void SubProgramBlockEditorWidget::onAddOutputClicked()
{
	mOutputsTypes.append(TypeAndDim(DataUnit::SINGLE,1));
	mOutputsTitles.append("output");
	QTreeWidgetItem *item=new QTreeWidgetItem(ui->outputsList);
	item->setText(0,mOutputsTitles.last());
	item->setText(1,typeStr(mOutputsTypes.last()));
}

void SubProgramBlockEditorWidget::onDelOutputClicked()
{
	if(!ui->outputsList->selectedItems().contains(ui->outputsList->currentItem()))return;
	int index=ui->outputsList->indexOfTopLevelItem(ui->outputsList->currentItem());
	if(index==-1)return;
	mOutputsTypes.removeAt(index);
	mOutputsTitles.removeAt(index);
	delete ui->outputsList->currentItem();
}

void SubProgramBlockEditorWidget::onInputsListDClicked(QTreeWidgetItem *item,int column)
{
	int index=ui->inputsList->indexOfTopLevelItem(item);
	if(index==-1)return;
	if(column==0)
	{
		QString newTitle=QInputDialog::getText(this,"input title","title",QLineEdit::Normal,mInputsTitles[index]);
		if(newTitle.isEmpty())return;
		mInputsTitles[index]=newTitle;
		item->setText(0,newTitle);
	}
	else if(column==1)
	{
		auto dlg=DataUnitTypeEdit::mkDialog(TypeConstraints(DataUnit::ANY,0),this);
		dlg.second->setType(mInputsTypes[index]);
		if(dlg.first->exec()!=QDialog::Accepted)return;
		TypeAndDim t=dlg.second->type();
		mInputsTypes[index]=t;
		item->setText(1,typeStr(t));
		delete dlg.first;
	}
}

void SubProgramBlockEditorWidget::onOutputsListDClicked(QTreeWidgetItem *item,int column)
{
	int index=ui->outputsList->indexOfTopLevelItem(item);
	if(index==-1)return;
	if(column==0)
	{
		QString newTitle=QInputDialog::getText(this,"input title","title",QLineEdit::Normal,mOutputsTitles[index]);
		if(newTitle.isEmpty())return;
		mOutputsTitles[index]=newTitle;
		item->setText(0,newTitle);
	}
	else if(column==1)
	{
		auto dlg=DataUnitTypeEdit::mkDialog(TypeConstraints(DataUnit::ANY,0),this);
		dlg.second->setType(mOutputsTypes[index]);
		if(dlg.first->exec()!=QDialog::Accepted)return;
		TypeAndDim t=dlg.second->type();
		mOutputsTypes[index]=t;
		item->setText(1,typeStr(t));
		delete dlg.first;
	}
}

QString SubProgramBlockEditorWidget::typeStr(const TypeAndDim &t)
{
	if(t.type==DataUnit::SINGLE)
		return "single("+QString::number(t.dim)+")";
	else if(t.type==DataUnit::ARRAY)
		return "array("+QString::number(t.dim)+")";
	else if(t.type==DataUnit::DATETIME)
		return "date+time";
	else return "bool";
}
