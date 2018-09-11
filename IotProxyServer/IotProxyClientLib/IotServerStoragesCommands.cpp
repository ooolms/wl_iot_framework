#include "IotServerStoragesCommands.h"

IotServerStoragesCommands::IotServerStoragesCommands(IotServerConnection *conn,QObject *parent)
	:QObject(parent)
{
	srvConn=conn;
}

bool IotServerStoragesCommands::listStorages(QList<IotServerStorageDescr> &stors)
{
	stors.clear();
	IotServerConnection::CmDataCallback cb=[&stors](const QByteArrayList &args)->bool
	{
		IotServerStorageDescr st;
		if(!storageFromArgs(args,st))return false;
		stors.append(st);
		return true;
	};
	if(!srvConn->execCommand("list_storages",QByteArrayList(),cb))
		return false;
	return true;
}

bool IotServerStoragesCommands::addStorage(
	const QByteArray &devIdOrName,const QByteArray &sensorName,ARpcISensorStorage::StoreMode storeMode,
	ARpcISensorStorage::TimestampRule tsRule,int valuesCount)
{
	if(storeMode==ARpcISensorStorage::LAST_N_VALUES||storeMode==ARpcISensorStorage::LAST_N_VALUES_IN_MEMORY)
		return srvConn->execCommand("add_storage",QByteArrayList()<<devIdOrName<<sensorName<<
			ARpcISensorStorage::storeModeToString(storeMode)<<ARpcISensorStorage::timestampRuleToString(tsRule)<<
			QByteArray::number(valuesCount));
	else return srvConn->execCommand("add_storage",QByteArrayList()<<devIdOrName<<sensorName<<
	ARpcISensorStorage::storeModeToString(storeMode)<<ARpcISensorStorage::timestampRuleToString(tsRule));
}

bool IotServerStoragesCommands::removeStorage(const QByteArray &devIdOrName, const QByteArray &sensorName)
{
	return srvConn->execCommand("remove_storage",QByteArrayList()<<devIdOrName<<sensorName);
}

bool IotServerStoragesCommands::bindStorage(
	const QByteArray &devIdOrName,const QByteArray &sensorName,const QByteArray &serviceName,
	const QMap<QByteArray,QByteArray> &serviceConfig)
{
	QByteArrayList serviceConfigArgs;
	for(auto i=serviceConfig.begin();i!=serviceConfig.end();++i)
		serviceConfigArgs.append(i.key()+":"+i.value());
	return srvConn->execCommand("bind_storage",QByteArrayList()<<devIdOrName<<sensorName<<serviceName<<
		serviceConfigArgs);
}

bool IotServerStoragesCommands::listSessions(const QByteArray &devIdOrName,const QByteArray &sensorName,
	QList<QUuid> &ids,QByteArrayList &titles)
{
	ids.clear();
	titles.clear();
	QByteArrayList retVal;
	IotServerConnection::CmDataCallback cb=[this,&ids,&titles](const QByteArrayList &args)
	{
		if(args.count()<2)
			return false;
		QUuid sId=QUuid(args[0]);
		if(sId.isNull())
			return false;
		ids.append(sId);
		titles.append(args[1]);
		return true;
	};
	return srvConn->execCommand("session_list",QByteArrayList()<<devIdOrName<<sensorName,retVal,cb);
}

bool IotServerStoragesCommands::listSessionAttrs(
	const QByteArray &devIdOrName,const QByteArray &sensorName,const QUuid &sessionId,
	QMap<QByteArray,QByteArray> &attrs)
{
	attrs.clear();
	QByteArrayList retVal;
	IotServerConnection::CmDataCallback cb=[this,&attrs](const QByteArrayList &args)
	{
		if(args.count()<2||args[0].isEmpty())
			return false;
		attrs[args[0]]=args[1];
		return true;
	};
	return srvConn->execCommand("session_list_attrs",QByteArrayList()<<devIdOrName<<sensorName<<
		sessionId.toByteArray(),retVal,cb);
}

bool IotServerStoragesCommands::getSessionAttr(const QByteArray &devIdOrName,const QByteArray &sensorName,
	const QUuid &sessionId,const QByteArray &attrName,QByteArray &value)
{
	QByteArrayList retVal;
	if(!srvConn->execCommand("session_get_attr",QByteArrayList()<<devIdOrName<<sensorName<<sessionId.toByteArray()<<
		attrName,retVal)||retVal.isEmpty())
		return false;
	value=retVal[0];
	return true;
}

bool IotServerStoragesCommands::setSessionAttr(const QByteArray &devIdOrName,const QByteArray &sensorName,
	const QUuid &sessionId,const QByteArray &attrName,const QByteArray &value)
{
	return srvConn->execCommand("session_set_attr",QByteArrayList()<<devIdOrName<<sensorName<<sessionId.toByteArray()<<
		attrName<<value);
}

bool IotServerStoragesCommands::removeSessionAttr(const QByteArray &devIdOrName,const QByteArray &sensorName,
	const QUuid &sessionId,const QByteArray &attrName)
{
	return srvConn->execCommand("session_set_attr",QByteArrayList()<<devIdOrName<<sensorName<<sessionId.toByteArray()<<
		attrName);
}

