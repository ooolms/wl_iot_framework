#include "ARpcContinuousStorage.h"
#include "ARpcBase/ARpcPacketSensorValue.h"
#include "ARpcBase/ARpcSingleSensorValue.h"
#include "ARpcBase/ARpcTextSensorValue.h"
#include "ARpcDBDriverHelpers.h"
#include <QDir>
#include <QDateTime>
#include <QSettings>

ARpcContinuousStorage::ARpcContinuousStorage(QObject *parent)
	:ARpcISensorStorage(parent)
{
	fbDb=new ARpcDBDriverFixedBlocks(this);
	cbDb=new ARpcDBDriverChainedBlocks(this);
	opened=false;
	dbType=CHAINED_BLOCKS;
}

ARpcISensorStorage::StoreMode ARpcContinuousStorage::getStoreMode()const
{
	return CONTINUOUS;
}

bool ARpcContinuousStorage::writeSensorValue(const ARpcISensorValue *val)
{
	if(!opened)return false;
	if(val->type()!=valueType)return false;
	if(dbType==FIXED_BLOCKS)return writeSensorValueFB(val);
	else return writeSensorValueCB(val);
}

bool ARpcContinuousStorage::createAsFixedBlocksDb(const QVector<quint32> &blockNotesSizes)
{
	if(opened)return false;
	if(!fbDb->create(dbDir.absolutePath()+"/data.db",blockNotesSizes))return false;
	QSettings settings(dbDir.absolutePath()+"/"+settingsFileRelPath(),QSettings::IniFormat);
	settings.setValue("db_type","fixed_blocks");
	settings.sync();
	dbType=FIXED_BLOCKS;
	opened=true;
	return true;
}

bool ARpcContinuousStorage::createAsChainedBlocksDb()
{
	if(opened)return false;
	if(!cbDb->create(dbDir.absolutePath()+"/data.db"))return true;
	QSettings settings(dbDir.absolutePath()+"/"+settingsFileRelPath(),QSettings::IniFormat);
	settings.setValue("db_type","chained_blocks");
	settings.sync();
	dbType=CHAINED_BLOCKS;
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

bool ARpcContinuousStorage::openInternal()
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
		if(!fbDb->open(dbDir.absolutePath()+"/data.db"))return false;
		dbType=CHAINED_BLOCKS;
	}
	else return false;
	opened=true;
	return true;
}

bool ARpcContinuousStorage::writeSensorValueFB(const ARpcISensorValue *val)
{
	return ARpcDBDriverHelpers::writeSensorValueToFixedBlocksDB(fbDb,val,true);
}

bool ARpcContinuousStorage::writeSensorValueCB(const ARpcISensorValue *val)
{
	return ARpcDBDriverHelpers::writeSensorValueToChainedBlocksDB(cbDb,val,true);
}
