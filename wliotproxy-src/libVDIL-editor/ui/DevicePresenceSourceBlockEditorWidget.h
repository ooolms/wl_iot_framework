#ifndef DEVICEPRESENCESOURCEBLOCKEDITORWIDGET_H
#define DEVICEPRESENCESOURCEBLOCKEDITORWIDGET_H

#include <QWidget>
#include <QUuid>
#include "VDIL/editor/IEditorHelper.h"

namespace Ui
{
	class DevicePresenceSourceBlockEditorWidget;
}

namespace WLIOTVDIL
{
	class DevicePresenceSourceBlockEditorWidget
		:public QWidget
	{
		Q_OBJECT
	public:
		explicit DevicePresenceSourceBlockEditorWidget(IEditorHelper *helper,QWidget *parent=nullptr);
		~DevicePresenceSourceBlockEditorWidget();
		void setParams(const QUuid &deviceId,const QString &devName);
		QUuid deviceId()const;

	private slots:
		void onSelectDevClicked();

	private:
		Ui::DevicePresenceSourceBlockEditorWidget *ui;
		IEditorHelper *mHelper;
	};
}

#endif // DEVICEPRESENCESOURCEBLOCKEDITORWIDGET_H
