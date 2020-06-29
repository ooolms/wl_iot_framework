#include "DevicePresenceSourceBlockEditorWidget.h"
#include "ui_DevicePresenceSourceBlockEditorWidget.h"
#include "wliot/devices/ControlsDefinition.h"

using namespace WLIOT;
using namespace WLIOTVDIL;

DevicePresenceSourceBlockEditorWidget::DevicePresenceSourceBlockEditorWidget(IEditorHelper *helper,QWidget *parent)
	:QWidget(parent)
{
	mHelper=helper;
	ui=new Ui::DevicePresenceSourceBlockEditorWidget;
	ui->setupUi(this);
	connect(ui->selectDevBtn,&QPushButton::clicked,this,&DevicePresenceSourceBlockEditorWidget::onSelectDevClicked);
}

DevicePresenceSourceBlockEditorWidget::~DevicePresenceSourceBlockEditorWidget()
{
	delete ui;
}

void DevicePresenceSourceBlockEditorWidget::setParams(const QUuid &devId,const QString &devName)
{
	ui->deviceIdEdit->setText(devId.toString());
	ui->deviceNameEdit->setText(devName);
}

QUuid DevicePresenceSourceBlockEditorWidget::deviceId()const
{
	return QUuid(ui->deviceIdEdit->text());
}

void DevicePresenceSourceBlockEditorWidget::onSelectDevClicked()
{
	QUuid newId;
	QString newName;
	ControlsGroup ctls;
	if(!mHelper->selectDevice(newId,newName,ctls))return;
	if(newId.isNull())return;
	ui->deviceIdEdit->setText(newId.toString());
	ui->deviceNameEdit->setText(newName);
}
