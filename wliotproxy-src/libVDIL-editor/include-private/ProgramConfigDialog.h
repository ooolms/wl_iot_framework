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

#ifndef PROGRAMCONFIGDIALOG_H
#define PROGRAMCONFIGDIALOG_H

#include <QDialog>
#include "ControlsEditor.h"
#include "SensorsEditor.h"

namespace Ui
{
	class ProgramConfigDialog;
}

namespace WLIOTVDIL
{
	class Program;
	class DataUnit;

	class ProgramConfigDialog
		:public QDialog
	{
		Q_OBJECT
	public:
		explicit ProgramConfigDialog(QWidget *parent=0);
		virtual ~ProgramConfigDialog();
		void loadSettings(const Program *p);
		void storeSettings(Program *p);

	private slots:
		void onAddRuntimeVarClicked();
		void onDelRuntimeVarClicked();
		void onEditRuntimeVarClicked();
		void onRenameRuntimeVarClicked();
		void onGenerateIdClicked();

	private:
		static QString dataUnitHint(const DataUnit &v);
		int selectedVarIndex();

	private:
		Ui::ProgramConfigDialog *ui;
		WLIOTUi::ControlsEditor *vdevControlsEditor;
		WLIOTUi::SensorsEditor *vdevSensorsEditor;
		QStringList mVarNames;
		QList<DataUnit> mVarDefVals;
	};
}
#endif // PROGRAMCONFIGDIALOG_H
