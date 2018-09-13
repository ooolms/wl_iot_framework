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
	explicit ControlUiWidget(const QByteArray &idOrName,QWidget *parent=0);
	bool prepare();

private slots:
	void onSocketError(QLocalSocket::LocalSocketError err);
	void onSocketDisconnected();
	void onRawMessage(const ARpcMessage &m);

private:
	QByteArray devIdOrName;
	QByteArray controlsDescr;
	ARpcOutsideDevice *dev;
	ARpcOutsideDeviceWrap *wrp;
	QLocalSocket *sock;
	ARpcControlUi *ui;
};

#endif // CONTROLUIWIDGET_H
