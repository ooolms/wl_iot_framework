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
