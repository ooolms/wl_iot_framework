#include "CommandReactionConfigDialog.h"
#include "ui_CommandReactionConfigDialog.h"
#include "DeviceStateMapEditDialog.h"

CommandReactionConfigDialog::CommandReactionConfigDialog(QWidget *parent)
	:QDialog(parent)
{
	ui=new Ui::CommandReactionConfigDialog;
	ui->setupUi(this);
	connect(ui->buttonBox,&QDialogButtonBox::accepted,this,&CommandReactionConfigDialog::accept);
	connect(ui->buttonBox,&QDialogButtonBox::rejected,this,&CommandReactionConfigDialog::reject);
	connect(ui->addAnswArgBtn,&QPushButton::clicked,this,&CommandReactionConfigDialog::onAddArgClicked);
	connect(ui->delAnswArgBtn,&QPushButton::clicked,this,&CommandReactionConfigDialog::onDelArgClicked);
	connect(ui->editStateChangeBeforeAnswerBtn,&QPushButton::clicked,
		this,&CommandReactionConfigDialog::onEditStateChangeBeforeAnswerClicked);
	connect(ui->editStateChangeAfterAnswerBtn,&QPushButton::clicked,
		this,&CommandReactionConfigDialog::onEditStateChangeAfterAnswerClicked);
	ui->argsList->setDragDropMode(QAbstractItemView::InternalMove);
}

CommandReactionConfigDialog::~CommandReactionConfigDialog()
{
	delete ui;
}

CommandReactionConfig CommandReactionConfigDialog::config()
{
	CommandReactionConfig cfg;
	cfg.jsScript=ui->jsCodeEdit->toPlainText();
	for(int i=0;i<ui->argsList->count();++i)
		cfg.retVal.append(ui->argsList->item(i)->text().toUtf8());
	if(ui->reactOkBtn->isChecked())
		cfg.act=CommandReaction::CMD_ANSWER_OK;
	else if(ui->reactErrBtn->isChecked())
		cfg.act=CommandReaction::CMD_ANSWER_ERR;
	else if(ui->reactJsBtn->isChecked())
		cfg.act=CommandReaction::CMD_JS_EXEC;
	else if(ui->reactResetBtn->isChecked())
		cfg.act=CommandReaction::DEV_RESET;
	else if(ui->reactStuckBtn->isChecked())
		cfg.act=CommandReaction::DEV_STUCK;
	else if(ui->reactManualBtn->isChecked())
		cfg.act=CommandReaction::CMD_WAIT_MANUAL_ANSWER;
	else cfg.act=CommandReaction::IGNORE;
	cfg.stateChangeBeforeAnswer=stateChangeBeforeAnswer;
	cfg.stateChangeAfterAnswer=stateChangeAfterAnswer;
	return cfg;
}

void CommandReactionConfigDialog::setConfig(const CommandReactionConfig &c)
{
	if(c.act==CommandReaction::CMD_ANSWER_OK)
		ui->reactOkBtn->setChecked(true);
	else if(c.act==CommandReaction::CMD_ANSWER_ERR)
		ui->reactErrBtn->setChecked(true);
	else if(c.act==CommandReaction::DEV_RESET)
		ui->reactResetBtn->setChecked(true);
	else if(c.act==CommandReaction::DEV_STUCK)
		ui->reactStuckBtn->setChecked(true);
	else if(c.act==CommandReaction::CMD_JS_EXEC)
		ui->reactJsBtn->setChecked(true);
	else if(c.act==CommandReaction::CMD_WAIT_MANUAL_ANSWER)
		ui->reactManualBtn->setChecked(true);
	else ui->reactIgnoreBtn->setChecked(true);
	ui->jsCodeEdit->setPlainText(c.jsScript);
	ui->argsList->clear();
	for(const QByteArray &a:c.retVal)
	{
		QListWidgetItem *item=new QListWidgetItem(ui->argsList);
		item->setFlags(item->flags()|Qt::ItemIsEditable);
		item->setText(QString::fromUtf8(a));
	}
	stateChangeBeforeAnswer=c.stateChangeBeforeAnswer;
	stateChangeAfterAnswer=c.stateChangeAfterAnswer;
}

void CommandReactionConfigDialog::onAddArgClicked()
{
	QListWidgetItem *item=new QListWidgetItem(ui->argsList);
	item->setFlags(item->flags()|Qt::ItemIsEditable);
}

void CommandReactionConfigDialog::onDelArgClicked()
{
	if(ui->argsList->currentItem())
		delete ui->argsList->currentItem();
}

void CommandReactionConfigDialog::onEditStateChangeBeforeAnswerClicked()
{
	DeviceStateMapEditDialog dlg(this);
	dlg.setMap(stateChangeBeforeAnswer);
	if(dlg.exec()!=QDialog::Accepted)return;
	stateChangeBeforeAnswer=dlg.getMap();
}

void CommandReactionConfigDialog::onEditStateChangeAfterAnswerClicked()
{
	DeviceStateMapEditDialog dlg(this);
	dlg.setMap(stateChangeAfterAnswer);
	if(dlg.exec()!=QDialog::Accepted)return;
	stateChangeAfterAnswer=dlg.getMap();
}

void CommandReactionConfigDialog::clearRadioBtns()
{
	ui->reactOkBtn->setChecked(false);
	ui->reactErrBtn->setChecked(false);
	ui->reactJsBtn->setChecked(false);
	ui->reactResetBtn->setChecked(false);
	ui->reactStuckBtn->setChecked(false);
	ui->reactIgnoreBtn->setChecked(false);
	ui->reactManualBtn->setChecked(false);
}
