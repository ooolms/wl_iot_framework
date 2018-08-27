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
	dev=new ARpcOutsideDevice(sock,this);
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
