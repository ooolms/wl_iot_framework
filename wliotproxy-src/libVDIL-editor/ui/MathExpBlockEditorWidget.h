#ifndef MATHEXPBLOCKEDITORWIDGET_H
#define MATHEXPBLOCKEDITORWIDGET_H

#include <QWidget>

namespace Ui
{
	class MathExpBlockEditorWidget;
}

class QStatusBar;

namespace WLIOTVDIL
{
	class MathExpBlockEditorWidget
		:public QWidget
	{
		Q_OBJECT
	public:
		explicit MathExpBlockEditorWidget(QWidget *parent=nullptr);
		virtual ~MathExpBlockEditorWidget();
		void setParams(const QStringList &vars,const QString &expr);
		QString expr();
		QStringList vars();

	private slots:
		void onTestExprClicked();

	private:
		Ui::MathExpBlockEditorWidget *ui;
		QStatusBar *testStatus;
	};
}

#endif // MATHEXPBLOCKEDITORWIDGET_H
