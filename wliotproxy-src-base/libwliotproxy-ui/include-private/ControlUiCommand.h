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

#ifndef CONTROLUICOMMAND_H
#define CONTROLUICOMMAND_H

#include "ControlUiElement.h"
#include "IParamElement.h"
#include "wliot/devices/ControlsDefinition.h"
#include <QObject>

class QGroupBox;

namespace WLIOTUi
{
	class ControlUiCommand
		:public ControlUiElement
	{
		Q_OBJECT
	public:
		explicit ControlUiCommand(const WLIOT::ControlsCommand &cmd,QObject *parent=0);
		virtual QWidget* widget()override;
		void updateState(const QMap<quint32,QByteArray> &values);
		virtual bool isCommand()const override{return true;}
		QString getCommand()const;

	private slots:
		void onElementActivated();
		void onSendCommand();

	private:
		static int numOfVisibleParams(const WLIOT::ControlsCommand &cmd);
		static int indexOfFirstVisibleParam(const WLIOT::ControlsCommand &cmd);
		static QString btnText(const WLIOT::ControlsCommand &cmd);

	private:
		QWidget *w;
		QList<IParamElement*> elements;
		QByteArray command;
		bool sendCommandOnElementActivation;
	};
}

#endif // CONTROLUIELEMENT_H
