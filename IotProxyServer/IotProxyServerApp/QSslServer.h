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

#ifndef QSSLSERVER_H
#define QSSLSERVER_H

#include <QTcpServer>
#include <QSslCertificate>
#include <QSslKey>

class QSslServer
	:public QTcpServer
{
	Q_OBJECT
public:
	explicit QSslServer(QObject *parent=0);
	void setSslOptions(const QSslCertificate &c,const QSslKey &k);

protected:
	virtual void incomingConnection(qintptr socketDescriptor);

private:
	QSslCertificate crt;
	QSslKey key;
};

#endif // QSSLSERVER_H
