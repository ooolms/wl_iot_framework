/*******************************************
Copyright 2017 OOO "LMS"

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

	http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.*/

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
