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

#include "ARpcLocalDatabase.h"
#include "ARpcAllStorages.h"
#include "ARpcBase/ARpcAllSensorValues.h"

static const int metaTypeIdForDeviceAndSensorId=qMetaTypeId<DeviceStorageId>();

ARpcLocalDatabase::ARpcLocalDatabase(QObject *parent)
	:QObject(parent)
{
	mOpened=false;
}

bool ARpcLocalDatabase::open(const QString &path)
{
	close();
	dbDir=QDir(path);
	if(!dbDir.exists())
		return false;
	QStringList dirs=dbDir.entryList(QDir::Dirs|QDir::NoDotAndDotDot);
	for(QString &d:dirs)
	{
		QString path=dbDir.absolutePath()+"/"+d;
		ARpcISensorStorage *st=ARpcISensorStorage::preOpen(path);
		if(!st)
			continue;
		DeviceStorageId id={st->deviceId(),st->sensor().name};
		if(storagesIds.contains(id))
		{
			delete st;
			continue;
		}
		else
		{
			storagesIds.append(id);
			storages.append(st);
		}
	}
	mOpened=true;
	emit opened();
	return true;
}

void ARpcLocalDatabase::close()
{
	if(!mOpened)
		return;
	mOpened=false;
	emit closed();
	for(auto v:storages)
		delete v;
	storages.clear();
	storagesIds.clear();
}

bool ARpcLocalDatabase::isOpened()
{
	return mOpened;
}

bool ARpcLocalDatabase::listSensors(QList<DeviceStorageId> &list)
{
	if(!mOpened)
		return false;
	list=storagesIds;
	return true;
}

bool ARpcLocalDatabase::listSensorsWithDevNames(QList<DeviceStorageId> &list,QStringList &titles)
{
	if(!mOpened)
		return false;
	list.clear();
	titles.clear();
	for(int i=0;i<storagesIds.count();++i)
	{
		list.append(storagesIds[i]);
		titles.append(storages[i]->deviceName());
	}
	return true;
}

ARpcISensorStorage* ARpcLocalDatabase::existingStorage(const DeviceStorageId &id)
{
	if(!mOpened)
		return 0;
	int index=storagesIds.indexOf(id);
	if(index==-1)
		return 0;
	return storages[index];
}

ARpcISensorStorage* ARpcLocalDatabase::preCreate(const QUuid &devId,const QString &devName,
	ARpcISensorStorage::StoreMode storeMode,const ARpcSensor &sensor,
	ARpcISensorStorage::TimestampRule rule)
{
	if(!mOpened)
		return 0;
	DeviceStorageId id={devId,sensor.name};
	if(devId.isNull()||sensor.name.isEmpty()||storagesIds.contains(id))
		return 0;
	QString path=dbDir.absolutePath()+"/"+devId.toString()+"_"+sensor.name;
	QFileInfo info(path);
	if(info.exists())
		return 0;
	auto retVal=ARpcISensorStorage::preCreate(path,storeMode,sensor,devId,devName,rule);
	if(retVal)
	{
		storagesIds.append(id);
		storages.append(retVal);
	}
	return retVal;
}

