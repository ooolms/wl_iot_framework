#include "ARpcContinuousStorage.h"
#include "ARpcBase/ARpcPacketSensorValue.h"
#include "ARpcBase/ARpcSingleSensorValue.h"
#include "ARpcBase/ARpcTextSensorValue.h"
#include "ARpcDBDriverHelpers.h"
#include <QDir>
#include <QDateTime>
#include <QSettings>

ARpcContinuousStorage::ARpcContinuousStorage(ARpcSensor::Type valType,QObject *parent)
	:ARpcISensorStorage(valType,parent)
{
	fbDb=new ARpcDBDriverFixedBlocks(this);
	cbDb=new ARpcDBDriverChainedBlocks(this);
	indDb=new ARpcDBDriverGTimeIndex(this);
	opened=false;
	hasIndex=false;
	dbType=CHAINED_BLOCKS;
	timestampRule=ADD_GT;
	effectiveValType=defaultEffectiveValuesType(timestampRule);
}

ARpcContinuousStorage::~ARpcContinuousStorage()
{
	close();
}

ARpcISensorStorage::StoreMode ARpcContinuousStorage::getStoreMode()const
{
	return CONTINUOUS;
}

bool ARpcContinuousStorage::writeSensorValue(const ARpcISensorValue *val)
{
	if(!opened)return false;
	if(val->type()!=valueType)return false;
	int hasTime;
	qint64 ts;
	QByteArray data=hlp.packSensorValue(val,hasTime,ts);
	if(data.isEmpty())return false;
	if(dbType==FIXED_BLOCKS)
	{
		if(fbDb->writeBlock(data))
		{
			if(hasIndex)
				indDb->append(ts,fbDb->blocksCount()-1);
			emit newValueWritten(val);
			return true;
		}
		return false;
	}
	else
	{
		if(cbDb->writeBlock(data))
		{
			if(hasIndex)
				indDb->append(ts,cbDb->blocksCount()-1);
			emit newValueWritten(val);
			return true;
		}
		return false;
	}
}

ARpcSensor::Type ARpcContinuousStorage::effectiveValuesType()const
{
	return effectiveValType;
}

bool ARpcContinuousStorage::createAsFixedBlocksDb(const ARpcISensorValue &templateValue,
	TimestampRule rule,bool gtIndex)
{
	if(opened)return false;
	if(templateValue.type()!=valueType)return false;
	timestampRule=rule;
	if((valueType==ARpcSensor::SINGLE_LT||valueType==ARpcSensor::PACKET_LT)&&
		timestampRule==ARpcISensorStorage::DONT_TOUCH)timestampRule=ARpcISensorStorage::ADD_GT;
	if(!fbDb->create(dbDir.absolutePath()+"/data.db",
		ARpcDBDriverHelpers(timestampRule).sizesForFixedBlocksDb(templateValue)))return false;
	effectiveValType=defaultEffectiveValuesType(timestampRule);
	hasIndex=gtIndex&&(effectiveValType==ARpcSensor::TEXT||effectiveValType==ARpcSensor::SINGLE_GT||
		effectiveValType==ARpcSensor::PACKET_GT);
	if(hasIndex)
	{
		if(!indDb->create(dbDir.absolutePath()+"/index.db"))return false;
	}
	QSettings settings(dbDir.absolutePath()+"/"+settingsFileRelPath(),QSettings::IniFormat);
	settings.setValue("db_type","fixed_blocks");
	settings.setValue("time_rule",timestampRuleToString(timestampRule));
	settings.setValue("gt_index",hasIndex?"1":"0");
	settings.sync();
	dbType=FIXED_BLOCKS;
	hlp=ARpcDBDriverHelpers(timestampRule);
	opened=true;
	return true;
}

