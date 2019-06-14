package com.alterozoom.libwliotproxy;

import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.Map;
import java.util.UUID;

public class IotServerStoragesCommands
{
	private IotServerConnection srvConn;

	IotServerStoragesCommands(IotServerConnection conn)
	{
		srvConn=conn;
	}

	public boolean listStorages(ArrayList<IotServerStorageDescr> stors)
	{
		class Cb
			implements CmDataCallback
		{
			private ArrayList<IotServerStorageDescr> stors;

			Cb(ArrayList<IotServerStorageDescr> s)
			{
				stors=s;
			}

			@Override
			public boolean onCmData(ByteArrayList args)
			{
				IotServerStorageDescr st=new IotServerStorageDescr();
				if(!storageFromArgs(args,st))return false;
				stors.add(st);
				return true;
			}
		}
		stors.clear();
		CmDataCallback cb=new Cb(stors);
		return srvConn.execCommand(ByteArray.fromString("list_storages"),null,cb);
	}

	public boolean addStorage(ByteArray devIdOrName,ByteArray sensorName,ISensorStorage.StoreMode storeMode,
		ISensorStorage.TimestampRule tsRule,int valuesCount)
	{
		if(storeMode==ISensorStorage.StoreMode.LAST_N_VALUES||
			storeMode==ISensorStorage.StoreMode.LAST_N_VALUES_IN_MEMORY)
			return srvConn.execCommand(ByteArray.fromString("add_storage"),new ByteArrayListBuilder().add(devIdOrName).
				add(sensorName).add(ISensorStorage.storeModeToString(storeMode)).
				add(ISensorStorage.timestampRuleToString(tsRule)).
				add(ByteArray.fromString(Integer.toString(valuesCount))).done());
		else return srvConn.execCommand(ByteArray.fromString("add_storage"),new ByteArrayListBuilder().add(devIdOrName).
			add(sensorName).add(ISensorStorage.storeModeToString(storeMode)).
			add(ISensorStorage.timestampRuleToString(tsRule)).done());
	}

	public boolean removeStorage(ByteArray devIdOrName,ByteArray sensorName)
	{
		return srvConn.execCommand(ByteArray.fromString("remove_storage"),
			new ByteArrayListBuilder().add(devIdOrName).add(sensorName).done());
	}

	public boolean storageAddDataExport(
		ByteArray devIdOrName,ByteArray sensorName,UUID serviceId,
		HashMap<ByteArray,ByteArray> serviceConfig)
	{
		ByteArrayList serviceConfigArgs=new ByteArrayList();
		try
		{
			for(Map.Entry<ByteArray,ByteArray> i: serviceConfig.entrySet())
			{
				ByteArrayOutputStream b=new ByteArrayOutputStream();
				b.write(i.getKey().dataRef());
				b.write(ByteArray.toBA(":"));
				b.write(i.getValue().dataRef());
				serviceConfigArgs.add(new ByteArray(b.toByteArray()));
			}
		}
		catch(IOException e)
		{
			e.printStackTrace();
			return false;
		}
		return srvConn.execCommand(ByteArray.fromString("storage_add_data_export"),
			new ByteArrayListBuilder().add(devIdOrName).add(sensorName).
				add(ByteArray.fromString(serviceId.toString())).add(serviceConfigArgs).done());
	}

	public boolean storageGetDataExport(
		ByteArray devIdOrName,ByteArray sensorName,UUID serviceId,HashMap<ByteArray,ByteArray> serviceConfig)
	{
		ObjectRef<ByteArrayList> retVal=new ObjectRef<>();
		if(!srvConn.execCommand(ByteArray.fromString("storage_get_data_export"),
			new ByteArrayListBuilder().add(devIdOrName).add(sensorName).
				add(ByteArray.fromString(serviceId.toString())).done()))
			return false;
		serviceConfig.clear();
		for(int i=0;i<retVal.o().size();++i)
		{
			String s=retVal.o().get(i).toString();
			int index=s.indexOf(":");
			if(index<=0)
				return false;
			String k=s.substring(0,index-1);
			String v=s.substring(index+1,s.length());
			serviceConfig.put(ByteArray.fromString(k),ByteArray.fromString(v));
		}
		return true;
	}

