#include "DeviceStateSourceBlockEditorWidget.h"
#include "ui_DeviceStateSourceBlockEditorWidget.h"

DeviceStateSourceBlockEditorWidget::DeviceStateSourceBlockEditorWidget(EditorInternalApi *ed,QWidget *parent)
	:QWidget(parent)
{
	ui=new Ui::DeviceStateSourceBlockEditorWidget;
	ui->setupUi(this);
	editor=ed;
	connect(ui->deviceSelectBtn,&QPushButton::clicked,this,&DeviceStateSourceBlockEditorWidget::onSelectDeviceClicked);
	connect(ui->numOutBtn,&QPushButton::toggled,this,&DeviceStateSourceBlockEditorWidget::onNumBtnStateChanged);
}

DeviceStateSourceBlockEditorWidget::~DeviceStateSourceBlockEditorWidget()
{
	delete ui;
}

void DeviceStateSourceBlockEditorWidget::setParams(const QUuid &devId,const QString &devName,
	const QByteArray &stateKey,bool cmdState,bool boolOut,quint32 cmdStateIndex)
{
	ui->deviceIdEdit->setText(devId.toString());
	ui->deviceNameEdit->setText(devName);
	ui->stateKeyEdit->setText(QString::fromUtf8(stateKey));
	if(cmdState)
		ui->cmdStateBtn->setChecked(true);
	else ui->additionalParamBtn->setChecked(true);
	if(boolOut)
		ui->boolOutBtn->setChecked(true);
	else ui->numOutBtn->setChecked(true);
	ui->cmdParamIndexEdit->setValue(cmdStateIndex);
}

QUuid DeviceStateSourceBlockEditorWidget::deviceId() const
{
	return QUuid(ui->deviceIdEdit->text());
}

QByteArray DeviceStateSourceBlockEditorWidget::stateKey() const
{
	return ui->stateKeyEdit->text().toUtf8();
}

bool DeviceStateSourceBlockEditorWidget::commandState() const
{
	return ui->cmdStateBtn->isChecked();
}

bool DeviceStateSourceBlockEditorWidget::boolOut() const
{
	return ui->boolOutBtn->isChecked();
}

quint32 DeviceStateSourceBlockEditorWidget::commandStateIndex()const
{
	return ui->cmdParamIndexEdit->value();
}

void DeviceStateSourceBlockEditorWidget::onSelectDeviceClicked()
{
	QUuid newId;
	QString newName;
	ControlsGroup ctls;
	editor->selectDevice(newId,newName,ctls);
	if(newId.isNull())return;
	ui->deviceIdEdit->setText(newId.toString());
	ui->deviceNameEdit->setText(newName);
}

void DeviceStateSourceBlockEditorWidget::onNumBtnStateChanged()
{
	ui->cmdParamIndexEdit->setEnabled(ui->cmdStateBtn->isChecked());
}