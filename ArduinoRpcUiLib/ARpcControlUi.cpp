#include "ARpcControlUi.h"
#include "ARpcBase/ARpcSyncCall.h"
#include "ARpcBase/ARpcUnsafeCall.h"
#include <QLayout>

ARpcControlUi::ARpcControlUi(ARpcDevice *dev,const ARpcControlsGroup &controlsDef,QWidget *parent)
	:QWidget(parent)
{
	device=dev;
	connect(device,&ARpcDevice::destroyed,this,&ARpcControlUi::onDeviceDestroyed);
	mainGroup=new ARpcControlUiGroup(controlsDef,this);

	QVBoxLayout *l=new QVBoxLayout(this);
	l->setContentsMargins(0,0,0,0);
	l->addWidget(mainGroup->widget());

	connect(mainGroup,&ARpcControlUiGroup::executeCommand,this,&ARpcControlUi::onExecuteCommand);
}

void ARpcControlUi::onDeviceDestroyed()
{
	device=0;
}

void ARpcControlUi::onExecuteCommand(const QString &command,const QStringList &args,bool syncCall)
{
	if(syncCall)
	{
		ARpcSyncCall call;
		QStringList retVal;
		call.call(device,command,args,retVal);
	}
	else
	{
		ARpcUnsafeCall call;
		QStringList retVal;
		call.call(device,command,args,retVal);
	}
}
