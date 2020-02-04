#ifndef LOGMANAGER_H
#define LOGMANAGER_H

#include <QObject>
#include <QTextEdit>

class LogManager
	:public QObject
{
	Q_OBJECT

public:
	enum Level
	{
		DEBUG,
		INFO,
		WARNING,
		ERROR
	};

public:
	explicit LogManager(QTextEdit *view,QObject *parent=nullptr);
	void setMinLevel(Level l);
	void clear();

public slots:
	void onDbgMessage(const QByteArray &msg);
	void onInfoMessage(const QByteArray &msg);
	void onWarningMessage(const QByteArray &msg);
	void onErrorMessage(const QByteArray &msg);

private:
	QTextBlockFormat makeBlockFormat(Level l);
	QString levelToString(Level l);
	void onMessage(const QByteArray &s,Level l);

private:
	Level minLvl;
	QTextEdit *logView;
	QList<QPair<Level,QString>> logData;
};

#endif // LOGMANAGER_H
