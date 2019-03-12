#ifndef CUSTOMNETWORKPROXYFACTORY_H
#define CUSTOMNETWORKPROXYFACTORY_H

#include <QNetworkProxyFactory>
#include <QNetworkProxyQuery>

class CustomNetworkProxyFactory
	:public QNetworkProxyFactory
{
public:
	CustomNetworkProxyFactory();
	virtual QList<QNetworkProxy> queryProxy(const QNetworkProxyQuery &query)override;

public:
	QMap<QString,QNetworkProxy> proxyMap;
	QNetworkProxy defaultProxy;
	static const QNetworkProxy noProxy;
};

#endif // CUSTOMNETWORKPROXYFACTORY_H
