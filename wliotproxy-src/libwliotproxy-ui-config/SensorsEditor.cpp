#include "SensorsEditor.h"
#include "ui_SensorsEditor.h"

using namespace WLIOT;
using namespace WLIOTUi;

static const int roleSensorType=Qt::UserRole;
static const int roleSensorAttributes=Qt::UserRole+1;

SensorsEditor::SensorsEditor(QWidget *parent)
	:QWidget(parent)
{
	construct(QList<SensorDef::NumType>()<<SensorDef::F32<<SensorDef::F64<<SensorDef::S8<<SensorDef::U8<<
		SensorDef::S16<<SensorDef::U16<<SensorDef::S32<<SensorDef::U32<<SensorDef::S64<<SensorDef::U64<<
	SensorDef::TEXT,true,true);
}

SensorsEditor::SensorsEditor(const QList<SensorDef::NumType> &numTypes,
	bool editTime,bool allowLocalTime,QWidget *parent)
	:QWidget(parent)
{
	construct(numTypes,editTime,allowLocalTime);
}

SensorsEditor::~SensorsEditor()
{
	delete ui;
}

void SensorsEditor::setSensors(const QList<SensorDef> &sensors)
{
	buildSensorsList(sensors);
}

QList<SensorDef> SensorsEditor::sensors()
{
	saveCurrentEditedSensorsItem();
	QList<SensorDef> sensors;
	dumpSensors(sensors);
	return sensors;
}

void SensorsEditor::onSensorsTreeSelChanged()
{
	saveCurrentEditedSensorsItem();
	currentEditedSensorsItem=0;
	if(!ui->sensorsTree->selectedItems().contains(ui->sensorsTree->currentItem()))
	{
		ui->typeGroup->setEnabled(false);
		return;
	}
	QListWidgetItem *item=ui->sensorsTree->currentItem();
	currentEditedSensorsItem=item;
	ui->typeGroup->setEnabled(true);

	SensorDef::Type type=item->data(roleSensorType).value<SensorDef::Type>();
	if(type.packType==SensorDef::SINGLE)
		ui->singleValueBtn->setChecked(true);
	else ui->packetValueBtn->setChecked(true);

	if(mEditTime)
	{
		if(type.tsType==SensorDef::GLOBAL_TIME)
			ui->globalTimeBtn->setChecked(true);
		else if(type.tsType==SensorDef::LOCAL_TIME&&mAllowLocalTime)
			ui->localTimeBtn->setChecked(true);
		else ui->noTimeBtn->setChecked(true);
	}

	ui->valTypeSelect->setCurrentIndex(0);
	for(int i=0;i<ui->valTypeSelect->count();++i)
	{
		if(ui->valTypeSelect->itemData(i).toInt()==(int)type.numType)
		{
			ui->valTypeSelect->setCurrentIndex(i);
			break;
		}
	}
	ui->dimEdit->setValue((int)type.dim);
}

void SensorsEditor::onAddClicked()
{
	QListWidgetItem *item=new QListWidgetItem(ui->sensorsTree);
	item->setSelected(true);
	item->setFlags(item->flags()|Qt::ItemIsEditable);
	SensorDef::Type t;
	t.numType=(SensorDef::NumType)ui->valTypeSelect->itemData(0,Qt::UserRole).toInt();
	item->setData(roleSensorType,QVariant::fromValue(t));
	item->setData(roleSensorAttributes,QVariantMap());
	ui->sensorsTree->clearSelection();
	ui->sensorsTree->setCurrentItem(item);
	item->setSelected(true);
	ui->sensorsTree->editItem(item);
	onSensorsTreeSelChanged();
}

void SensorsEditor::onDelClicked()
{
	saveCurrentEditedSensorsItem();
	if(!ui->sensorsTree->selectedItems().contains(ui->sensorsTree->currentItem()))return;
	currentEditedSensorsItem=0;
	delete ui->sensorsTree->currentItem();
	onSensorsTreeSelChanged();
}

