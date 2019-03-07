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

#ifndef ALTEROZOOMNOCOOKIEJAR_H
#define ALTEROZOOMNOCOOKIEJAR_H

#include <QNetworkCookieJar>
#include <QNetworkCookie>

class AlterozoomNoCookieJar
	:public QNetworkCookieJar
{
	Q_OBJECT
public:
	explicit AlterozoomNoCookieJar(QObject *parent=nullptr);
	virtual QList<QNetworkCookie> cookiesForUrl(const QUrl &url) const override;
	virtual bool setCookiesFromUrl(const QList<QNetworkCookie> &cookieList,const QUrl &url) override;
	virtual bool insertCookie(const QNetworkCookie &cookie) override;
	virtual bool updateCookie(const QNetworkCookie &cookie) override;
	virtual bool deleteCookie(const QNetworkCookie &cookie) override;

private:
	bool checkIfTargetCookie(const QNetworkCookie &c);
};

#endif // ALTEROZOOMNOCOOKIEJAR_H
