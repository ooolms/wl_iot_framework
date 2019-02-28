#include "IotProxyNetworkProxyFactory.h"
#include <QDebug>

const QNetworkProxy IotProxyNetworkProxyFactory::noProxy=QNetworkProxy::NoProxy;

QList<QNetworkProxy> IotProxyNetworkProxyFactory::queryProxy(const QNetworkProxyQuery &query)
{
	QList<QNetworkProxy> retVal;
	if(query.peerPort()==80||query.peerPort()==443||query.peerPort()==-1)
	{
		if(proxyMap.contains(query.peerHostName()))
			retVal.append(proxyMap[query.peerHostName()]);
		else retVal.append(defaultProxy);
	}
	else retVal.append(noProxy);
	return retVal;
}

IotProxyNetworkProxyFactory::IotProxyNetworkProxyFactory()
{
	defaultProxy=QNetworkProxy::NoProxy;
	QNetworkProxyFactory::setApplicationProxyFactory(this);
}
