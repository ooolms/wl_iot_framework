package com.alterozoom.libwliotproxy;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.UUID;

public class IotServerDevicesCommands
{
	private IotServerConnection srvConn;

	IotServerDevicesCommands(IotServerConnection conn)
	{
		srvConn=conn;
	}


	public boolean listTty(ArrayList<IotServerTtyPortDescr> ttyList)
	{
		class Cb
			implements CmDataCallback
		{
			private ArrayList<IotServerTtyPortDescr> ttyList;

			Cb(ArrayList<IotServerTtyPortDescr> list)
			{
				ttyList=list;
			}

			@Override
			public boolean onCmData(ByteArrayList args)
			{
				if(args.size()<5)
					return false;
				IotServerTtyPortDescr d=new IotServerTtyPortDescr();
				d.portName=args.get(0);
				d.serialNumber=args.get(1);
				d.manufacturer=args.get(2);
				d.usbVendorId=args.get(3);
				d.usbProductId=args.get(4);
				ttyList.add(d);
				return true;
			}
		};
		Cb cb=new Cb(ttyList);
		return srvConn.execCommand(ByteArray.fromString("list_tty"),null,cb);
	}

	public boolean identifyTty(ByteArray portName)
	{
		return srvConn.execCommand(ByteArray.fromString("identify_tty"),
			new ByteArrayListBuilder().add(portName).done());
	}

	public boolean identifyTcp(ByteArray host)
	{
		return srvConn.execCommand(ByteArray.fromString("identify_tcp"),new ByteArrayListBuilder().add(host).done());
	}

	public boolean listIdentified(ArrayList<IotServerIdentifiedDeviceDescr> devs)
	{
		class Cb
			implements CmDataCallback
		{
			private ArrayList<IotServerIdentifiedDeviceDescr> devs;

			Cb(ArrayList<IotServerIdentifiedDeviceDescr> d)
			{
				devs=d;
			}

			@Override
			public boolean onCmData(ByteArrayList args)
			{
				if(args.size()<4)return false;
				IotServerIdentifiedDeviceDescr d=new IotServerIdentifiedDeviceDescr();
				d.id=UUID.fromString(args.get(0).toString());
				d.name=args.get(1);
				d.typeId=UUID.fromString(args.get(2).toString());
				d.portOrName=args.get(3);
				if(d.id==null)return false;
				devs.add(d);
				return true;
			}
		}
		Cb cb=new Cb(devs);
		return srvConn.execCommand(ByteArray.fromString("list_identified"),null,cb);
	}

	public boolean listSensors(ByteArray idOrName,ArrayList<SensorDef> sensors)
	{
		ObjectRef<ByteArrayList> retVal=new ObjectRef<>();
		if(!srvConn.execCommand(ByteArray.fromString("list_sensors"),new ByteArrayListBuilder().add(idOrName).done(),
			retVal)||retVal.o().size()==0)return false;
		if(retVal.o().get(0).toString().startsWith("{"))
			return SensorDef.parseJsonDescription(retVal.o().get(0),sensors);
		else return SensorDef.parseXmlDescription(retVal.o().get(0),sensors);
	}

	public boolean listControls(ByteArray idOrName,ControlsDefinition.ControlsGroup controls)
	{
		ObjectRef<ByteArrayList> retVal=new ObjectRef<>();
		if(!srvConn.execCommand(ByteArray.fromString("list_controls"),new ByteArrayListBuilder().add(idOrName).done(),
			retVal)||retVal.o().size()==0)return false;
		if(retVal.o().get(0).toString().startsWith("{"))
			return ControlsDefinition.ControlsGroup.parseJsonDescription(retVal.o().get(0),controls);
		else return ControlsDefinition.ControlsGroup.parseXmlDescription(retVal.o().get(0),controls);
	}

	public boolean getDevState(ByteArray idOrName,DeviceState state)
	{
		ObjectRef<ByteArrayList> retVal=new ObjectRef<>();
		if(!srvConn.execCommand(ByteArray.fromString("get_dev_state"),
			new ByteArrayListBuilder().add(idOrName).done(),retVal))return false;
		return state.parseMsgArgs(retVal.o());
	}

	public UUID deviceId(ByteArray idOrName)
	{
		ObjectRef<ByteArrayList> retVal=new ObjectRef<>();
		if(!srvConn.execCommand(ByteArray.fromString("device_id"),
			new ByteArrayListBuilder().add(idOrName).done(),retVal)||retVal.o().size()==0)return null;
		return UUID.fromString(retVal.o().get(0).toString());
	}

	public boolean execDeviceCommand(
		ByteArray idOrName,ByteArray command,ByteArrayList args,ObjectRef<ByteArrayList> retVal)
	{
		if(command.equals(WLIOTProtocolDefs.getSensorsCommand))
			return srvConn.execCommand(ByteArray.fromString("list_sensors"),
				new ByteArrayListBuilder().add(idOrName).done(),retVal);
		else if(command.equals(WLIOTProtocolDefs.getControlsCommand))
			return srvConn.execCommand(ByteArray.fromString("list_controls"),
				new ByteArrayListBuilder().add(idOrName).done(),retVal);
		else if(command.equals(WLIOTProtocolDefs.getStateCommand))
			return srvConn.execCommand(ByteArray.fromString("get_dev_state"),
				new ByteArrayListBuilder().add(idOrName).done(),retVal);
		else return srvConn.execCommand(ByteArray.fromString("exec_command"),
				new ByteArrayListBuilder().add(idOrName).add(command).add(args).done(),retVal);
	}

	public boolean registerVirtualDevice(
		UUID deviceId,ByteArray deviceName,UUID typeId)
	{
		return srvConn.execCommand(ByteArray.fromString("register_virtual_device"),
			new ByteArrayListBuilder().add(ByteArray.fromString(deviceId.toString())).add(deviceName).
				add(ByteArray.fromString(typeId.toString())).done());
	}

	public boolean sendVDevMeasurement(
		UUID deviceId,ByteArray sensorName,ByteArrayList measArgs)
	{
		return srvConn.execCommand(ByteArray.fromString("vdev_meas"),
			new ByteArrayListBuilder().add(ByteArray.fromString(deviceId.toString())).
				add(sensorName).add(measArgs).done());
	}

	public boolean devNames(ArrayList<UUID> ids,ObjectRef<ByteArrayList> names)
	{
		ByteArrayList idsList=new ByteArrayList();
		for(UUID id:ids)
			idsList.add(ByteArray.fromString(id.toString()));
		return srvConn.execCommand(ByteArray.fromString("dev_names"),idsList,names);
	}

	public boolean setDevName(UUID devId,ByteArray devName)
	{
		if(devName.length()==0)
			return srvConn.execCommand(ByteArray.fromString("set_dev_name"),
				new ByteArrayListBuilder().add(ByteArray.fromString(devId.toString())).done());
		else return srvConn.execCommand(ByteArray.fromString("set_dev_name"),
			new ByteArrayListBuilder().add(ByteArray.fromString(devId.toString())).add(devName).done());
	}
}