bool IotServerStoragesCommands::startSession(
	const QByteArray &devIdOrName,const QByteArray &sensorName,const QUuid &sessionId)
{
	return srvConn->execCommand("session_start",QByteArrayList()<<devIdOrName<<sensorName<<sessionId.toByteArray());
}

bool IotServerStoragesCommands::createSession(
	const QByteArray &devIdOrName,const QByteArray &sensorName,const QByteArray &sessionTitle,QUuid &sessionId)
{
	QByteArrayList retVal;
	if(!srvConn->execCommand("session_create",QByteArrayList()<<devIdOrName<<sensorName<<sessionTitle,retVal)||
		retVal.isEmpty())
		return false;
	sessionId=QUuid(retVal[0]);
	return !sessionId.isNull();
}

bool IotServerStoragesCommands::stopSession(const QByteArray &devIdOrName,const QByteArray &sensorName)
{
	return srvConn->execCommand("session_stop",QByteArrayList()<<devIdOrName<<sensorName);
}

bool IotServerStoragesCommands::removeSession(const QByteArray &devIdOrName,const QByteArray &sensorName,
	const QUuid &sessionId)
{
	return srvConn->execCommand("session_remove",QByteArrayList()<<devIdOrName<<sensorName<<sessionId.toByteArray());
}

bool IotServerStoragesCommands::continueSession(const QByteArray &devIdOrName,const QByteArray &sensorName,
	const QUuid &sessionId)
{
	return srvConn->execCommand("session_continue",QByteArrayList()<<devIdOrName<<sensorName<<sessionId.toByteArray());
}

bool IotServerStoragesCommands::getSessionWriteId(const QByteArray &devIdOrName,const QByteArray &sensorName,
	QUuid &sessionId)
{
	QByteArrayList retVal;
	if(!srvConn->execCommand("session_get_write_id",QByteArrayList()<<devIdOrName<<sensorName,retVal)||
		retVal.isEmpty())
		return false;
	sessionId=QUuid(retVal[0]);
	return !sessionId.isNull();
}

bool IotServerStoragesCommands::getSamplesCount(
	const QByteArray &devIdOrName,const QByteArray &sensorName,quint64 &count,const QUuid &sessionId)
{
	QByteArrayList retVal;
	QByteArrayList args=QByteArrayList()<<devIdOrName<<sensorName;
	if(!sessionId.isNull())
		args.append(sessionId.toByteArray());
	if(!srvConn->execCommand("get_samples_count",args,retVal)||retVal.isEmpty())
		return false;
	bool ok=false;
	count=retVal[0].toULongLong(&ok);
	return ok;
}

bool IotServerStoragesCommands::getSamples(const QByteArray &devIdOrName,const QByteArray &sensorName,
	quint64 startIndex,quint64 count,quint64 step,const ARpcSensorDef::Type &sensorType,
	VeryBigArray<ARpcSensorValue*> &values,const QUuid &sessionId)
{
	if(step==0)step=1;
	values.clear();
	IotServerConnection::CmDataCallback cb=[this,&values,&sensorType](const QByteArrayList &args)
	{
		ARpcSensorValue *val=ARpcSensorValue::createSensorValue(sensorType);
		if(!val)return false;
		if(!val->parseMsgArgs(args))return false;
		values.append(val);
		return true;
	};
	QByteArrayList args=QByteArrayList()<<devIdOrName<<sensorName;
	if(!sessionId.isNull())
		args.append(sessionId.toByteArray());
	args.append(QByteArray::number(startIndex));
	args.append(QByteArray::number(count));
	args.append(QByteArray::number(step));
	return srvConn->execCommand("get_samples",args,cb);
}

bool IotServerStoragesCommands::registerVirtualDevice(
	const QUuid &deviceId,const QByteArray &deviceName,const QByteArray &sensorsDef)
{
	return srvConn->execCommand("register_virtual_device",
		QByteArrayList()<<deviceId.toByteArray()<<deviceName<<sensorsDef);
}

bool IotServerStoragesCommands::sendVDevMeasurement(
	const QUuid &deviceId,const QByteArray &sensorName,const QByteArrayList &measArgs)
{
	return srvConn->execCommand("vdev_meas",
		QByteArrayList()<<deviceId.toByteArray()<<sensorName<<measArgs);
}

bool IotServerStoragesCommands::storageFromArgs(const QByteArrayList &args,IotServerStorageDescr &st)
{
	if(args.count()<8)return false;
	QByteArrayList attributesStrs=args[4].split(';');
	QMap<QByteArray,QByteArray> attributes;
	for(QByteArray &s:attributesStrs)
	{
		int index=s.indexOf('=');
		if(index==-1)continue;
		attributes[s.mid(0,index)]=s.mid(index+1);
	}
	st.deviceId=QUuid(args[0]);
	st.deviceName=args[1];
	st.sensor.name=args[2];
	st.sensor.type.fromString(args[3]);
	st.sensor.attributes=attributes;
	st.mode=ARpcISensorStorage::storeModeFromString(args[5]);
	if(!ARpcISensorStorage::timestampRuleFromString(args[6],st.tsRule))
		return false;
	st.storedValuesType.fromString(args[7]);
	if(st.deviceId.isNull()||st.mode==ARpcISensorStorage::BAD_MODE||!st.sensor.type.isValid()||
		st.sensor.name.isEmpty()||!st.storedValuesType.isValid())
		return false;
	return true;
}