ARpcISensorStorage* ARpcLocalDatabase::create(const QUuid &devId,const QString &devName,
	ARpcISensorStorage::StoreMode mode,const ARpcSensor &sensor,
	ARpcISensorStorage::TimestampRule rule,int nForLastNValues)
{
	ARpcISensorStorage *stor=preCreate(devId,devName,mode,sensor,rule);
	if(!stor)
		return 0;
	quint32 dims=1;
	if(sensor.constraints.contains("dims"))
		dims=sensor.constraints["dims"].toUInt();
	if(dims==0)
		dims=1;
	if(mode==ARpcISensorStorage::CONTINUOUS)
	{
		if(sensor.type==ARpcSensor::SINGLE)
			((ARpcContinuousStorage*)stor)->createAsFixedBlocksDb(ARpcSingleSensorValue(dims),true);
		else if(sensor.type==ARpcSensor::SINGLE_LT)
			((ARpcContinuousStorage*)stor)->createAsFixedBlocksDb(ARpcSingleSensorValue(dims,true),true);
		else if(sensor.type==ARpcSensor::SINGLE_GT)
			((ARpcContinuousStorage*)stor)->createAsFixedBlocksDb(ARpcSingleSensorValue(dims,false),true);
		else
			((ARpcContinuousStorage*)stor)->createAsChainedBlocksDb(true);
	}
	else if(mode==ARpcISensorStorage::AUTO_SESSIONS||mode==ARpcISensorStorage::MANUAL_SESSIONS)
	{
		if(sensor.type==ARpcSensor::SINGLE)
			((ARpcSessionStorage*)stor)->createAsFixedBlocksDb(ARpcSingleSensorValue(dims),true);
		else if(sensor.type==ARpcSensor::SINGLE_LT)
			((ARpcSessionStorage*)stor)->createAsFixedBlocksDb(ARpcSingleSensorValue(dims,true),true);
		else if(sensor.type==ARpcSensor::SINGLE_GT)
			((ARpcSessionStorage*)stor)->createAsFixedBlocksDb(ARpcSingleSensorValue(dims,false),true);
		else
			((ARpcSessionStorage*)stor)->createAsChainedBlocksDb(true);
	}
	else if(mode==ARpcISensorStorage::LAST_N_VALUES)
	{
		if(sensor.type==ARpcSensor::TEXT)
			((ARpcLastNValuesStorage*)stor)->create(nForLastNValues,ARpcTextSensorValue());
		else if(sensor.type==ARpcSensor::SINGLE)
			((ARpcLastNValuesStorage*)stor)->create(nForLastNValues,ARpcSingleSensorValue(dims));
		else if(sensor.type==ARpcSensor::SINGLE_LT)
			((ARpcLastNValuesStorage*)stor)->create(nForLastNValues,ARpcSingleSensorValue(dims,true));
		else if(sensor.type==ARpcSensor::SINGLE_GT)
			((ARpcLastNValuesStorage*)stor)->create(nForLastNValues,ARpcSingleSensorValue(dims,false));
		else if(sensor.type==ARpcSensor::PACKET)
			((ARpcLastNValuesStorage*)stor)->create(nForLastNValues,ARpcPacketSensorValue(dims));
		else if(sensor.type==ARpcSensor::PACKET_LT)
			((ARpcLastNValuesStorage*)stor)->create(nForLastNValues,ARpcPacketSensorValue(dims,true));
		else if(sensor.type==ARpcSensor::PACKET_GT)
			((ARpcLastNValuesStorage*)stor)->create(nForLastNValues,ARpcPacketSensorValue(dims,false));
	}
	creationFinished({devId,sensor.name});
	return stor;
}

bool ARpcLocalDatabase::hasStorage(const DeviceStorageId &id)
{
	return storagesIds.contains(id);
}

bool ARpcLocalDatabase::removeStorage(const DeviceStorageId &id)
{
	if(!mOpened)
		return false;
	int index=storagesIds.indexOf(id);
	if(index==-1)
		return false;
	if(storages[index]->isOpened())
		storages[index]->close();
	QString path=dbDir.absolutePath()+"/"+id.deviceId.toString()+"_"+id.sensorName;
	QDir dir(path);
	if(!rmDirRec(dir))
		return false;
	emit storageRemoved(id);
	delete storages[index];
	storages.removeAt(index);
	storagesIds.removeAt(index);
	return true;
}

void ARpcLocalDatabase::creationFinished(const DeviceStorageId &id)
{
	if(!storagesIds.contains(id))
		return;
	emit storageCreated(id);
}

ARpcISensorStorage* ARpcLocalDatabase::findStorageForDevice(const QString &devIdOrName,const QString &sensorName,
	QUuid &devId)
{
	for(int i=0;i<storagesIds.count();++i)
	{
		if(storagesIds[i].sensorName!=sensorName)
			continue;
		if(storagesIds[i].deviceId.toString()==devIdOrName)
		{
			devId=storagesIds[i].deviceId;
			return storages[i];
		}
		else
		{
			QString devNameFromStorage=storages[i]->deviceName();
			if(!devNameFromStorage.isEmpty()&&devNameFromStorage==devIdOrName)
			{
				devId=storagesIds[i].deviceId;
				return storages[i];
			}
		}
	}
	return 0;
}

bool ARpcLocalDatabase::rmDirRec(QDir dir)
{
	QStringList entries=dir.entryList(QDir::Files|QDir::Hidden|QDir::System);
	for(int i=0;i<entries.count();++i)
		if(!dir.remove(entries[i]))
			return false;
	entries=dir.entryList(QDir::Dirs|QDir::NoDotAndDotDot);
	for(int i=0;i<entries.count();++i)
	{
		QDir chDir=dir;
		if(!chDir.cd(entries[i]))
			return false;
		if(!rmDirRec(chDir))
			return false;
	}
	return dir.rmdir(dir.absolutePath());
}
