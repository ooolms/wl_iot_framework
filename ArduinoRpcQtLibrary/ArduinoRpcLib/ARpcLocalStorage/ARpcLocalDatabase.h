#ifndef ARPCLOCALDATABASE_H
#define ARPCLOCALDATABASE_H

#include <QObject>

/**
 * @brief The ARpcLocalDatabase class
 * База с базами данных датчиков. Задача класса - управлять базами со значениями отдельных датчиков
 * (создание/удаление/перечисление).
 */
class ARpcLocalDatabase
	:public QObject
{
	Q_OBJECT
public:
	explicit ARpcLocalDatabase(QObject *parent=0);
};

#endif // ARPCLOCALDATABASE_H
