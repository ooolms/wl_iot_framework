#ifndef DEVICESTATESOURCEBLOCKEDITORWIDGET_H
#define DEVICESTATESOURCEBLOCKEDITORWIDGET_H

#include <QWidget>
#include "EditorInternalApi.h"
#include "GDIL/editor/IEditorHelper.h"

namespace Ui
{
	class DeviceStateSourceBlockEditorWidget;
}
class EditorInternalApi;

class DeviceStateSourceBlockEditorWidget
	:public QWidget
{
	Q_OBJECT
public:
	explicit DeviceStateSourceBlockEditorWidget(IEditorHelper *helper,QWidget *parent=nullptr);
	~DeviceStateSourceBlockEditorWidget();
	void setParams(const QUuid &devId,const QString &devName,const QByteArray &stateKey,
		bool cmdState,bool boolOut,quint32 cmdStateIndex);
	QUuid deviceId()const;
	QByteArray stateKey()const;
	bool commandState()const;
	bool boolOut()const;
	quint32 commandStateIndex()const;

private slots:
	void onSelectDeviceClicked();
	void onNumBtnStateChanged();

private:
	IEditorHelper *mHelper;
	Ui::DeviceStateSourceBlockEditorWidget *ui;
};

#endif // DEVICESTATESOURCEBLOCKEDITORWIDGET_H
