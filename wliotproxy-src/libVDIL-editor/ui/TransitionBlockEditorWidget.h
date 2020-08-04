#ifndef TRANSITIONBLOCKEDITORWIDGET_H
#define TRANSITIONBLOCKEDITORWIDGET_H

#include <QWidget>

class QSpinBox;

namespace WLIOTVDIL
{
	class TransitionBlockEditorWidget
		:public QWidget
	{
		Q_OBJECT
	public:
		explicit TransitionBlockEditorWidget(QWidget *parent=nullptr);
		void setCount(quint32 c);
		quint32 count();

	private:
		QSpinBox *countEdit;
	};
}

#endif // TRANSITIONBLOCKEDITORWIDGET_H
