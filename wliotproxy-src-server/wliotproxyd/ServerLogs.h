#ifndef SERVERLOGS_H
#define SERVERLOGS_H

#include <QString>

namespace log4cpp
{
	class Category;
}

class ServerLogs
{
public:
	static bool setup(const QString &baseLogsDir,bool dupToStdout);
	static void terminate();
	static void logMain(QtMsgType type,const QString &msg);
	static void logDevices(QtMsgType type,const QString &msg);
	static void logMain(QtMsgType type,const QByteArray &msg);
	static void logDevices(QtMsgType type,const QByteArray &msg);
	inline static bool dupLogToStdout(){return mDupLogToStdout;}

private:
	static void log(QtMsgType type,const QByteArray &msg,log4cpp::Category *cat);
	static log4cpp::Category *mkLog(const std::string &catName,const std::string &filePath);
	static void setLogPriority(log4cpp::Category *cat,QtMsgType minLogType);
	static bool checkLogFile(const QString &path);

private:
	static log4cpp::Category *mMainLogCat;
	static log4cpp::Category *mDevicesLogCat;
	static bool mDupLogToStdout;
};

#if (QT_VERSION<QT_VERSION_CHECK(5,5,0))
#define qInfo qDebug
#define QtInfoMsg QtDebugMsg
#endif


#endif // SERVERLOGS_H
