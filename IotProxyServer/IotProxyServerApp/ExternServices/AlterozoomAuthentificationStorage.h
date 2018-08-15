#ifndef ALTEROZOOMAUTHENTIFICATIONSTORAGE_H
#define ALTEROZOOMAUTHENTIFICATIONSTORAGE_H

#include <QString>
#include <QMap>

class AlterozoomAuthentificationStorage
{
public:
	struct AuthKey
	{
		QByteArray host;
		QByteArray email;
		bool operator==(const AuthKey &t)const{return host==t.host&&email==t.email;}
		bool operator<(const AuthKey &t)const{return host<t.host||(host==t.host&&email<t.email);}
	};
	struct AuthValue
	{
		quint64 userId;
		QByteArray token;
		bool operator==(const AuthValue &t)const{return userId==t.userId&&token==t.token;}
	};

public:
	static bool readConfig(const QString &path);
	static void storeConfig();
	static void clearAuth(const QByteArray &host,const QByteArray &email);
	static void setAuth(const QByteArray &host,const QByteArray &email,quint64 userId,const QByteArray &token);
	static const QMap<AuthKey,AuthValue>& getAuthMap();
	static const QByteArray& getDefaultHost();
	static void setDefaultHost(const QByteArray &h);
	static QByteArray hostFromStr(const QByteArray &s);

private:
	static QString cfgPath;
	static QMap<AuthKey,AuthValue> authData;
	static QByteArray defaultHost;
};

#endif // ALTEROZOOMAUTHENTIFICATIONSTORAGE_H