	public boolean storageAllDataExports(
		ByteArray devIdOrName,ByteArray sensorName,ArrayList<UUID> services)
	{
		class Cb
			implements CmDataCallback
		{
			private ArrayList<UUID> services;

			Cb(ArrayList<UUID> s)
			{
				services=s;
			}

			@Override
			public boolean onCmData(ByteArrayList args)
			{
				if(args.size()==0)return false;
				UUID uid=UUID.fromString(args.get(0).toString());
				if(uid==null)return false;
				services.add(uid);
				return true;
			}
		}
		Cb cb=new Cb(services);
		services.clear();
		return srvConn.execCommand(ByteArray.fromString("storage_get_data_export_list"),
			new ByteArrayListBuilder().add(devIdOrName).add(sensorName).done(),cb);
	}

	public boolean listSessions(ByteArray devIdOrName,ByteArray sensorName,
		ArrayList<UUID> ids,ByteArrayList titles)
	{
		class Cb
			implements CmDataCallback
		{
			private ArrayList<UUID> ids;
			private ByteArrayList titles;

			Cb(ArrayList<UUID> i,ByteArrayList t)
			{
				ids=i;
				titles=t;
			}

			@Override
			public boolean onCmData(ByteArrayList args)
			{
				if(args.size()<2)
					return false;
				UUID sId=UUID.fromString(args.get(0).toString());
				if(sId==null)
					return false;
				ids.add(sId);
				titles.add(args.get(1));
				return true;
			}
		}
		Cb cb=new Cb(ids,titles);
		ids.clear();
		titles.clear();
		ObjectRef<ByteArrayList> retVal=new ObjectRef<>();
		return srvConn.execCommand(ByteArray.fromString("session_list"),
			new ByteArrayListBuilder().add(devIdOrName).add(sensorName).done(),retVal,cb);
	}

	public boolean listSessionAttrs(
		ByteArray devIdOrName,ByteArray sensorName,UUID sessionId,HashMap<ByteArray,ByteArray> attrs)
	{
		class Cb
			implements CmDataCallback
		{
			private HashMap<ByteArray,ByteArray> attrs;

			Cb(HashMap<ByteArray,ByteArray> at)
			{
				attrs=at;
			}

			@Override
			public boolean onCmData(ByteArrayList args)
			{
				if(args.size()<2||args.get(0).length()==0)
					return false;
				attrs.put(args.get(0),args.get(1));
				return true;
			}
		}
		Cb cb=new Cb(attrs);
		attrs.clear();
		ObjectRef<ByteArrayList> retVal=new ObjectRef<>();
		return srvConn.execCommand(ByteArray.fromString("session_list_attrs"),
			new ByteArrayListBuilder().add(devIdOrName).add(sensorName).
				add(ByteArray.fromString(sessionId.toString())).done(),retVal,cb);
	}

	public ByteArray getSessionAttr(ByteArray devIdOrName,ByteArray sensorName,
		UUID sessionId,ByteArray attrName)
	{
		ObjectRef<ByteArrayList> retVal=new ObjectRef<>();
		if(!srvConn.execCommand(ByteArray.fromString("session_get_attr"),
			new ByteArrayListBuilder().add(devIdOrName).add(sensorName).add(ByteArray.fromString(sessionId.toString())).
				add(attrName).done(),retVal)||retVal.o().size()==0)
			return null;
		return retVal.o().get(0);
	}

	public boolean setSessionAttr(ByteArray devIdOrName,ByteArray sensorName,
		UUID sessionId,ByteArray attrName,ByteArray value)
	{
		return srvConn.execCommand(ByteArray.fromString("session_set_attr"),
			new ByteArrayListBuilder().add(devIdOrName).add(sensorName).add(ByteArray.fromString(sessionId.toString())).
				add(attrName).done());
	}

	public boolean removeSessionAttr(ByteArray devIdOrName,ByteArray sensorName,
		UUID sessionId,ByteArray attrName)
	{
		return srvConn.execCommand(ByteArray.fromString("session_set_attr"),
			new ByteArrayListBuilder().add(devIdOrName).add(sensorName).
				add(ByteArray.fromString(sessionId.toString())).add(attrName).done());
	}

