#ifndef SENSORSEDITOR_H
#define SENSORSEDITOR_H

#include <QWidget>
#include "wliot/devices/SensorDef.h"

class QListWidgetItem;

namespace Ui
{
	class SensorsEditor;
}

namespace WLIOTUi
{
	class SensorsEditor
		:public QWidget
	{
		Q_OBJECT

	public:
		explicit SensorsEditor(QWidget *parent=nullptr);
		explicit SensorsEditor(const QList<WLIOT::SensorDef::NumType> &numTypes,
			bool editTime,bool allowLocalTime,QWidget *parent=nullptr);
		~SensorsEditor();
		void setSensors(const QList<WLIOT::SensorDef> &sensors);
		QList<WLIOT::SensorDef> sensors();

	private slots:
		void onSensorsTreeSelChanged();
		void onAddClicked();
		void onDelClicked();

	private:
		void construct(QList<WLIOT::SensorDef::NumType> numTypes,bool editTime,bool allowLocalTime);
		void saveCurrentEditedSensorsItem();
		void dumpSensors(QList<WLIOT::SensorDef> &sensors);
		void buildSensorsList(const QList<WLIOT::SensorDef> &sensors);

	private:
		Ui::SensorsEditor *ui;
		bool mEditTime;
		bool mAllowLocalTime;
		QListWidgetItem *currentEditedSensorsItem;
	};
}

#endif // SENSORSEDITOR_H
