#ifndef ELEMENTSETTINGSWIDGET_H
#define ELEMENTSETTINGSWIDGET_H

#include <QStackedWidget>
#include <QTreeWidgetItem>
#include "ARpcBase/ARpcControlsDefinition.h"
#include "ui_GroupSettingsEdit.h"
#include "ui_ControlSettingsEdit.h"
#include "ui_ParamSettingsEdit.h"

class ElementSettingsWidget
	:public QStackedWidget
{
	Q_OBJECT
public:
	explicit ElementSettingsWidget(QWidget *parent=0);
	void editNothing();
	void editGroup(ARpcControlsGroup *group);
	void editControl(ARpcCommandControl *control);
	void editParam(ARpcControlParam *param);
	void saveGroup(ARpcControlsGroup *group);
	void saveControl(ARpcCommandControl *control);
	void saveParam(ARpcControlParam *param);

private slots:
	void onAddSelectValueClicked();
	void onDelSelectValueClicked();

private:
	void resetAllConfigs();
	void addToSelectValuesList(const QString &str);

private:
	QWidget *nWidget,*cWidget,*gWidget,*pWidget;
	Ui::ControlSettingsEdit controlUi;
	Ui::GroupSettingsEdit groupUi;
	Ui::ParamSettingsEdit paramUi;
};

#endif // ELEMENTSETTINGSWIDGET_H
