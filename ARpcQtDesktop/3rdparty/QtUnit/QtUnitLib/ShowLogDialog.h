#ifndef SHOWLOGDIALOG_H
#define SHOWLOGDIALOG_H

#include <QDialog>

class ShowLogDialog
	:public QDialog
{
	Q_OBJECT
public:
	ShowLogDialog(const QString &html,QWidget *parent=0);
};

#endif // SHOWLOGDIALOG_H