bool ARpcContinuousStorage::createAsChainedBlocksDb(ARpcISensorStorage::TimestampRule rule,bool gtIndex)
{
	if(opened)return false;
	if(!cbDb->create(dbDir.absolutePath()+"/data.db"))return true;
	timestampRule=rule;
	if((valueType==ARpcSensor::SINGLE_LT||valueType==ARpcSensor::PACKET_LT)&&
		timestampRule==ARpcISensorStorage::DONT_TOUCH)timestampRule=ARpcISensorStorage::ADD_GT;
	effectiveValType=defaultEffectiveValuesType(timestampRule);
	hasIndex=(gtIndex&&(effectiveValType==ARpcSensor::TEXT||effectiveValType==ARpcSensor::SINGLE_GT||
		effectiveValType==ARpcSensor::PACKET_GT));
	if(hasIndex)
	{
		if(!indDb->create(dbDir.absolutePath()+"/index.db"))return false;
	}
	QSettings settings(dbDir.absolutePath()+"/"+settingsFileRelPath(),QSettings::IniFormat);
	settings.setValue("db_type","chained_blocks");
	settings.setValue("time_rule",timestampRuleToString(timestampRule));
	settings.setValue("gt_index",hasIndex?"1":"0");
	settings.sync();
	dbType=CHAINED_BLOCKS;
	hlp=ARpcDBDriverHelpers(timestampRule);
	opened=true;
	return true;
}

void ARpcContinuousStorage::closeInternal()
{
	if(!opened)return;
	if(dbType==FIXED_BLOCKS)fbDb->close();
	else cbDb->close();
	opened=false;
}

bool ARpcContinuousStorage::isFixesBlocksDb()const
{
	return dbType==FIXED_BLOCKS;
}

bool ARpcContinuousStorage::isChainedBlocksDb()const
{
	return dbType==CHAINED_BLOCKS;
}

quint64 ARpcContinuousStorage::valuesCount()
{
	if(!opened)return 0;
	if(dbType==FIXED_BLOCKS)return fbDb->blocksCount();
	else return cbDb->blocksCount();
}

ARpcISensorValue* ARpcContinuousStorage::valueAt(quint64 index)
{
	if(!opened)return 0;
	QByteArray data;
	if(dbType==FIXED_BLOCKS&&!fbDb->readBlock(index,data))return 0;
	else if(dbType==CHAINED_BLOCKS&&!cbDb->readBlock((quint32)index,data))return 0;
	return hlp.unpackSensorValue(effectiveValType,data);
}

quint64 ARpcContinuousStorage::findInGTIndex(qint64 ts)
{
	if(!opened||!hasIndex)return 0;
	return indDb->findIndex(ts);
}

bool ARpcContinuousStorage::isOpened()const
{
	return opened;
}

bool ARpcContinuousStorage::open()
{
	if(opened)return false;
	QSettings settings(dbDir.absolutePath()+"/"+settingsFileRelPath(),QSettings::IniFormat);
	QString dbTypeStr=settings.value("db_type").toString();
	if(dbTypeStr=="fixed_blocks")
	{
		if(!fbDb->open(dbDir.absolutePath()+"/data.db"))return false;
		dbType=FIXED_BLOCKS;
	}
	else if(dbTypeStr=="chained_blocks")
	{
		if(!cbDb->open(dbDir.absolutePath()+"/data.db"))return false;
		dbType=CHAINED_BLOCKS;
	}
	else return false;
	hasIndex=(settings.value("gt_index").toString()=="1");
	if(!timestampRuleFromString(settings.value("time_rule").toString(),timestampRule))return false;
	if((valueType==ARpcSensor::SINGLE_LT||valueType==ARpcSensor::PACKET_LT)&&
		timestampRule==ARpcISensorStorage::DONT_TOUCH)timestampRule=ARpcISensorStorage::ADD_GT;
	effectiveValType=defaultEffectiveValuesType(timestampRule);
	if(hasIndex)
	{
		if(!indDb->open(dbDir.absolutePath()+"/index.db"))return false;
	}
	hlp=ARpcDBDriverHelpers(timestampRule);
	opened=true;
	return true;
}
