package com.alterozoom.libwliotproxy;

import java.util.ArrayList;

public class SensorValue
{
	private SensorDef.Type mType;
	private long mTime;
	private ArrayList<Object> mData;
	private int mPacketsCount;

	SensorValue(SensorDef.Type t)
	{
		mType=t;
		mTime=0;
		mPacketsCount=1;
		mData=new ArrayList<>();
	}

	public boolean isEmpty()
	{
		return mType.dim==0||mPacketsCount==0;
	}

	public boolean equals(SensorValue t)
	{
		if(mType.numType!=t.mType.numType||mType.dim!=t.mType.dim||mPacketsCount!=t.mPacketsCount)
			return false;
		int valCount=mType.dim*mPacketsCount;
		if(mType.numType==SensorDef.NumType.TEXT)
			for(int i=0;i<valCount;++i)
				if(!mData.get(i).equals(t.mData.get(i)))return false;
		return true;
	}

	public boolean parseMsgArgs(ByteArrayList args)
	{
		int argsCount=args.size();
		if(mType.numType==SensorDef.NumType.BAD_TYPE||mType.dim==0)return false;
		int dataOffset=(mType.tsType==SensorDef.TimestampType.NO_TIME)?0:1;
		if(argsCount<dataOffset)return false;
		if(mType.packType==SensorDef.ValPackType.SINGLE)
		{
			if(argsCount!=(mType.dim+dataOffset))return false;
			mPacketsCount=1;
		}
		else
		{
			if((argsCount-dataOffset)%mType.dim!=0)return false;
			mPacketsCount=(argsCount-dataOffset)/mType.dim;
		}
		if(mType.tsType!=SensorDef.TimestampType.NO_TIME)
		{
			boolean ok=false;
			Long t=Long.parseLong(args.get(0).toString());
			if(t==null)return false;
			mTime=t;
		}
		int totalValuesCount=mType.dim*mPacketsCount;
		mData.clear();
		if(mType.numType==SensorDef.NumType.TEXT)
		for(int i=0;i<totalValuesCount;++i)
			mData.add(new ByteArray(args.get(i)));
		else
			for(int i=0;i<totalValuesCount;++i)
			{
				Double d=Double.parseDouble(args.get(i).toString());
				if(d==null)
				{
					mData.clear();
					return false;
				}
				mData.add(d);
			}
		return true;
	}

	ByteArrayList dumpToMsgArgs()
	{
		ByteArrayList retVal=new ByteArrayList();
		if(mType.tsType!=SensorDef.TimestampType.NO_TIME)
			retVal.add(ByteArray.fromString(Long.toString(mTime)));
		retVal.add(dumpToMsgArgsNoTime());
		return retVal;

	}

	ByteArrayList dumpToMsgArgsNoTime()
	{
		ByteArrayList retVal=new ByteArrayList();
		int totalValuesCount=mType.dim*mPacketsCount;
		if(mType.numType==SensorDef.NumType.TEXT)
			for(int i=0;i<totalValuesCount;++i)
				retVal.add(new ByteArray((ByteArray)mData.get(i)));
		else
			for(int i=0;i<totalValuesCount;++i)
				retVal.add(ByteArray.fromString(mData.get(i).toString()));
		return retVal;
	}

	int packetsCount()
	{
		return mPacketsCount;
	}

	SensorDef.Type type()
	{
		return mType;
	}

	long time()
	{
		return mTime;
	}

	void setTime(long t)
	{
		mTime=t;
	}

	double valueToDouble(int dimIndex)
	{
		return valueToDouble(dimIndex,0);
	}

	double valueToDouble(int dimIndex,int packIndex)
	{
		if(mType.numType==SensorDef.NumType.TEXT)
			return 0;
		return (Double)mData.get(packIndex*mType.dim+dimIndex);
	}

	ByteArray valueToString(int dimIndex)
	{
		return valueToString(dimIndex,0);
	}

	ByteArray valueToString(int dimIndex,int packIndex)
	{
		if(mType.numType==SensorDef.NumType.TEXT)
			return new ByteArray((ByteArray)mData.get(packIndex*mType.dim+dimIndex));
		return ByteArray.fromString(mData.get(packIndex*mType.dim+dimIndex).toString());
	}
}
