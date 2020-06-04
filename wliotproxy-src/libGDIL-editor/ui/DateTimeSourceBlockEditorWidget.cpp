#include "DateTimeSourceBlockEditorWidget.h"
#include <QCheckBox>
#include <QLabel>
#include <QLayout>

using namespace WLIOTGDIL;

DateTimeSourceBlockEditorWidget::DateTimeSourceBlockEditorWidget(QWidget *parent)
	:QWidget(parent)
{
	QLabel *l=new QLabel(this);
	secondsCheck=new QCheckBox("seconds",this);
	minutesCheck=new QCheckBox("minutes",this);
	hoursCheck=new QCheckBox("hours",this);
	dayOfMonthCheck=new QCheckBox("day of month (1 - 31)",this);
	dayOfWeekCheck=new QCheckBox("day of week (1 - monday to 7 - sunday)",this);
	monthCheck=new QCheckBox("month",this);
	yearCheck=new QCheckBox("year",this);
	unixTimeCheck=new QCheckBox("Unix time (seconds from 01.01.1970, may be negative)",this);
	dateTimeCheck=new QCheckBox("date+time (special internal type)",this);

	QVBoxLayout *mainLayout=new QVBoxLayout(this);
	mainLayout->addWidget(l);
	mainLayout->addWidget(secondsCheck);
	mainLayout->addWidget(minutesCheck);
	mainLayout->addWidget(hoursCheck);
	mainLayout->addWidget(dayOfMonthCheck);
	mainLayout->addWidget(dayOfWeekCheck);
	mainLayout->addWidget(monthCheck);
	mainLayout->addWidget(yearCheck);
	mainLayout->addWidget(unixTimeCheck);
	mainLayout->addWidget(dateTimeCheck);
}

void DateTimeSourceBlockEditorWidget::setDateOutputs(const DateTimeSourceBlock::DateTimeOutputs &o)
{
	secondsCheck->setChecked(o&DateTimeSourceBlock::SECONDS);
	minutesCheck->setChecked(o&DateTimeSourceBlock::MINUTES);
	hoursCheck->setChecked(o&DateTimeSourceBlock::HOURS);
	dayOfMonthCheck->setChecked(o&DateTimeSourceBlock::DAY_OF_MONTH);
	dayOfWeekCheck->setChecked(o&DateTimeSourceBlock::DAY_OF_WEEK);
	monthCheck->setChecked(o&DateTimeSourceBlock::MONTH);
	yearCheck->setChecked(o&DateTimeSourceBlock::YEAR);
	unixTimeCheck->setChecked(o&DateTimeSourceBlock::UNIX_TIME);
	dateTimeCheck->setChecked(o&DateTimeSourceBlock::DATETIME);
}

DateTimeSourceBlock::DateTimeOutputs DateTimeSourceBlockEditorWidget::dateOutputs()const
{
	DateTimeSourceBlock::DateTimeOutputs o;
	if(secondsCheck->isChecked())
		o|=DateTimeSourceBlock::SECONDS;
	if(minutesCheck->isChecked())
		o|=DateTimeSourceBlock::MINUTES;
	if(hoursCheck->isChecked())
		o|=DateTimeSourceBlock::HOURS;
	if(dayOfMonthCheck->isChecked())
		o|=DateTimeSourceBlock::DAY_OF_MONTH;
	if(dayOfWeekCheck->isChecked())
		o|=DateTimeSourceBlock::DAY_OF_WEEK;
	if(monthCheck->isChecked())
		o|=DateTimeSourceBlock::MONTH;
	if(yearCheck->isChecked())
		o|=DateTimeSourceBlock::YEAR;
	if(unixTimeCheck->isChecked())
		o|=DateTimeSourceBlock::UNIX_TIME;
	if(dateTimeCheck->isChecked())
		o|=DateTimeSourceBlock::DATETIME;
	return o;
}
