#include "ControlUiWidget.h"
#include <QTimer>
#include <QEventLoop>
#include <QLayout>

static const QString localServerName=QString("wliotproxyd");

ControlUiWidget::ControlUiWidget(const QString &idOrName,QWidget *parent)
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
		//TODO msg
		return false;
	}
	QTimer t(this);
	t.setInterval(ARpcConfig::syncCallWaitTime);
	t.setSingleShot(true);
	QEventLoop loop;
	bool ok=false;
	auto conn1=connect(dev,&ARpcOutsideDevice::rawMessage,[&t,&loop,this,&ok](const ARpcMessage &m)
	{
		if(m.title==ARpcConfig::funcAnswerErrMsg)
		{
			ok=false;
			loop.quit();
		}
		else if(m.title!=ARpcConfig::funcAnswerOkMsg)return;
		if(m.args.count()!=1)
		{
			ok=false;
			loop.quit();
		}
		ok=true;
		controlsDescr=m.args[0].trimmed();
		loop.quit();
	});
	connect(&t,&QTimer::timeout,&loop,&QEventLoop::quit);
	connect(dev,&ARpcOutsideDevice::disconnected,&loop,&QEventLoop::quit);
//	t.start();
	dev->writeMsg("list_controls",QStringList()<<devIdOrName);
	loop.exec(QEventLoop::ExcludeUserInputEvents);
	disconnect(conn1);
	if(!ok)return false;
	ARpcControlsGroup grp;
	if(!ARpcControlsGroup::parseXmlDescription(controlsDescr,grp))
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
	{
		qDebug()<<"Function error: "<<m.args.join("|");
		close();
	}
}
