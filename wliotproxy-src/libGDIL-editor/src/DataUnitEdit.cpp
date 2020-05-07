#include "GDIL/editor/DataUnitEdit.h"
#include "DataUnitValueEdit.h"
#include "ui_DataUnitEdit.h"

DataUnitEdit::DataUnitEdit(const TypeConstraints &c,QWidget *parent)
	:QWidget(parent)
{
	mConstr=c;
	ui=new Ui::DataUnitEdit;
	ui->setupUi(this);
	singleValueEdit=new DataUnitValueEdit(ui->singleWidget);
	((QVBoxLayout*)ui->singleWidget->layout())->insertWidget(1,singleValueEdit);
	ui->arrayValuesList->setDragDropMode(QAbstractItemView::InternalMove);
	connect(ui->boolBtn,&QPushButton::clicked,this,&DataUnitEdit::onTypeBtnClicked);
	connect(ui->singleBtn,&QPushButton::clicked,this,&DataUnitEdit::onTypeBtnClicked);
	connect(ui->arrayBtn,&QPushButton::clicked,this,&DataUnitEdit::onTypeBtnClicked);
	connect(ui->arrayAddRowBtn,&QPushButton::clicked,this,&DataUnitEdit::onArrAddRowClicked);
	connect(ui->arrayDelRowBtn,&QPushButton::clicked,this,&DataUnitEdit::onArrDelRowClicked);

	if(!(mConstr.types&DataUnit::SINGLE))
		ui->singleBtn->setEnabled(false);
	if(!(mConstr.types&DataUnit::ARRAY))
		ui->arrayBtn->setEnabled(false);
	if(!(mConstr.types&DataUnit::BOOL))
		ui->boolBtn->setEnabled(false);

	if(mConstr.dim!=0)
	{
		ui->dimEdit->setValue(mConstr.dim);
		ui->dimEdit->setEnabled(false);
	}
}

DataUnitEdit::~DataUnitEdit()
{
	delete ui;
}

DataUnit DataUnitEdit::value()const
{
	if(ui->arrayBtn->isChecked())
	{
		if(ui->arrayValuesList->count()==0)
			return DataUnit(DataUnit::ARRAY,1);
		QString s=ui->arrayValuesList->item(0)->text();
		QByteArrayList b=s.toUtf8().split(';');
		if(b.isEmpty())
			return DataUnit(0.0);
		SensorDef::Type t;
		t.dim=b.count();
		t.numType=SensorDef::F64;
		t.packType=SensorDef::PACKET;
		for(int i=0;i<ui->arrayValuesList->count();++i)
		{
			s=ui->arrayValuesList->item(i)->text();
			QByteArrayList bb=s.toUtf8().split(';');
			if(bb.size()!=(int)t.dim)
				return DataUnit(DataUnit::ARRAY,1);
			b.append(bb);
		}
		QScopedPointer<SensorValue> v(SensorValue::createSensorValue(t));
		if(!v->parseMsgArgs(b))
			return DataUnit(DataUnit::ARRAY,1);
		return DataUnit(v.data());
	}
	else if(ui->singleBtn->isChecked())
	{
		quint32 dim=ui->dimEdit->value();
		DataUnit u;
		if(singleValueEdit->canBeS64())
		{
			u=DataUnit(DataUnit::SINGLE,dim);

		}
		return u;

/*		QString s=ui->singleValueEdit->text();
		QByteArrayList b=s.toUtf8().split(';');
		if(b.isEmpty())
			return DataUnit(0.0);
		SensorDef::Type t;
		t.dim=b.count();
		if(s.contains('.')||s.contains('e',Qt::CaseInsensitive))
			t.numType=SensorDef::F64;
		else t.numType=SensorDef::S64;
		QScopedPointer<SensorValue> v(SensorValue::createSensorValue(t));
		if(!v->parseMsgArgs(b))
			return DataUnit(0.0);
		return DataUnit(v.data());*/
	}
	else
		return DataUnit(ui->boolTrueCheck->isChecked());
}

void DataUnitEdit::setValue(const DataUnit &v)
{
	if(!mConstr.match(v))
		return;
	ui->boolBtn->setChecked(true);
	singleValueEdit->setDim(0);
	ui->arrayValuesList->clear();
	if(v.type()==DataUnit::BOOL)
	{
		ui->boolBtn->setChecked(true);
		ui->typeEditorsSelect->setCurrentWidget(ui->boolWidget);
		ui->boolTrueCheck->setChecked(v.value()->valueToS64(0)==1);
	}
	else if(v.type()==DataUnit::SINGLE)
	{
		ui->singleBtn->setChecked(true);
		ui->typeEditorsSelect->setCurrentWidget(ui->singleWidget);
//		ui->singleValueEdit->setText(valToStr(v.value(),0));
		//TODO
	}
	else if(v.type()==DataUnit::ARRAY)
	{
		ui->arrayBtn->setChecked(true);
		ui->typeEditorsSelect->setCurrentWidget(ui->arrayWidget);
		for(quint32 i=0;i<v.value()->packetsCount();++i)
		{
			QListWidgetItem *item=new QListWidgetItem(valToStr(v.value(),i),ui->arrayValuesList);
			item->setFlags(item->flags()|Qt::ItemIsEditable);
		}
	}
}

void DataUnitEdit::onTypeBtnClicked()
{
	if(ui->boolBtn->isChecked())
		ui->typeEditorsSelect->setCurrentWidget(ui->boolWidget);
	else if(ui->singleBtn->isChecked())
		ui->typeEditorsSelect->setCurrentWidget(ui->singleWidget);
	else ui->typeEditorsSelect->setCurrentWidget(ui->arrayWidget);
	if(mConstr.dim==0)
		ui->dimEdit->setEnabled(!ui->boolBtn->isChecked());
}

void DataUnitEdit::onArrAddRowClicked()
{
	QListWidgetItem *item=new QListWidgetItem(ui->arrayValuesList);
	item->setFlags(item->flags()|Qt::ItemIsEditable);
	ui->arrayValuesList->editItem(item);
}

void DataUnitEdit::onArrDelRowClicked()
{
	if(!ui->arrayValuesList->selectedItems().contains(ui->arrayValuesList->currentItem()))
		return;
	delete ui->arrayValuesList->currentItem();
}

QString DataUnitEdit::valToStr(const SensorValue *v,quint32 packIndex)
{
	QString retVal;
	for(quint32 i=0;i<v->type().dim;++i)
	{
		retVal+=QString::fromUtf8(v->valueToString(i,packIndex));
		retVal+=",";
	}
	retVal.chop(1);
	return retVal;
}
