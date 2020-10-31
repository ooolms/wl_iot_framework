#ifndef ALTEROZOOMAUTHTYPES_H
#define ALTEROZOOMAUTHTYPES_H

#include <QByteArray>

struct AlterozoomAuthKey
{
	QByteArray host;
	QByteArray email;
	bool operator==(const AlterozoomAuthKey &t)const{return host==t.host&&email==t.email;}
	bool operator<(const AlterozoomAuthKey &t)const{return host<t.host||(host==t.host&&email<t.email);}
};
struct AlterozoomAuthValue
{
	quint64 userId;
	QByteArray token;
	bool operator==(const AlterozoomAuthValue &t)const{return userId==t.userId&&token==t.token;}
};

#endif // ALTEROZOOMAUTHTYPES_H