	public boolean startSession(
		ByteArray devIdOrName,ByteArray sensorName,UUID sessionId)
	{
		return srvConn.execCommand(ByteArray.fromString("session_start"),
			new ByteArrayListBuilder().add(devIdOrName).add(sensorName).
				add(ByteArray.fromString(sessionId.toString())).done());
	}

	public UUID createSession(
		ByteArray devIdOrName,ByteArray sensorName,ByteArray sessionTitle)
	{
		ObjectRef<ByteArrayList> retVal=new ObjectRef<>();
		if(!srvConn.execCommand(ByteArray.fromString("session_create"),
			new ByteArrayListBuilder().add(devIdOrName).add(sensorName).add(sessionTitle).done(),retVal)||
			retVal.o().size()==0)
			return null;
		return UUID.fromString(retVal.o().get(0).toString());
	}

	public boolean stopSession(ByteArray devIdOrName,ByteArray sensorName)
	{
		return srvConn.execCommand(ByteArray.fromString("session_stop"),
			new ByteArrayListBuilder().add(devIdOrName).add(sensorName).done());
	}

	public boolean removeSession(ByteArray devIdOrName,ByteArray sensorName,UUID sessionId)
	{
		return srvConn.execCommand(ByteArray.fromString("session_remove"),
			new ByteArrayListBuilder().add(devIdOrName).add(sensorName).
				add(ByteArray.fromString(sessionId.toString())).done());
	}

	public boolean continueSession(ByteArray devIdOrName,ByteArray sensorName,UUID sessionId)
	{
		return srvConn.execCommand(ByteArray.fromString("session_continue"),
			new ByteArrayListBuilder().add(devIdOrName).add(sensorName).
				add(ByteArray.fromString(sessionId.toString())).done());
	}

	public UUID getSessionWriteId(ByteArray devIdOrName,ByteArray sensorName)
	{
		ObjectRef<ByteArrayList> retVal=new ObjectRef<>();
		if(!srvConn.execCommand(ByteArray.fromString("session_get_write_id"),
			new ByteArrayListBuilder().add(devIdOrName).add(sensorName).done(),retVal)||
			retVal.o().size()==0)
			return null;
		return UUID.fromString(retVal.o().get(0).toString());
	}

	public Long getSamplesCount(ByteArray devIdOrName,ByteArray sensorName,UUID sessionId)
	{
		ObjectRef<ByteArrayList> retVal=new ObjectRef<>();
		ByteArrayListBuilder args=new ByteArrayListBuilder();
		args.add(devIdOrName).add(sensorName);
		if(sessionId!=null)
			args.add(ByteArray.fromString(sessionId.toString()));
		if(!srvConn.execCommand(ByteArray.fromString("get_samples_count"),args.done(),retVal)||retVal.o().size()==0)
			return null;
		return Long.parseLong(retVal.o().get(0).toString());
	}

	public boolean getSamples(ByteArray devIdOrName,ByteArray sensorName,
		long startIndex,long count,long step,SensorDef.Type sensorType,
		ArrayList<SensorValue> values,UUID sessionId)
	{
		class Cb
			implements CmDataCallback
		{
			private ArrayList<SensorValue> values;
			private SensorDef.Type sensorType;

			Cb(ArrayList<SensorValue> a,SensorDef.Type t)
			{
				values=a;
				sensorType=t;
			}

			@Override
			public boolean onCmData(ByteArrayList args)
			{
				SensorValue val=new SensorValue(sensorType);
				if(!val.parseMsgArgs(args))return false;
				values.add(val);
				return true;
			}
		}
		if(step<=0)
			step=1;
		values.clear();
		Cb cb=new Cb(values,sensorType);
		ByteArrayListBuilder args=new ByteArrayListBuilder().add(devIdOrName).add(sensorName);
		if(sessionId!=null)
			args.add(ByteArray.fromString(sessionId.toString()));
		args.add(ByteArray.fromString(Long.toString(startIndex)));
		args.add(ByteArray.fromString(Long.toString(count)));
		args.add(ByteArray.fromString(Long.toString(step)));
		return srvConn.execCommand(ByteArray.fromString("get_samples"),args.done(),cb);
	}

