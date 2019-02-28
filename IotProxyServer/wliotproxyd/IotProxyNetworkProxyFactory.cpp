#include "IotProxyNetworkProxyFactory.h"
#include <QDebug>

const QNetworkProxy IotProxyNetworkProxyFactory::noProxy=QNetworkProxy::NoProxy;

QList<QNetworkProxy> IotProxyNetworkProxyFactory::queryProxy(const QNetworkProxyQuery &query)
{
	QList<QNetworkProxy> retVal;
	qDebug()<<"PROXY RQ: "<<query.peerHostName()<<":"<<query.peerPort()<<":"<<query.protocolTag()<<
		":"<<(int)query.queryType();
	if(query.queryType()==QNetworkProxyQuery::UrlRequest)
	{
		if(query.peerPort()==80||query.peerPort()==443||
			query.protocolTag()=="https"||query.protocolTag()=="http")
		{
			if(proxyMap.contains(query.peerHostName()))
				retVal.append(proxyMap[query.peerHostName()]);
			else retVal.append(defaultProxy);
		}
	}
	if(retVal.isEmpty())
		retVal.append(noProxy);
	return retVal;
}

IotProxyNetworkProxyFactory::IotProxyNetworkProxyFactory()
{
	defaultProxy=QNetworkProxy::NoProxy;
	QNetworkProxyFactory::setApplicationProxyFactory(this);
}
