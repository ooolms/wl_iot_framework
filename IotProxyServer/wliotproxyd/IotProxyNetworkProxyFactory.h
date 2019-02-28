#ifndef IOTPROXYNETWORKPROXYFACTORY_H
#define IOTPROXYNETWORKPROXYFACTORY_H

#include <QNetworkProxyFactory>
#include <QNetworkProxyQuery>

class IotProxyNetworkProxyFactory
	:public QNetworkProxyFactory
{
public:
	IotProxyNetworkProxyFactory();
	virtual QList<QNetworkProxy> queryProxy(const QNetworkProxyQuery &query)override;

public:
	QMap<QString,QNetworkProxy> proxyMap;
	QNetworkProxy defaultProxy;
	static const QNetworkProxy noProxy;
};

#endif // IOTPROXYNETWORKPROXYFACTORY_H
