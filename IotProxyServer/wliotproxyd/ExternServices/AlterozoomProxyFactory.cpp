#include "AlterozoomProxyFactory.h"

QList<QNetworkProxy> AlteroozomProxyFactory::queryProxy(const QNetworkProxyQuery &query)
{
	QList<QNetworkProxy> retVal;
	if(proxyMap.contains(query.peerHostName()))
		retVal.append(proxyMap[query.peerHostName()]);
	else retVal.append(defaultProxy);
	return retVal;
}

AlteroozomProxyFactory::AlteroozomProxyFactory()
{
	defaultProxy=QNetworkProxy::NoProxy;
}
