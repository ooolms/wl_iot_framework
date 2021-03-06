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

#include "wliot/client/StoragesCommands.h"

using namespace WLIOT;
using namespace WLIOTClient;

StoragesCommands::StoragesCommands(ServerConnection *conn)
	:QObject(conn)
{
	srvConn=conn;
}

bool StoragesCommands::listStorages(QList<StorageDescr> &stors)
{
	stors.clear();
	CmDataCallback cb=[&stors](const QByteArrayList &args)->bool
	{
		StorageDescr st;
		if(!storageFromArgs(args,st))return false;
		stors.append(st);
		return true;
	};
	if(!srvConn->execCommand("list_storages",QByteArrayList(),cb))
		return false;
	return true;
}

bool StoragesCommands::addStorage(
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

bool StoragesCommands::removeStorage(const QByteArray &devIdOrName, const QByteArray &sensorName)
{
	return srvConn->execCommand("remove_storage",QByteArrayList()<<devIdOrName<<sensorName);
}

bool StoragesCommands::storageAddDataExport(
	const QByteArray &devIdOrName,const QByteArray &sensorName,const QUuid &serviceId,
	const QMap<QByteArray,QByteArray> &serviceConfig)
{
	QByteArrayList serviceConfigArgs;
	for(auto i=serviceConfig.begin();i!=serviceConfig.end();++i)
		serviceConfigArgs.append(i.key()+":"+i.value());
	return srvConn->execCommand("storage_add_data_export",QByteArrayList()<<devIdOrName<<sensorName<<
		serviceId.toByteArray()<<serviceConfigArgs);
}

bool StoragesCommands::storageGetDataExport(
	const QByteArray &devIdOrName,const QByteArray &sensorName,const QUuid &serviceId,
	QMap<QByteArray,QByteArray> &serviceConfig)
{
	QByteArrayList retVal;
	if(!srvConn->execCommand("storage_get_data_export",
		QByteArrayList()<<devIdOrName<<sensorName<<serviceId.toByteArray()))
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

bool StoragesCommands::storageAllDataExports(
	const QByteArray &devIdOrName,const QByteArray &sensorName,QList<QUuid> &services)
{
	CmDataCallback cb=[&services](const QByteArrayList &args)->bool
	{
		if(args.isEmpty())return false;
		QUuid uid(args[0]);
		if(uid.isNull())return false;
		services.append(uid);
		return true;
	};
	services.clear();
	return srvConn->execCommand("storage_get_data_export_list",QByteArrayList()<<devIdOrName<<sensorName,cb);
}

bool StoragesCommands::listSessions(const QByteArray &devIdOrName,const QByteArray &sensorName,
	QList<QUuid> &ids,QByteArrayList &titles)
{
	ids.clear();
	titles.clear();
	QByteArrayList retVal;
	CmDataCallback cb=[&ids,&titles](const QByteArrayList &args)->bool
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

bool StoragesCommands::listSessionAttrs(
	const QByteArray &devIdOrName,const QByteArray &sensorName,const QUuid &sessionId,
	QMap<QByteArray,QByteArray> &attrs)
{
	attrs.clear();
	QByteArrayList retVal;
	CmDataCallback cb=[&attrs](const QByteArrayList &args)->bool
	{
		if(args.count()<2||args[0].isEmpty())
			return false;
		attrs[args[0]]=args[1];
		return true;
	};
	return srvConn->execCommand("session_list_attrs",QByteArrayList()<<devIdOrName<<sensorName<<
		sessionId.toByteArray(),retVal,cb);
}

bool StoragesCommands::getSessionAttr(const QByteArray &devIdOrName,const QByteArray &sensorName,
	const QUuid &sessionId,const QByteArray &attrName,QByteArray &value)
{
	QByteArrayList retVal;
	if(!srvConn->execCommand("session_get_attr",QByteArrayList()<<devIdOrName<<sensorName<<sessionId.toByteArray()<<
		attrName,retVal)||retVal.isEmpty())
		return false;
	value=retVal[0];
	return true;
}

bool StoragesCommands::setSessionAttr(const QByteArray &devIdOrName,const QByteArray &sensorName,
	const QUuid &sessionId,const QByteArray &attrName,const QByteArray &value)
{
	return srvConn->execCommand("session_set_attr",QByteArrayList()<<devIdOrName<<sensorName<<sessionId.toByteArray()<<
		attrName<<value);
}

bool StoragesCommands::removeSessionAttr(const QByteArray &devIdOrName,const QByteArray &sensorName,
	const QUuid &sessionId,const QByteArray &attrName)
{
	return srvConn->execCommand("session_set_attr",QByteArrayList()<<devIdOrName<<sensorName<<sessionId.toByteArray()<<
		attrName);
}

bool StoragesCommands::startSession(
	const QByteArray &devIdOrName,const QByteArray &sensorName,const QUuid &sessionId)
{
	return srvConn->execCommand("session_start",QByteArrayList()<<devIdOrName<<sensorName<<sessionId.toByteArray());
}

bool StoragesCommands::createSession(
	const QByteArray &devIdOrName,const QByteArray &sensorName,const QByteArray &sessionTitle,QUuid &sessionId)
{
	QByteArrayList retVal;
	if(!srvConn->execCommand("session_create",QByteArrayList()<<devIdOrName<<sensorName<<sessionTitle,retVal)||
		retVal.isEmpty())
		return false;
	sessionId=QUuid(retVal[0]);
	return !sessionId.isNull();
}

bool StoragesCommands::stopSession(const QByteArray &devIdOrName,const QByteArray &sensorName)
{
	return srvConn->execCommand("session_stop",QByteArrayList()<<devIdOrName<<sensorName);
}

bool StoragesCommands::removeSession(const QByteArray &devIdOrName,const QByteArray &sensorName,
	const QUuid &sessionId)
{
	return srvConn->execCommand("session_remove",QByteArrayList()<<devIdOrName<<sensorName<<sessionId.toByteArray());
}

bool StoragesCommands::continueSession(const QByteArray &devIdOrName,const QByteArray &sensorName,
	const QUuid &sessionId)
{
	return srvConn->execCommand("session_continue",QByteArrayList()<<devIdOrName<<sensorName<<sessionId.toByteArray());
}

bool StoragesCommands::getSessionWriteId(const QByteArray &devIdOrName,const QByteArray &sensorName,
	QUuid &sessionId)
{
	QByteArrayList retVal;
	if(!srvConn->execCommand("session_get_write_id",QByteArrayList()<<devIdOrName<<sensorName,retVal)||
		retVal.isEmpty())
		return false;
	sessionId=QUuid(retVal[0]);
	return !sessionId.isNull();
}

bool StoragesCommands::getSamplesCount(
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

bool StoragesCommands::getSamples(const QByteArray &devIdOrName,const QByteArray &sensorName,
	quint64 startIndex,quint64 count,quint64 step,const SensorDef::Type &sensorType,
	VeryBigArray<SensorValue*> &values,const QUuid &sessionId)
{
	if(count<=100000)
		return getSamplesRaw(devIdOrName,sensorName,startIndex,count,step,sensorType,values,sessionId);
	else return getSamplesBin(devIdOrName,sensorName,startIndex,count,step,sensorType,values,sessionId);
}

bool StoragesCommands::getSamplesBin(const QByteArray &devIdOrName,const QByteArray &sensorName,
	quint64 startIndex,quint64 count,quint64 step,const SensorDef::Type &sensorType,
	VeryBigArray<SensorValue*> &values,const QUuid &sessionId)
{
	if(step==0)step=1;
	values.clear();
	CmDataCallback cb=[&values,&sensorType](const QByteArrayList &args)
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

bool StoragesCommands::getSamplesRaw(const QByteArray &devIdOrName,const QByteArray &sensorName,
	quint64 startIndex,quint64 count,quint64 step,const SensorDef::Type &sensorType,
	VeryBigArray<SensorValue*> &values,const QUuid &sessionId)
{
	if(step==0)
		step=1;
	values.clear();
	int valSz=sensorValBinSize(sensorType);
	if(valSz==0)return false;
	QByteArrayList args=QByteArrayList()<<devIdOrName<<sensorName;
	if(!sessionId.isNull())
		args.append(sessionId.toByteArray());
	args.append(QByteArray::number(startIndex));
	args.append(QByteArray::number(count));
	args.append(QByteArray::number(step));
	QByteArrayList retVal;
	if(!srvConn->execCommand("get_samples_raw",args,retVal)||retVal.count()!=1)
		return false;
	if(retVal[0].size()%valSz!=0)return false;
	int retCount=retVal[0].size()/valSz;
	for(int i=0,sz=0;i<retCount;++i,sz+=valSz)
	{
		SensorValue *v=SensorValue::createSensorValue(sensorType);
		if(v->parseBinary(retVal[0].mid(sz,valSz)))
			values.append(v);
	}
	return true;
}

int StoragesCommands::sensorValBinSize(const SensorDef::Type &type)
{
	if(type.packType==SensorDef::PACKET||type.numType==SensorDef::TEXT)return 0;
	int sz=0;
	if(type.tsType!=SensorDef::NO_TIME)
		sz+=sizeof(quint64);
	if(type.numType==SensorDef::U8||type.numType==SensorDef::S8)
		sz+=sizeof(quint8);
	else if(type.numType==SensorDef::U16||type.numType==SensorDef::S16)
		sz+=sizeof(quint16);
	else if(type.numType==SensorDef::U32||type.numType==SensorDef::S32)
		sz+=sizeof(quint32);
	else if(type.numType==SensorDef::U64||type.numType==SensorDef::S64)
		sz+=sizeof(quint64);
	else if(type.numType==SensorDef::F32)
		sz+=sizeof(float);
	else if(type.numType==SensorDef::F64)
		sz+=sizeof(double);
	return sz;
}

bool StoragesCommands::setStorageAttr(
	const QByteArray &devIdOrName,const QByteArray &sensorName,const QByteArray &attrName,const QByteArray &attrValue)
{
	return srvConn->execCommand("storage_set_attr",QByteArrayList()<<devIdOrName<<sensorName<<attrName<<attrValue);
}

bool StoragesCommands::getStorageAttr(
	const QByteArray &devIdOrName,const QByteArray &sensorName,const QByteArray &attrName,QByteArray &attrValue)
{
	QByteArrayList retVal;
	if(!srvConn->execCommand("storage_get_attr",QByteArrayList()<<devIdOrName<<sensorName<<attrName,retVal)||
		retVal.isEmpty())
			return false;
	attrValue=retVal[0];
	return true;
}

bool StoragesCommands::availableDataExportServices(QList<DataExportServiceDescr> &services)
{
	auto onCmDataFunc=[&services](const QByteArrayList &args)->bool
	{
		if(args.count()<3)return false;
		QUuid uid=args[0];
		if(uid.isNull()||args[1].isEmpty())
			return false;
		DataExportServiceDescr d={uid,args[1],args.mid(2)};
		d.paramNames.removeAll(QByteArray());
		if(d.paramNames.isEmpty())return false;
		services.append(d);
		return true;
	};
	services.clear();
	return srvConn->execCommand("available_data_export_services",QByteArrayList(),onCmDataFunc);
}

bool StoragesCommands::alterozoomAuth(const QByteArray &host,const QByteArray &email,const QByteArray &pass)
{
	return srvConn->execCommand("alterozoom_auth",QByteArrayList()<<host<<email<<pass);
}

bool StoragesCommands::alterozoomList(QList<QPair<QByteArray,QByteArray>> &hostEmailList)
{
	auto onCmDataFunc=[&hostEmailList](const QByteArrayList &args)->bool
	{
		if(args.count()<2)return false;
		if(!args[0].isEmpty()&&!args[1].isEmpty())
			hostEmailList.append(qMakePair(args[0],args[1]));
		return true;
	};
	hostEmailList.clear();
	return srvConn->execCommand("alterozoom_list",QByteArrayList(),onCmDataFunc);
}

bool StoragesCommands::storageFromArgs(const QByteArrayList &args,StorageDescr &st)
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
	st.tsRule=ISensorStorage::timestampRuleFromString(args[6]);
	st.storedValuesType.fromString(args[7]);
	if(st.deviceId.isNull()||st.mode==ISensorStorage::INVALID_MODE||st.tsRule==ISensorStorage::INVALID_RULE||
		!st.sensor.type.isValid()||st.sensor.name.isEmpty()||!st.storedValuesType.isValid())
		return false;
	return true;
}
