package com.alterozoom.libwliotproxy;

import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

public class SensorDef
{
	public enum NumType
	{
		BAD_TYPE,
		F32,//float
		F64,//double
		S8,//signed char
		U8,//unsigned char
		S16,//signed short
		U16,//unsigned short
		S32,//signed long
		U32,//unsigned long
		S64,//signed long long
		U64,//unsigned long long
		TEXT//char* UTF-8
	};

	public enum ValPackType
	{
		SINGLE,
		PACKET
	};

	public enum TimestampType
	{
		NO_TIME,
		LOCAL_TIME,
		GLOBAL_TIME
	}

	public static class Type
	{
		public NumType numType;
		public ValPackType packType;
		public TimestampType tsType;
		public int dim;

		Type()
		{
			numType=NumType.BAD_TYPE;
			packType=ValPackType.SINGLE;
			tsType=TimestampType.NO_TIME;
			dim=1;
		}

		Type(NumType nt,ValPackType pt,TimestampType tt,int d)
		{
			numType=nt;
			packType=pt;
			tsType=tt;
			dim=d;
		}

		Type(Type t)
		{
			numType=t.numType;
			packType=t.packType;
			tsType=t.tsType;
			dim=t.dim;
		}

		public boolean isValid()
		{
			return numType!=NumType.BAD_TYPE&&dim>0;
		}

		public ByteArray toBA()
		{
			if(!isValid())return new ByteArray();
			ByteArrayOutputStream s=new ByteArrayOutputStream();
			//numType
			try
			{
				if(numType==NumType.F32)
					s.write(ByteArray.toBA("f32"));
				else if(numType==NumType.F64)
					s.write(ByteArray.toBA("f64"));
				else if(numType==NumType.S8)
					s.write(ByteArray.toBA("s8"));
				else if(numType==NumType.U8)
					s.write(ByteArray.toBA("u8"));
				else if(numType==NumType.S16)
					s.write(ByteArray.toBA("s16"));
				else if(numType==NumType.U16)
					s.write(ByteArray.toBA("u16"));
				else if(numType==NumType.S32)
					s.write(ByteArray.toBA("s32"));
				else if(numType==NumType.U32)
					s.write(ByteArray.toBA("u32"));
				else if(numType==NumType.S64)
					s.write(ByteArray.toBA("s64"));
				else if(numType==NumType.U64)
					s.write(ByteArray.toBA("u64"));
				else //TEXT
					s.write(ByteArray.toBA("txt"));
				//dim
				if(dim!=1)
				{
					s.write(ByteArray.toBA("_d"));
					s.write(ByteArray.toBA(Integer.toString(dim)));
				}
				//packType
				if(packType==ValPackType.SINGLE)
					s.write(ByteArray.toBA("_sv"));
				else s.write(ByteArray.toBA("_pv"));
				//timestamp
				if(tsType==TimestampType.LOCAL_TIME)
					s.write(ByteArray.toBA("_lt"));
				else if(tsType==TimestampType.GLOBAL_TIME)
					s.write(ByteArray.toBA("_gt"));
				else s.write(ByteArray.toBA("_nt"));
			}
			catch(IOException e)
			{
				e.printStackTrace();
			}
			return new ByteArray(s.toByteArray());

		}

		public boolean fromBA(ByteArray str)
		{
			NumType newNumType=NumType.BAD_TYPE;
			ValPackType newPackType=ValPackType.SINGLE;
			Integer newDim=new Integer(1);
			TimestampType newTsType=TimestampType.NO_TIME;
			ByteArrayList list=str.split((byte)'_');
			Pattern dimRegExp=Pattern.compile("^d(\\d+)$");
			for(int i=0;i<list.size();++i)
			{
				ByteArray el=list.get(i);
				//numType
				if(el.equals("f32"))
					newNumType=NumType.F32;
				else if(el.equals("f64"))
					newNumType=NumType.F64;
				else if(el.equals("s8"))
					newNumType=NumType.S8;
				else if(el.equals("u8"))
					newNumType=NumType.U8;
				else if(el.equals("s16"))
					newNumType=NumType.S16;
				else if(el.equals("u16"))
					newNumType=NumType.U16;
				else if(el.equals("s32"))
					newNumType=NumType.S32;
				else if(el.equals("u32"))
					newNumType=NumType.U32;
				else if(el.equals("s64"))
					newNumType=NumType.S64;
				else if(el.equals("u64"))
					newNumType=NumType.U64;
				else if(el.equals("txt"))
					newNumType=NumType.TEXT;
					//packType
				else if(el.equals("sv"))
					newPackType=ValPackType.SINGLE;
				else if(el.equals("pv"))
					newPackType=ValPackType.PACKET;
					//tsType
				else if(el.equals("nt"))
					newTsType=TimestampType.NO_TIME;
				else if(el.equals("lt"))
					newTsType=TimestampType.LOCAL_TIME;
				else if(el.equals("gt"))
					newTsType=TimestampType.GLOBAL_TIME;
					//dim
				else
				{
					Matcher m=dimRegExp.matcher(el.toString());
					if(!m.matches())
						return false;
					newDim=Integer.parseInt(m.group(1));
					if(newDim==null)
						return false;
				}
			}
			if(newNumType==NumType.BAD_TYPE)return false;
			numType=newNumType;
			dim=newDim;
			packType=newPackType;
			tsType=newTsType;
			return true;
		}
	}

	public ByteArray name;
	public ByteArray title;
	public ByteArray unit;
	public Type type;
	public HashMap<ByteArray,ByteArray> attributes;

	SensorDef()
	{
		name=new ByteArray();
		title=new ByteArray();
		unit=new ByteArray();
		type=new Type();
		attributes=new HashMap<>();
	}

	SensorDef(Type t,ByteArray n)
	{
		name=new ByteArray(n);
		type=new Type(t);
		title=new ByteArray();
		unit=new ByteArray();
		attributes=new HashMap<>();
	}

	SensorDef(SensorDef t)
	{
		name=new ByteArray(t.name);
		type=new Type(t.type);
		title=new ByteArray(t.title);
		unit=new ByteArray(t.unit);
		attributes=t.attributes.getClass().cast(t.attributes.clone());
	}

	public ByteArray nameOrTitle()
	{
		if(title.length()==0)
			return name;
		return title;

	}

	public static int findByName(ArrayList<SensorDef> sensors,ByteArray name)
	{
		for(int i=0;i<sensors.size();++i)
			if(sensors.get(i).name.equals(name))
				return i;
		return -1;
	}


	public static boolean parseJsonDescription(ByteArray data,ArrayList<SensorDef> sensors)
	{
		//TODO
		return false;
	}

	public static boolean parseXmlDescription(ByteArray data,ArrayList<SensorDef> sensors)
	{
		//TODO
		return false;
	}

	public static ByteArray dumpToJson(ArrayList<SensorDef> sensors)
	{
		//TODO
		return null;
	}

	public static ByteArray dumpToXml(ArrayList<SensorDef> sensors)
	{
		//TODO
		return null;
	}
}
