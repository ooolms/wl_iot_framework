#include "TriggersEditDialog.h"
#include "ui_TriggersEditDialog.h"
#include <QCheckBox>

TriggersEditDialog::TriggersEditDialog(Program *p,QWidget *parent)
	:QDialog(parent)
{
	prg=p;
	ui=new Ui::TriggersEditDialog;
	ui->setupUi(this);

	connect(ui->buttonBox,&QDialogButtonBox::accepted,this,&TriggersEditDialog::onOkClicked);
	connect(ui->buttonBox,&QDialogButtonBox::rejected,this,&TriggersEditDialog::reject);

	for(auto id:prg->allUsedStorages())
	{
		QListWidgetItem *item=new QListWidgetItem(ui->storageTriggersList);
		QCheckBox *chk=new QCheckBox(prg->findDevName(id.deviceId)+" ("+id.deviceId.toString()+"): "+
			QString::fromUtf8(id.sensorName),ui->storageTriggersList);
		storageTriggersMap[chk]=id;
		ui->storageTriggersList->setItemWidget(item,chk);
		if(prg->storageTriggers().contains(id))
			chk->setChecked(true);
	}
}

TriggersEditDialog::~TriggersEditDialog()
{
	delete ui;
}

void TriggersEditDialog::onOkClicked()
{
	QList<StorageId> list;
	for(auto i=storageTriggersMap.begin();i!=storageTriggersMap.end();++i)
	{
		if(i.key()->isChecked())
			list.append(i.value());
	}
	prg->setStorageTriggers(list);
	accept();
}
