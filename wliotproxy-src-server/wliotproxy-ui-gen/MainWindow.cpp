/*******************************************
Copyright 2017 OOO "LMS"

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

	http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.*/

#include "MainWindow.h"
#include "wliot/devices/SensorDef.h"
#include <QMessageBox>
#include <QSharedPointer>
#include <QFileDialog>
#include <QFile>
#include <QDebug>
#include <QClipboard>

static const int roleItemType=Qt::UserRole;
static const int roleValue=Qt::UserRole+1;

static const int roleSensorType=Qt::UserRole;
static const int roleSensorAttributes=Qt::UserRole+1;

static const int itemTypeGroup=1;
static const int itemTypeControl=2;
static const int itemTypeParam=3;

using namespace WLIOT;
using namespace WLIOTUi;

Q_DECLARE_METATYPE(QSharedPointer<ControlsElement>)

MainWindow::MainWindow(QWidget *parent)
	:QMainWindow(parent)
{
	ui.setupUi(this);
	controlsEditor=new ControlsEditor(ui.controlsTab);
	ui.controlsTab->layout()->addWidget(controlsEditor);
	sensorsEditor=new SensorsEditor(ui.sensorsTab);
	ui.sensorsTab->layout()->addWidget(sensorsEditor);

	connect(ui.saveUiXmlAction,&QAction::triggered,this,&MainWindow::onSaveUiAsXmlTriggered);
	connect(ui.saveUiJsonAction,&QAction::triggered,this,&MainWindow::onSaveUiAsJsonTriggered);
	connect(ui.openUiXmlAction,&QAction::triggered,this,&MainWindow::onOpenUiXmlTriggered);
	connect(ui.openUiJsonAction,&QAction::triggered,this,&MainWindow::onOpenUiJsonTriggered);
	connect(ui.copyUiXmlAsCVarAction,&QAction::triggered,this,&MainWindow::onCopyUiXmlAsVarTriggered);
	connect(ui.copyUiJsonAsCVarAction,&QAction::triggered,this,&MainWindow::onCopyUiJsonAsVarTriggered);
	connect(ui.saveSensorsXmlAction,&QAction::triggered,this,&MainWindow::onSaveSensorsAsXmlTriggered);
	connect(ui.saveSensorsJsonAction,&QAction::triggered,this,&MainWindow::onSaveSensorsAsJsonTriggered);
	connect(ui.openSensorsXmlAction,&QAction::triggered,this,&MainWindow::onOpenSensorsXmlTriggered);
	connect(ui.openSensorsJsonAction,&QAction::triggered,this,&MainWindow::onOpenSensorsJsonTriggered);
	connect(ui.copySensorsXmlAsCVarAction,&QAction::triggered,this,&MainWindow::onCopySensorsXmlAsVarTriggered);
	connect(ui.copySensorsJsonAsCVarAction,&QAction::triggered,this,&MainWindow::onCopySensorsJsonAsVarTriggered);
	connect(ui.uuidGenBtn,&QPushButton::clicked,this,&MainWindow::onGenUuidTriggered);
}

void MainWindow::onSaveUiAsXmlTriggered()
{
	QString fileName=QFileDialog::getSaveFileName(this,tr("Save as xml"),QString(),"All files (*.*)");
	if(fileName.isEmpty())return;
	QByteArray data;
	ControlsGroup grp=controlsEditor->controls();
	ControlsParser::dumpToXml(data,grp);
	QFile file(fileName);
	if(!file.open(QIODevice::WriteOnly))return;
	file.write(data);
	file.close();
}

void MainWindow::onSaveUiAsJsonTriggered()
{
	QString fileName=QFileDialog::getSaveFileName(this,tr("Save as json"),QString(),"All files (*.*)");
	if(fileName.isEmpty())return;
	QByteArray data;
	ControlsGroup grp=controlsEditor->controls();
	ControlsParser::dumpToJson(data,grp);
	QFile file(fileName);
	if(!file.open(QIODevice::WriteOnly))return;
	file.write(data);
	file.close();
}

void MainWindow::onOpenUiXmlTriggered()
{
	QString fileName=QFileDialog::getOpenFileName(this,tr("Open xml"),QString(),"All files (*.*)");
	if(fileName.isEmpty())return;
	QFile file(fileName);
	if(!file.open(QIODevice::ReadOnly))return;
	QByteArray data=file.readAll();
	file.close();
	ControlsGroup grp;
	if(!ControlsParser::parseXmlDescription(data,grp,true))
	{
		QMessageBox::warning(this,tr("Error!"),tr("Parsing error"));
		return;
	}
	controlsEditor->setControls(grp);
}