	public boolean setStorageAttr(
		ByteArray devIdOrName,ByteArray sensorName,ByteArray attrName,ByteArray attrValue)
	{
		return srvConn.execCommand(ByteArray.fromString("storage_set_attr"),
			new ByteArrayListBuilder().add(devIdOrName).add(sensorName).add(attrName).add(attrValue).done());
	}

	ByteArray getStorageAttr(ByteArray devIdOrName,ByteArray sensorName,ByteArray attrName)
	{
		ObjectRef<ByteArrayList> retVal=new ObjectRef<>();
		if(!srvConn.execCommand(ByteArray.fromString("storage_get_attr"),
			new ByteArrayListBuilder().add(devIdOrName).add(sensorName).add(attrName).done(),retVal)||
			retVal.o().size()==0)
			return null;
		return retVal.o().get(0);
	}

	public boolean availableDataExportServices(ArrayList<IotServerDataExportServiceDescr> services)
	{
		class Cb
			implements CmDataCallback
		{
			private ArrayList<IotServerDataExportServiceDescr> services;

			Cb(ArrayList<IotServerDataExportServiceDescr> s)
			{
				services=s;
			}

			@Override
			public boolean onCmData(ByteArrayList args)
			{
				if(args.size()<3)return false;
				UUID uid=UUID.fromString(args.get(0).toString());
				if(uid==null||args.get(1).length()==0)
					return false;
				IotServerDataExportServiceDescr d=new IotServerDataExportServiceDescr();
				d.uid=uid;
				d.name=args.get(1);
				d.paramNames=new ByteArrayList(args);
				d.paramNames.remove(0);
				d.paramNames.remove(0);
				if(d.paramNames.size()==0)return false;
				services.add(d);
				return true;
			}
		}
		Cb cb=new Cb(services);
		services.clear();
		return srvConn.execCommand(ByteArray.fromString("available_data_export_services"),null,cb);
	}

	public boolean alterozoomAuth(ByteArray host,ByteArray email,ByteArray pass)
	{
		return srvConn.execCommand(ByteArray.fromString("alterozoom_auth"),
			new ByteArrayListBuilder().add(host).add(email).add(pass).done());
	}

	public boolean alterozoomList(ArrayList<Pair<ByteArray,ByteArray>> hostEmailList)
	{
		class Cb
			implements CmDataCallback
		{
			ArrayList<Pair<ByteArray,ByteArray>> hostEmailList;

			Cb(ArrayList<Pair<ByteArray,ByteArray>> l)
			{
				hostEmailList=l;
			}

			@Override
			public boolean onCmData(ByteArrayList args)
			{
				if(args.size()<2)return false;
				if(args.get(0).length()!=0&&args.get(1).length()!=0)
					hostEmailList.add(new Pair<>(args.get(0),args.get(1)));
				return true;
			}
		}
		Cb cb=new Cb(hostEmailList);
		hostEmailList.clear();
		return srvConn.execCommand(ByteArray.fromString("alterozoom_list"),null,cb);
	}

	public static boolean storageFromArgs(ByteArrayList args,IotServerStorageDescr st)
	{
		if(args.size()<8)return false;
		ByteArrayList attributesStrs=args.get(4).split((byte)';');
		HashMap<ByteArray,ByteArray> attributes=new HashMap<>();
		for(int i=0;i<attributesStrs.size();++i)
		{
			String s=attributesStrs.get(i).toString();
			int index=s.indexOf('=');
			if(index==-1)continue;
			String k=s.substring(0,index-1);
			String v=s.substring(index,s.length());
			attributes.put(ByteArray.fromString(k),ByteArray.fromString(v));
		}
		st.deviceId=UUID.fromString(args.get(0).toString());
		st.deviceName=args.get(1);
		st.sensor.name=args.get(2);
		st.sensor.type.fromBA(args.get(3));
		st.sensor.attributes=attributes;
		st.mode=ISensorStorage.storeModeFromString(args.get(5));
		st.tsRule=ISensorStorage.timestampRuleFromString(args.get(6));
		st.storedValuesType.fromBA(args.get(7));
		if(st.deviceId==null||st.mode==ISensorStorage.StoreMode.BAD_MODE||
			st.tsRule==ISensorStorage.TimestampRule.BAD_RULE||!st.sensor.type.isValid()||st.sensor.name.length()==0||
			!st.storedValuesType.isValid())
			return false;
		return true;
	}

}
