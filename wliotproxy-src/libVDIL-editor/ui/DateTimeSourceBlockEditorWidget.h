#ifndef DATETIMESOURCEBLOCKEDITORWIDGET_H
#define DATETIMESOURCEBLOCKEDITORWIDGET_H

#include <QWidget>
#include "VDIL/blocks/DateTimeSourceBlock.h"

class QCheckBox;

namespace WLIOTVDIL
{
	class DateTimeSourceBlockEditorWidget
		:public QWidget
	{
		Q_OBJECT
	public:
		explicit DateTimeSourceBlockEditorWidget(QWidget *parent=nullptr);
		void setDateOutputs(const DateTimeSourceBlock::DateTimeOutputs &o);
		DateTimeSourceBlock::DateTimeOutputs dateOutputs()const;

	private:
		QCheckBox *secondsCheck,*minutesCheck,*hoursCheck,*dayOfMonthCheck,*dayOfWeekCheck,
			*monthCheck,*yearCheck,*unixTimeCheck,*dateTimeCheck;
	};
}

#endif // DATETIMESOURCEBLOCKEDITORWIDGET_H
