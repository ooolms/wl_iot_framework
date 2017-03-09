#ifndef ARPCCONTROLUI_H
#define ARPCCONTROLUI_H

#include <QWidget>
#include "ARpcBase/ARpcDevice.h"
#include "ARpcBase/ARpcControlsDefinition.h"
#include "ARpcBase/ARpcDeviceState.h"
#include "ARpcBase/ARpcSimpleMsgDispatch.h"

class ARpcControlUiGroup;

class ARpcControlUi
	:public QWidget
{
	Q_OBJECT
public:
	explicit ARpcControlUi(ARpcDevice *dev,const ARpcControlsGroup &controlsDef,QWidget *parent=0);
	void updateState(const ARpcDeviceState &state);

private slots:
	void onDeviceDestroyed();
	void onExecuteCommand(const QString &command,const QStringList &args,bool syncCall);
	void onCommandStateChanged(const QString &command,int index,const QString &value);


private:
	ARpcDevice *device;
	ARpcControlUiGroup *mainGroup;
	ARpcSimpleMsgDispatch *msgDsp;
};

#endif // ARPCCONTROLUI_H
