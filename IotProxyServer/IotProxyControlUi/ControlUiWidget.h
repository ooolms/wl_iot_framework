#ifndef CONTROLUIWIDGET_H
#define CONTROLUIWIDGET_H

#include "ARpcBase/ARpcOutsideDevice.h"
#include "ARpcOutsideDeviceWrap.h"
#include "ARpcUi/ARpcControlUi.h"
#include <QWidget>
#include <QLocalSocket>

class ControlUiWidget
	:public QWidget
{
	Q_OBJECT
public:
	explicit ControlUiWidget(const QString &idOrName,QWidget *parent=0);
	bool prepare();

private slots:
	void onSocketError(QLocalSocket::LocalSocketError err);
	void onSocketDisconnected();
	void onRawMessage(const ARpcMessage &m);

private:
	QString devIdOrName;
	QString controlsDescr;
	ARpcOutsideDevice *dev;
	ARpcOutsideDeviceWrap *wrp;
	QLocalSocket *sock;
	ARpcControlUi *ui;
};

#endif // CONTROLUIWIDGET_H
