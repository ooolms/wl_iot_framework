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

#include "ControlUiWidget.h"
#include <QTimer>
#include <QEventLoop>
#include <QLayout>

static const QString localServerName=QString("wliotproxyd");

ControlUiWidget::ControlUiWidget(const QByteArray &idOrName,QWidget *parent)
	:QWidget(parent)
{
	devIdOrName=idOrName;
	sock=new QLocalSocket(this);
	dev=new ARpcOutsideDevice(sock,[this](){sock->flush();},this);
	connect(sock,&QLocalSocket::connected,dev,&ARpcOutsideDevice::onDeviceConnected);
	wrp=new ARpcOutsideDeviceWrap(devIdOrName,dev,this);

	new QVBoxLayout(this);

	connect(sock,static_cast<void (QLocalSocket::*)(QLocalSocket::LocalSocketError)>(&QLocalSocket::error),
		this,&ControlUiWidget::onSocketError);
	connect(sock,&QLocalSocket::disconnected,this,&ControlUiWidget::onSocketDisconnected);
}

bool ControlUiWidget::prepare()
{
	sock->connectToServer(localServerName);
	if(!sock->waitForConnected())
	{
		qDebug()<<"Can't connect to server: "<<localServerName;
		return false;
	}
	wrp->queryDeviceId();
	ARpcControlsGroup grp;
	if(!wrp->getControlsDescription(grp))
	{
		//TODO msg
		return false;
	}
	ui=new ARpcControlUi(wrp,grp,this);
	layout()->addWidget(ui);
	connect(dev,&ARpcOutsideDevice::rawMessage,this,&ControlUiWidget::onRawMessage);
	return true;
}

void ControlUiWidget::onSocketError(QLocalSocket::LocalSocketError err)
{
	Q_UNUSED(err)
	qDebug()<<"Socket error: "<<sock->errorString();
	close();
}

void ControlUiWidget::onSocketDisconnected()
{
	qDebug()<<"Socket disconnected";
	close();
}

void ControlUiWidget::onRawMessage(const ARpcMessage &m)
{
	if(m.title==ARpcConfig::funcAnswerErrMsg)
		qDebug()<<"Function error: "<<m.args.join("|");
}
