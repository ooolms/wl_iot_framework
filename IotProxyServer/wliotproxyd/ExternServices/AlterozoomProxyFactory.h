#ifndef ALTEROOZOMPROXYFACTORY_H
#define ALTEROOZOMPROXYFACTORY_H

#include <QNetworkProxyFactory>
#include <QNetworkProxyQuery>

class AlteroozomProxyFactory
	:public QNetworkProxyFactory
{
public:
	AlteroozomProxyFactory();
	virtual QList<QNetworkProxy> queryProxy(const QNetworkProxyQuery &query)override;

public:
	QMap<QString,QNetworkProxy> proxyMap;
	QNetworkProxy defaultProxy;
};

#endif // ALTEROOZOMPROXYFACTORY_H
