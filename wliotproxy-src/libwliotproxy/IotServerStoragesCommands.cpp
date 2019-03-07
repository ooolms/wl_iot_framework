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

#include "IotServerStoragesCommands.h"

IotServerStoragesCommands::IotServerStoragesCommands(IotServerConnection *conn,QObject *parent)
	:QObject(parent)
{
	srvConn=conn;
}

bool IotServerStoragesCommands::listStorages(QList<IotServerStorageDescr> &stors)
{
	stors.clear();
	CmDataCallback cb=[&stors](const QByteArrayList &args)->bool
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
	const QByteArray &devIdOrName,const QByteArray &sensorName,ISensorStorage::StoreMode storeMode,
	ISensorStorage::TimestampRule tsRule,int valuesCount)
{
	if(storeMode==ISensorStorage::LAST_N_VALUES||storeMode==ISensorStorage::LAST_N_VALUES_IN_MEMORY)
		return srvConn->execCommand("add_storage",QByteArrayList()<<devIdOrName<<sensorName<<
			ISensorStorage::storeModeToString(storeMode)<<ISensorStorage::timestampRuleToString(tsRule)<<
			QByteArray::number(valuesCount));
	else return srvConn->execCommand("add_storage",QByteArrayList()<<devIdOrName<<sensorName<<
		ISensorStorage::storeModeToString(storeMode)<<ISensorStorage::timestampRuleToString(tsRule));
}

bool IotServerStoragesCommands::removeStorage(const QByteArray &devIdOrName, const QByteArray &sensorName)
{
	return srvConn->execCommand("remove_storage",QByteArrayList()<<devIdOrName<<sensorName);
}

bool IotServerStoragesCommands::storageAddDataExport(
	const QByteArray &devIdOrName,const QByteArray &sensorName,const QByteArray &serviceName,
	const QMap<QByteArray,QByteArray> &serviceConfig)
{
	QByteArrayList serviceConfigArgs;
	for(auto i=serviceConfig.begin();i!=serviceConfig.end();++i)
		serviceConfigArgs.append(i.key()+":"+i.value());
	return srvConn->execCommand("storage_add_data_export",QByteArrayList()<<devIdOrName<<sensorName<<serviceName<<
		serviceConfigArgs);
}

bool IotServerStoragesCommands::storageGetDataExport(
	const QByteArray &devIdOrName,const QByteArray &sensorName,const QByteArray &serviceName,
	QMap<QByteArray,QByteArray> &serviceConfig)
{
	QByteArrayList retVal;
	if(!srvConn->execCommand("storage_get_data_export",QByteArrayList()<<devIdOrName<<sensorName<<serviceName))
		return false;
	serviceConfig.clear();
	for(QByteArray s:retVal)
	{
		int index=s.indexOf(":");
		if(index<=0)
			return false;
		serviceConfig[s.left(index)]=s.mid(index+1);
	}
	return true;
}

bool IotServerStoragesCommands::storageAllDataExports(
	const QByteArray &devIdOrName,const QByteArray &sensorName,QByteArrayList &services)
{
	return srvConn->execCommand("storage_get_data_export_list",QByteArrayList()<<devIdOrName<<sensorName,services);
}

bool IotServerStoragesCommands::listSessions(const QByteArray &devIdOrName,const QByteArray &sensorName,
	QList<QUuid> &ids,QByteArrayList &titles)
{
	ids.clear();
	titles.clear();
	QByteArrayList retVal;
	CmDataCallback cb=[this,&ids,&titles](const QByteArrayList &args)
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
	CmDataCallback cb=[this,&attrs](const QByteArrayList &args)
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
	quint64 startIndex,quint64 count,quint64 step,const SensorDef::Type &sensorType,
	VeryBigArray<SensorValue*> &values,const QUuid &sessionId)
{
	if(step==0)step=1;
	values.clear();
	CmDataCallback cb=[this,&values,&sensorType](const QByteArrayList &args)
	{
		SensorValue *val=SensorValue::createSensorValue(sensorType);
		if(!val)return false;
		if(args.count()!=1)return false;
		if(!val->parseBinary(args[0]))return false;
		values.append(val);
		return true;
	};
	QByteArrayList args=QByteArrayList()<<devIdOrName<<sensorName;
	if(!sessionId.isNull())
		args.append(sessionId.toByteArray());
	args.append(QByteArray::number(startIndex));
	args.append(QByteArray::number(count));
	args.append(QByteArray::number(step));
	return srvConn->execCommand("get_samples_bin",args,cb);
}

bool IotServerStoragesCommands::setStorageAttr(
	const QByteArray &devIdOrName,const QByteArray &sensorName,const QByteArray &attrName,const QByteArray &attrValue)
{
	return srvConn->execCommand("storage_set_attr",QByteArrayList()<<devIdOrName<<sensorName<<attrName<<attrValue);
}

bool IotServerStoragesCommands::getStorageAttr(
	const QByteArray &devIdOrName,const QByteArray &sensorName,const QByteArray &attrName,QByteArray &attrValue)
{
	QByteArrayList retVal;
	if(!srvConn->execCommand("storage_get_attr",QByteArrayList()<<devIdOrName<<sensorName<<attrName,retVal)||
		retVal.isEmpty())
			return false;
	attrValue=retVal[0];
	return true;
}

bool IotServerStoragesCommands::availableDataExportServices(QByteArrayList &services)
{
	QByteArrayList retVal;
	if(!srvConn->execCommand("available_data_export_services",QByteArrayList(),retVal))return false;
	services=retVal;
	return true;
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
	st.mode=ISensorStorage::storeModeFromString(args[5]);
	if(!ISensorStorage::timestampRuleFromString(args[6],st.tsRule))
		return false;
	st.storedValuesType.fromString(args[7]);
	if(st.deviceId.isNull()||st.mode==ISensorStorage::BAD_MODE||!st.sensor.type.isValid()||
		st.sensor.name.isEmpty()||!st.storedValuesType.isValid())
		return false;
	return true;
}
