#include "LogManager.h"

LogManager::LogManager(QTextEdit *view,QObject *parent)
	:QObject(parent)
{
	minLvl=DEBUG;
	logView=view;
}

void LogManager::setMinLevel(LogManager::Level l)
{
	minLvl=l;
	logView->clear();
	QTextCursor cur=logView->textCursor();
	cur.movePosition(QTextCursor::End);
	for(auto &p:logData)
	{
		if(p.first<minLvl)continue;
		QTextBlockFormat f=makeBlockFormat(p.first);
		cur.insertBlock(f);
		cur.insertText(levelToString(p.first)+": ");
		cur.insertText(p.second);
	}
	logView->setTextCursor(cur);
}

void LogManager::clear()
{
	logData.clear();
	logView->clear();
}

void LogManager::onDbgMessage(const QByteArray &msg)
{
	onMessage(msg,DEBUG);
}

void LogManager::onInfoMessage(const QByteArray &msg)
{
	onMessage(msg,INFO);
}

void LogManager::onWarningMessage(const QByteArray &msg)
{
	onMessage(msg,WARNING);
}

void LogManager::onErrorMessage(const QByteArray &msg)
{
	onMessage(msg,ERROR);
}

QTextBlockFormat LogManager::makeBlockFormat(LogManager::Level l)
{
	QTextBlockFormat f;
	if(l==WARNING)
		f.setForeground(Qt::darkYellow);
	else if(l==INFO)
		f.setForeground(Qt::darkBlue);
	else if(l==ERROR)
		f.setForeground(Qt::red);
	return f;
}

QString LogManager::levelToString(LogManager::Level l)
{
	if(l==INFO)
		return "INFO";
	else if(l==WARNING)
		return "WARNING";
	else if(l==ERROR)
		return "ERROR";
	else return "DEBUG";
}

void LogManager::onMessage(const QByteArray &s,LogManager::Level l)
{
	logData.append(qMakePair(l,s));
	if(l<minLvl)return;
	QTextCursor cur=logView->textCursor();
	cur.movePosition(QTextCursor::End);
	QTextBlockFormat f=makeBlockFormat(l);
	cur.insertBlock(f);
	cur.insertText(levelToString(l)+": ");
	cur.insertText(QString::fromUtf8(s));
	logView->setTextCursor(cur);
}
