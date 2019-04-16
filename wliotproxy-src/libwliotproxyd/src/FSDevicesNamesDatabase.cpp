#include "wliot/FSDevicesNamesDatabase.h"
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QVariant>

FSDevicesNamesDatabase::FSDevicesNamesDatabase(QObject *parent)
	:IDevicesNamesDatabase(parent)
{
	db=QSqlDatabase::addDatabase("QSQLITE","FSDevicesNamesDatabase");
}

FSDevicesNamesDatabase::~FSDevicesNamesDatabase()
{
	close();
	db=QSqlDatabase();
	QSqlDatabase::removeDatabase("FSDevicesNamesDatabase");
}

bool FSDevicesNamesDatabase::initDb(const QString &path)
{
	close();
	dbPath=path;
	db.setDatabaseName(dbPath);
	if(!db.open())return false;
	QSqlQuery q(db);
	return q.exec("create table if not exists tblDevNames (uid unique on conflict replace,name,manualSetName);");
}

void FSDevicesNamesDatabase::close()
{
	if(!db.isOpen())return;
	db.close();
	dbPath.clear();
}

QByteArray FSDevicesNamesDatabase::deviceName(const QUuid &uid)
{
	if(!db.isOpen())return QByteArray();
	QSqlQuery q(db);
	q.prepare("select name from tblDevNames where uid=:uid");
	q.bindValue(":uid",uid.toString());
	if(!q.exec()||!q.next())
		return QByteArray();
	return q.value(0).toString().toUtf8();
}

QByteArrayList FSDevicesNamesDatabase::devicesNames(const QList<QUuid> &uids)
{
	if(!db.isOpen())return QByteArrayList();
	QByteArrayList retVal;
	for(const QUuid &id:uids)
		retVal.append(deviceName(id));
	return retVal;
}

QUuid FSDevicesNamesDatabase::findDevice(const QByteArray &name)
{
	if(!db.isOpen())return QByteArray();
	QSqlQuery q(db);
	q.prepare("select uid from tblDevNames where manualSetName=:name or (manualSetName='' and name=:name);");
	q.bindValue(":name",QString::fromUtf8(name));
	if(!q.exec()||!q.next())
		return QUuid();
	return QUuid(q.value(0).toString());
}

bool FSDevicesNamesDatabase::setManualDevName(const QUuid &uid,const QByteArray &name)
{
	if(!db.isOpen())return false;
	QSqlQuery q(db);
	if(exists(uid))
		q.prepare("update tblDevNames set manualSetName=:name where uid=:uid;");
	else q.prepare("insert into tblDevNames (uid,manualSetName)values(:uid,:name);");
	q.bindValue(":uid",uid.toString());
	q.bindValue(":name",QString::fromUtf8(name));
	return q.exec();
}

void FSDevicesNamesDatabase::onDeviceIdentified(const QUuid &uid,const QByteArray &name)
{
	if(!db.isOpen())return;
	QSqlQuery q(db);
	if(exists(uid))
		q.prepare("update tblDevNames set name=:name where uid=:uid;");
	else q.prepare("insert into tblDevNames (uid,name)values(:uid,:name);");
	q.bindValue(":uid",uid.toString());
	q.bindValue(":name",QString::fromUtf8(name));
	q.exec();
}

bool FSDevicesNamesDatabase::exists(const QUuid &uid)
{
	if(!db.isOpen())return false;
	QSqlQuery q(db);
	q.prepare("select uid from tblDevNames where uid=:uid");
	q.bindValue(":uid",uid.toString());
	if(!q.exec())return false;
	return q.next();
}
