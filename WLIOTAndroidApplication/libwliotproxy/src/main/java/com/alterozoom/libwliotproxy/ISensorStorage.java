package com.alterozoom.libwliotproxy;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.UUID;

public abstract class ISensorStorage
{
	public class DataExportConfig
		extends HashMap<ByteArray,ByteArray>
	{
	}

	public enum StoreMode
	{
		BAD_MODE,
		CONTINUOUS,
		MANUAL_SESSIONS,
		AUTO_SESSIONS,
		LAST_N_VALUES,
		LAST_N_VALUES_IN_MEMORY
	}

	public enum TimestampRule
	{
		BAD_RULE,
		DONT_TOUCH,
		ADD_GT,
		//also replace TL with GT
		DROP_TIME
	}

	protected UUID mDeviceId;
	protected ByteArray mDeviceName;
	protected SensorDef mSensor;
	protected ISensorStorage.TimestampRule mTimestampRule;
	protected SensorDef.Type mStoredValuesType;
	protected StoreMode mStoreMode;

	ISensorStorage(final UUID devId,final ByteArray devName,final SensorDef sensor,final StoreMode stMode,
		final ISensorStorage.TimestampRule tsRule,final SensorDef.Type storedValType)
	{
		mDeviceId=UUID.fromString(devId.toString());
		mDeviceName=new ByteArray(devName);
		mSensor=new SensorDef(sensor);
		mTimestampRule=tsRule;
		mStoredValuesType=new SensorDef.Type(storedValType);
		mStoreMode=stMode;
	}

	public abstract boolean open();
	public abstract void close();
	public abstract boolean isOpened();
	public abstract long valuesCount();
	public abstract SensorValue valueAt(long index);
	public abstract boolean writeSensorValue(SensorValue val);
	public abstract void writeAttribute(ByteArray str,ByteArray var);
	public abstract ByteArray readAttribute(ByteArray str);
	public abstract void addDataExportConfig(UUID serviceId,DataExportConfig cfg);
	public abstract boolean hasDataExportConfig(UUID serviceId);
	public abstract DataExportConfig getDataExportConfig(UUID serviceId);
	public abstract void removeDataExportConfig(UUID serviceId);
	public abstract ArrayList<UUID> allDataExportServices();

	public SensorDef sensor()
	{
		return mSensor;
	}

	public UUID deviceId()
	{
		return mDeviceId;
	}

	public ByteArray deviceName()
	{
		return mDeviceName;
	}


	static ByteArray storeModeToString(StoreMode mode)
	{
		if(mode==StoreMode.CONTINUOUS)
			return ByteArray.fromString("continuous");
		else if(mode==StoreMode.MANUAL_SESSIONS)
			return ByteArray.fromString("manual_sessions");
		else if(mode==StoreMode.AUTO_SESSIONS)
			return ByteArray.fromString("auto_sessions");
		else if(mode==StoreMode.LAST_N_VALUES)
			return ByteArray.fromString("last_n_values");
		else if(mode==StoreMode.LAST_N_VALUES_IN_MEMORY)
			return ByteArray.fromString("memory");
		else
			return new ByteArray();
	}

	static StoreMode storeModeFromString(ByteArray str)
	{
		if(str.equals(ByteArray.fromString("continuous")))
			return StoreMode.CONTINUOUS;
		else if(str.equals(ByteArray.fromString("manual_sessions")))
			return StoreMode.MANUAL_SESSIONS;
		else if(str.equals(ByteArray.fromString("auto_sessions")))
			return StoreMode.AUTO_SESSIONS;
		else if(str.equals(ByteArray.fromString("last_n_values")))
			return StoreMode.LAST_N_VALUES;
		else if(str.equals(ByteArray.fromString("memory")))
			return StoreMode.LAST_N_VALUES_IN_MEMORY;
		else
			return StoreMode.BAD_MODE;
	}

	static ByteArray timestampRuleToString(TimestampRule rule)
	{
		if(rule==TimestampRule.ADD_GT)
			return ByteArray.fromString("add_global_time");
		else if(rule==TimestampRule.DROP_TIME)
			return ByteArray.fromString("drop_time");
		else /*DONT_TOUCH*/
			return ByteArray.fromString("dont_touch");
	}

	static TimestampRule timestampRuleFromString(ByteArray str)
	{
		if(str.equals(ByteArray.fromString("add_global_time")))
			return TimestampRule.ADD_GT;
		else if(str.equals(ByteArray.fromString("drop_time")))
			return TimestampRule.DROP_TIME;
		else if(str.equals(ByteArray.fromString("dont_touch")))
			return TimestampRule.DONT_TOUCH;
		else
			return TimestampRule.BAD_RULE;
	}

	TimestampRule timestampRule()
	{
		return mTimestampRule;
	}

	SensorDef.Type storedValuesType()
	{
		return mStoredValuesType;
	}

	StoreMode storeMode()
	{
		return mStoreMode;
	}

	boolean values(long index,long count,long step,ArrayList<SensorValue> vals)
	{
		if(!isOpened())
			return false;
		vals.clear();
		for(long i=0;i<count;++i)
		{
			SensorValue v=valueAt(index+i*step);
			if(v!=null)
				vals.add(v);
			else break;
		}
		return true;
	}
}