void MainWindow::onOpenUiJsonTriggered()
{
	QString fileName=QFileDialog::getOpenFileName(this,tr("Open json"),QString(),"All files (*.*)");
	if(fileName.isEmpty())return;
	QFile file(fileName);
	if(!file.open(QIODevice::ReadOnly))return;
	QByteArray data=file.readAll();
	file.close();
	ControlsGroup grp;
	if(!ControlsParser::parseJsonDescription(data,grp,true))
	{
		QMessageBox::warning(this,tr("Error!"),tr("Parsing error"));
		return;
	}
	controlsEditor->setControls(grp);
}

void MainWindow::onCopyUiXmlAsVarTriggered()
{
	QByteArray data;
	ControlsGroup grp=controlsEditor->controls();
	ControlsParser::dumpToXml(data,grp,true);
	qApp->clipboard()->setText(QString::fromUtf8(toCVar("interfaceStr",data)));
}

void MainWindow::onCopyUiJsonAsVarTriggered()
{
	QByteArray data;
	ControlsGroup grp=controlsEditor->controls();
	ControlsParser::dumpToJson(data,grp);
	qApp->clipboard()->setText(QString::fromUtf8(toCVar("interfaceStr",data)));
}

void MainWindow::onSaveSensorsAsXmlTriggered()
{
	QString fileName=QFileDialog::getSaveFileName(this,tr("Save as xml"),QString(),"All files (*.*)");
	if(fileName.isEmpty())return;
	QList<SensorDef> sensors=sensorsEditor->sensors();
	QByteArray data;
	SensorsParser::dumpToXml(data,sensors);
	QFile file(fileName);
	if(!file.open(QIODevice::WriteOnly))return;
	file.write(data);
	file.close();
}

void MainWindow::onSaveSensorsAsJsonTriggered()
{
	QString fileName=QFileDialog::getSaveFileName(this,tr("Save as json"),QString(),"All files (*.*)");
	if(fileName.isEmpty())return;
	QList<SensorDef> sensors=sensorsEditor->sensors();
	QByteArray data;
	SensorsParser::dumpToJson(data,sensors);
	QFile file(fileName);
	if(!file.open(QIODevice::WriteOnly))return;
	file.write(data);
	file.close();
}

void MainWindow::onOpenSensorsXmlTriggered()
{
	QString fileName=QFileDialog::getOpenFileName(this,tr("Open xml"),QString(),"All files (*.*)");
	if(fileName.isEmpty())return;
	QFile file(fileName);
	if(!file.open(QIODevice::ReadOnly))return;
	QByteArray data=file.readAll();
	file.close();
	QList<SensorDef> sensors;
	if(!SensorsParser::parseXmlDescription(data,sensors))
	{
		QMessageBox::warning(this,tr("Error!"),tr("Parsing error"));
		return;
	}
	sensorsEditor->setSensors(sensors);
}

void MainWindow::onOpenSensorsJsonTriggered()
{
	QString fileName=QFileDialog::getOpenFileName(this,tr("Open json"),QString(),"All files (*.*)");
	if(fileName.isEmpty())return;
	QFile file(fileName);
	if(!file.open(QIODevice::ReadOnly))return;
	QByteArray data=file.readAll();
	file.close();
	QList<SensorDef> sensors;
	if(!SensorsParser::parseJsonDescription(data,sensors))
	{
		QMessageBox::warning(this,tr("Error!"),tr("Parsing error"));
		return;
	}
	sensorsEditor->setSensors(sensors);
}

void MainWindow::onCopySensorsXmlAsVarTriggered()
{
	QByteArray data;
	QList<SensorDef> sensors=sensorsEditor->sensors();
	SensorsParser::dumpToXml(data,sensors,true);
	qApp->clipboard()->setText(QString::fromUtf8(toCVar("sensorsStr",data)));
}

void MainWindow::onCopySensorsJsonAsVarTriggered()
{
	QByteArray data;
	QList<SensorDef> sensors=sensorsEditor->sensors();
	SensorsParser::dumpToJson(data,sensors);
	qApp->clipboard()->setText(QString::fromUtf8(toCVar("sensorsStr",data)));
}

void MainWindow::onGenUuidTriggered()
{
	QUuid id=QUuid::createUuid();
	ui.uuidViewText->setText(id.toString());
	ui.uuidCompressedHex->setText(QString::fromUtf8(id.toRfc4122().toHex()));
}

QByteArray MainWindow::toCVar(const QByteArray &varName,QByteArray data)
{
	data.replace('\"',"\\\"");
	QByteArray varData="const char *"+varName+"=";
	QByteArrayList tmp=data.split('\n');
	for(const QByteArray &str:tmp)
		if(!str.isEmpty())
			varData.append("\n\""+str+"\"");
	varData.append(";\n");
	return varData;
}
