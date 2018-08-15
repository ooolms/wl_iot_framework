#include "AlterozoomNoCookieJar.h"
#include <QList>
#include <QDebug>

AlterozoomNoCookieJar::AlterozoomNoCookieJar(QObject *parent)
	:QNetworkCookieJar(parent)
{
}

QList<QNetworkCookie> AlterozoomNoCookieJar::cookiesForUrl(const QUrl &url)const
{
	return QNetworkCookieJar::cookiesForUrl(url);
}

bool AlterozoomNoCookieJar::setCookiesFromUrl(const QList<QNetworkCookie> &cookieList,const QUrl &url)
{
	return QNetworkCookieJar::setCookiesFromUrl(cookieList,url);
}

bool AlterozoomNoCookieJar::insertCookie(const QNetworkCookie &cookie)
{
	if(checkIfTargetCookie(cookie))
		return false;
	else return QNetworkCookieJar::insertCookie(cookie);
}

bool AlterozoomNoCookieJar::updateCookie(const QNetworkCookie &cookie)
{
	return QNetworkCookieJar::updateCookie(cookie);
}

bool AlterozoomNoCookieJar::deleteCookie(const QNetworkCookie &cookie)
{
	return QNetworkCookieJar::deleteCookie(cookie);
}

bool AlterozoomNoCookieJar::checkIfTargetCookie(const QNetworkCookie &c)
{
	return c.domain()=="alterozoom.com"||c.domain().endsWith(".alterozoom.com");
}
