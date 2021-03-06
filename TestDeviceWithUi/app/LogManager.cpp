#include "LogManager.h"
#include <QScrollBar>

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
		if(p.lvl<minLvl)continue;
		QTextCharFormat f=makeBlockFormat(p.lvl);
		cur.insertBlock();
		cur.setBlockCharFormat(f);
		cur.insertText(p.date.toString()+": "+levelToString(p.lvl)+": ");
		cur.insertText(p.str);
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

QTextCharFormat LogManager::makeBlockFormat(LogManager::Level l)
{
	QTextCharFormat f;
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
	logData.append({l,QDateTime::currentDateTime(),QString::fromUtf8(s)});
	if(l<minLvl)return;
	bool scrollToMax=logView->verticalScrollBar()&&logView->verticalScrollBar()->value()==
		logView->verticalScrollBar()->maximum();
	LogNote &n=logData.last();
	QTextCursor cur=logView->textCursor();
	cur.movePosition(QTextCursor::End);
	QTextCharFormat f=makeBlockFormat(l);
	cur.insertBlock();
	cur.setBlockCharFormat(f);
	cur.insertText(n.date.toString()+": "+levelToString(l)+": ");
	cur.insertText(n.str);
	logView->setTextCursor(cur);
	if(scrollToMax)
		logView->verticalScrollBar()->setValue(logView->verticalScrollBar()->maximum());
}
