var deviceId="{0ca36183-95f1-4078-bdbe-9b2397cfdb09}";
var deviceName="test_vdev";
var sensorsXml="<sensors><sensor type=\"single\" name=\"sin_x\"/><sensor type=\"single\" name=\"rand\"/></sensors>";
var controlsXml="<controls><group title=\"Device controls\"><control title=\"Random\" command=\"rand\"/></group></controls>";

function setup()
{
	script.device=Devices.registerVirtualDevice(deviceId,deviceName);
	device.setSensorsXml(sensorsXml);
	device.setControlsXml(controlsXml);
	if(!device)
	{
		print("ERROR: output device not registered!!!!");
		return;
	}
	var commandFunc=function(cmd,args)
	{
		if(cmd=="rand")
		{
			script.device.writeMeasurement("rand",[Math.random()]);
			return [];
		}
		return null;
	};
	device.setCommandCallback(commandFunc);
	var t=0;
	Timer.setInterval(function()
	{
		device.writeMeasurement("sin_x",[Math.sin(0.01*t),Math.cos(0.01*t)]);
		++t;
	},1000);
};
setup();

