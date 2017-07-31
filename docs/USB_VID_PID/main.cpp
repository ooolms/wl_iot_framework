#include <QCoreApplication>
#include <QFile>
#include <QDomDocument>
#include <QDomElement>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QVariant>

int main(int argc,char *argv[])
{
	QCoreApplication app(argc,argv);
	QFile file("USB_VIDS_PIDS.txt");
	if(!file.open(QIODevice::ReadOnly))return 1;
	QFile("USB_VIDS_PIDS.sqlite").open(QIODevice::WriteOnly);
	QSqlDatabase db=QSqlDatabase::addDatabase("QSQLITE");
	db.setDatabaseName("USB_VIDS_PIDS.sqlite");
	db.open();
	db.exec("create table vendors (vid,title);");
	db.exec("create table products (vid,pid,title);");
	QDomDocument xml;
	QDomElement rootElem=xml.createElement("usb-vendors");
	xml.appendChild(rootElem);
	QDomElement vendorElem;
	QString currentVid;
	while(!file.atEnd())
	{
		QString str=QString::fromUtf8(file.readLine());
		str.chop(1);
		if(!str.startsWith('\t'))//vendor
		{
			currentVid=str.mid(0,4);
			QString title=str.mid(6);
			vendorElem=xml.createElement("vendor");
			rootElem.appendChild(vendorElem);
			vendorElem.setAttribute("vid",currentVid);
			vendorElem.setAttribute("title",title);
			QSqlQuery q(db);
			q.prepare("insert into vendors (vid,title) values (:vid,:title);");
			q.bindValue(":vid",currentVid);
			q.bindValue(":title",title);
			q.exec();
		}
		else
		{
			QString pid=str.mid(1,4);
			QString title=str.mid(7);
			QDomElement prodElem=xml.createElement("product");
			vendorElem.appendChild(prodElem);
			prodElem.setAttribute("pid",pid);
			prodElem.setAttribute("title",title);
			QSqlQuery q(db);
			q.prepare("insert into products (vid,pid,title) values (:vid,:pid,:title);");
			q.bindValue(":vid",currentVid);
			q.bindValue(":pid",pid);
			q.bindValue(":title",title);
			q.exec();
		}
	}
	file.close();
	QFile file2("USB_VIDS_PIDS.xml");
	file2.open(QIODevice::WriteOnly);
	file2.write(xml.toByteArray());
	file2.close();
	return 0;
}

