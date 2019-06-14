package com.alterozoom.libwliotproxy;

import java.util.HashMap;
import java.util.Map;

public class DeviceState
{
	public HashMap<ByteArray,HashMap<Integer,ByteArray>> commandParams;
	public HashMap<ByteArray,ByteArray> additionalAttributes;

	public ByteArrayList dumpToMsgArgs()
	{
		ByteArrayList retVal=new ByteArrayList();
		for(Map.Entry<ByteArray,HashMap<Integer,ByteArray>> i:commandParams.entrySet())
		{
			ByteArray command=i.getKey();
			HashMap<Integer,ByteArray> params=i.getValue();
			for(Map.Entry<Integer,ByteArray> j:params.entrySet())
			{
				retVal.add(command);
				retVal.add(ByteArray.fromString(j.getKey().toString()));
				retVal.add(j.getValue());
			}
		}
		for(Map.Entry<ByteArray,ByteArray> i:additionalAttributes.entrySet())
		{
			retVal.add(ByteArray.fromString("#"));
			retVal.add(i.getKey());
			retVal.add(i.getValue());
		}
		return retVal;
	}

	public boolean parseMsgArgs(ByteArrayList args)
	{
		if(args.size()%3!=0)return false;
		additionalAttributes.clear();
		commandParams.clear();
		for(int i=0;i<args.size()/3;++i)
		{
			ByteArray command=args.get(3*i);
			ByteArray nameOrIndex=args.get(3*i+1);
			ByteArray value=args.get(3*i+2);
			if(command.length()==0)return false;
			else if(command.equals(ByteArray.fromString("#")))
			{
				if(nameOrIndex.length()==0)return false;
				additionalAttributes.put(nameOrIndex,value);
			}
			else
			{
				Integer index=Integer.parseInt(nameOrIndex.toString());
				if(index==null||index<=0)return false;
				if(!commandParams.containsKey(command))
					commandParams.put(command,new HashMap<Integer,ByteArray>());
				commandParams.get(command).put(index,value);
			}
		}
		return true;
	}
}
