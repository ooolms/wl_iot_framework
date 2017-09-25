var deviceId="{f84526c1-5e88-4315-81f8-f7da45daa09d}";
var sensorName="blinks_count";

var outDeviceId="{129ed0af-42ba-4f4e-a71c-1d0152abf930}";
var outSensorsXml="<sensors><sensor type=\"single\" name=\"result\"/></sensors>";

function setup()
{
	var dbList=sensorsDatabase.listSensors();
	for(var i=0;i<dbList.length;++i)
	{
		print("device: "+dbList[i].deviceId);
		print("sensor: "+dbList[i].sensorName);
	}
	script.inStorage=sensorsDatabase.existingStorage({deviceId:deviceId,sensorName:sensorName});
	if(inStorage==null)
	{
		print("No input storage found");
		return;
	}
	else
	{
		print("db_values_type: "+inStorage.valuesType());
		print("db_device_name: "+inStorage.getDeviceName());
		print("db_values_count: "+inStorage.valuesCount());
		print("db_store_mode: "+inStorage.getStoreMode());
	}

	script.device=devices.registerVirtualDevice(outDeviceId,"test.js-out",outSensorsXml);
	if(!device)
	{
		print("ERROR: output device not registered!!!!");
		return;
	}
	script.outStorage=sensorsDatabase.existingStorage({deviceId:outDeviceId,sensorName:"result"});
	if(!outStorage)
	{
		print("No output storage found");
		outStorage=sensorsDatabase.createStorage({deviceId:outDeviceId,sensorName:"result",storeMode:"last_n_values",N:100});
		if(!outStorage)
		{
			print("ERROR: can't create storage for output values!!!!");
			return;
		}
	}

	inStorage.newValueWritten.connect(function(value)
	{
		try
		{
			print("NEW VALUE: ");
			print("1");
			print(value.data);
			print("2");
			device.writeMeasurement("result",[value.data[0]+Math.random()]);
			print("3");
		}
		catch(e)
		{
			print("4");
			print(e.message);
		}
		finally
		{
			print("DONE");
		}
	});
}
setup();