void SensorsEditor::construct(QList<SensorDef::NumType> numTypes,bool editTime,bool allowLocalTime)
{
	mEditTime=editTime;
	mAllowLocalTime=allowLocalTime;
	if(numTypes.isEmpty())
		numTypes.append(SensorDef::F64);

	ui=new Ui::SensorsEditor;
	ui->setupUi(this);
	ui->typeGroup->setEnabled(false);
	currentEditedSensorsItem=0;
	if(numTypes.contains(SensorDef::F32))
		ui->valTypeSelect->addItem("float 32-bit",(int)SensorDef::F32);
	if(numTypes.contains(SensorDef::F64))
		ui->valTypeSelect->addItem("float 64-bit",(int)SensorDef::F64);
	if(numTypes.contains(SensorDef::S8))
		ui->valTypeSelect->addItem("signed int 8-bit",(int)SensorDef::S8);
	if(numTypes.contains(SensorDef::U8))
		ui->valTypeSelect->addItem("unsigned int 8-bit",(int)SensorDef::U8);
	if(numTypes.contains(SensorDef::S16))
		ui->valTypeSelect->addItem("signed int 16-bit",(int)SensorDef::S16);
	if(numTypes.contains(SensorDef::U16))
		ui->valTypeSelect->addItem("unsigned int 16-bit",(int)SensorDef::U16);
	if(numTypes.contains(SensorDef::S32))
		ui->valTypeSelect->addItem("signed int 32-bit",(int)SensorDef::S32);
	if(numTypes.contains(SensorDef::U32))
		ui->valTypeSelect->addItem("unsigned int 32-bit",(int)SensorDef::U32);
	if(numTypes.contains(SensorDef::S64))
		ui->valTypeSelect->addItem("signed int 64-bit",(int)SensorDef::S64);
	if(numTypes.contains(SensorDef::U64))
		ui->valTypeSelect->addItem("unsigned int 64-bit",(int)SensorDef::U64);
	if(numTypes.contains(SensorDef::TEXT))
		ui->valTypeSelect->addItem("text",(int)SensorDef::TEXT);

	if(!mEditTime)
		delete ui->timestampGroup;
	else if(!mAllowLocalTime)
		delete ui->localTimeBtn;

	connect(ui->sensorsTree,&QListWidget::itemSelectionChanged,this,&SensorsEditor::onSensorsTreeSelChanged,
		Qt::QueuedConnection);
	connect(ui->addBtn,&QPushButton::clicked,this,&SensorsEditor::onAddClicked);
	connect(ui->delBtn,&QPushButton::clicked,this,&SensorsEditor::onDelClicked);
}

void SensorsEditor::saveCurrentEditedSensorsItem()
{
	if(!currentEditedSensorsItem)return;
	SensorDef::Type t;

	if(mEditTime)
	{
		if(ui->globalTimeBtn->isChecked())
			t.tsType=SensorDef::GLOBAL_TIME;
		else if(mAllowLocalTime&&ui->localTimeBtn->isChecked())
			t.tsType=SensorDef::LOCAL_TIME;
		else t.tsType=SensorDef::NO_TIME;
	}
	else t.tsType=SensorDef::NO_TIME;

	if(ui->packetValueBtn->isChecked())
		t.packType=SensorDef::PACKET;
	else t.packType=SensorDef::SINGLE;

	t.numType=(SensorDef::NumType)ui->valTypeSelect->currentData().toInt();
	t.dim=(quint32)ui->dimEdit->value();

	currentEditedSensorsItem->setData(roleSensorType,QVariant::fromValue(t));
}

void SensorsEditor::dumpSensors(QList<SensorDef> &sensors)
{
	saveCurrentEditedSensorsItem();
	sensors.clear();
	for(int i=0;i<ui->sensorsTree->count();++i)
	{
		QListWidgetItem *item=ui->sensorsTree->item(i);
		if(item->text().isEmpty())continue;
		SensorDef s;
		s.attributes=item->data(roleSensorAttributes).value<decltype(s.attributes)>();
		s.type=item->data(roleSensorType).value<SensorDef::Type>();
		s.name=item->text().toUtf8();
		sensors.append(s);
	}
}

void SensorsEditor::buildSensorsList(const QList<SensorDef> &sensors)
{
	currentEditedSensorsItem=0;
	ui->sensorsTree->clear();
	for(const SensorDef &s:sensors)
	{
		QListWidgetItem *item=new QListWidgetItem(ui->sensorsTree);
		item->setFlags(item->flags()|Qt::ItemIsEditable);
		item->setText(QString::fromUtf8(s.name));
		item->setData(roleSensorType,QVariant::fromValue(s.type));
		item->setData(roleSensorAttributes,QVariant::fromValue(s.attributes));
	}
	if(!sensors.isEmpty())
	{
		ui->sensorsTree->setCurrentRow(0);
		ui->sensorsTree->item(0)->setSelected(true);
	}
	onSensorsTreeSelChanged();
}
