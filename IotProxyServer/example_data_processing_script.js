//параметры датчика с реального устройства, значения которого мы будем обрабатывать
var deviceId="{f84526c1-5e88-4315-81f8-f7da45daa09d}";
var sensorName="blinks_count";

//параметры виртуального устройства, которое будет использоваться для сохранения результатов обработки и обработки команд от пользователя
var outDeviceId="{129ed0af-42ba-4f4e-a71c-1d0152abf930}";
var outSensorsXml="<sensors><sensor type=\"sv_f32\" name=\"result\"/></sensors>";//один датчик - result
var outControlsXml="<controls><group title=\"Device controls\"><control command=\"blink\" title=\"Blink\" button_text=\"blink\"/></group></controls>";//одна команда - blink

function setup()
{
	//отладка - выводим список всех имеющихся хранилищ
	var dbList=SensorsDatabase.listSensors();
	for(var i=0;i<dbList.length;++i)
	{
		print("device: "+dbList[i].deviceId);
		print("sensor: "+dbList[i].sensorName);
	}

	//регистрируем виртуальное устройство
	device=Devices.registerVirtualDevice(outDeviceId,"test.js-out",outSensorsXml,outControlsXml);
	if(!device)
	{
		print("ERROR: output device not registered!!!!");
		return;
	}

	//ищем хранилище для датчика с реального устройства
	inStorage=SensorsDatabase.existingStorage({deviceId:deviceId,sensorName:sensorName});
	if(inStorage==null)
	{
		print("No input storage found");
	}
	else
	{
		//выводим параметры хранилища
		print("db_values_type: "+inStorage.valuesType());
		print("db_device_name: "+inStorage.deviceName());
		print("db_values_count: "+inStorage.valuesCount());
		print("db_store_mode: "+inStorage.getStoreMode());

		//устанавливаем обработчик, которая будет вызываться сервером при появлении нового значения
		inStorage.newValueWritten.connect(function(value)
		{
			try
			{
				//выводим значение
				print("NEW VALUE: ");
				print(value.data);
				//производим "очень сложную" обработку и выдаем результат как "измерение" с датчика виртуального устройства
				device.writeMeasurement("result",{data:[value.data[0]+Math.random()]});
			}
			catch(e)
			{
				print(e.message);
			}
			finally
			{
				print("DONE");
			}
		});

		//ищем хранилище для результатов обработки
		outStorage=SensorsDatabase.existingStorage({deviceId:outDeviceId,sensorName:"result"});
		if(!outStorage)
		{
			print("No output storage found");
			//если не нашли - создаем
			outStorage=SensorsDatabase.createStorage({deviceId:outDeviceId,deviceName:"test.js-out",sensorName:"result",storeMode:"last_n_values",N:100});
			if(!outStorage)
			{
				print("ERROR: can't create storage for output values!!!!");
				return;
			}
		}
	}

	//устанавливаем функцию-обработчик команд
	device.setCommandCallback(function(cmd,args){
		if(cmd=="blink")//команда blink
		{
			//можно было бы мигнуть светодиодом, но у виртуального устройства его нет
			print("Wow, we have no led in a script, but we will try");
			print("Blink, BLINK!! Just do this!!!!");
			//команда "выполнена" успешно, возвращаем true
			return {done:true,result:["we did it, i think","however, we tried"]};
		}
		else return {done:false,result:["unknown command"]};//неизвестная команда
	});
};

setup();

