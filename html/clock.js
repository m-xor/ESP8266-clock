function set_data(dataset) {
	
	var Request = new XMLHttpRequest();
	
	Request.open('POST', 'data.json', 1);
	Request.setRequestHeader("Action","set");
	Request.setRequestHeader("DataSet",dataset);

	switch(dataset)
	{
	case 'ap':
		Request.setRequestHeader("ssid",document.getElementById("ssid").value);
		Request.setRequestHeader("pwd",document.getElementById("pwd").value);
		break;
	case 'ntp' :
		Request.setRequestHeader("ntp",document.getElementById("ntp_serv").value);
		break
	default:
		break; 
	}
		Request.onreadystatechange = function (aEvt) {
		if (Request.readyState == 4) {
   		if(Request.status == 200)
   		{
   			var Data = JSON.parse(Request.responseText);
   			populate_data(Data);
   		}
     		else
	     		document.getElementById("msg").innerHTML = "Possible problem with setting configuration"; 

		}
	};	
	Request.send(null);
}
	


function get_data(dataset) {

	var Request = new XMLHttpRequest();

	
	Request.open('POST', 'data.json', 1);
	Request.setRequestHeader("Action","get");
	Request.setRequestHeader("DataSet", dataset);
	
	Request.onreadystatechange = function (aEvt) {
		if (Request.readyState == 4) {
   		if(Request.status == 200)
   		{
   			var Data = JSON.parse(Request.responseText);
   			populate_data(Data);
   		}
     		else
	     		document.getElementById("msg").innerHTML = "Can't fetch config data"; 

		}
	};	
	Request.send(null);
}

function populate_data(Data) {
	   		
   			console.log(Data);
   			if (typeof Data.Config.ssid !== 'undefined') {
   				document.getElementById("ssid").value = Data.Config.ssid; 
   			}
   			if (typeof Data.Config.ntp !== 'undefined') {
   				document.getElementById("ntp_serv").value = Data.Config.ntp; 
   			}
   			if (typeof Data.Config.auth !== 'undefined') {
	   			document.getElementById("pwd").disabled = Data.Config.auth==='false';
   			}
   			
   			if (typeof Data.Info.currentTime !== 'undefined') {
	   			document.getElementById("currentTime").innerHTML = Data.Info.currentTime;
   			}
   			if (typeof Data.Info.lastUpd !== 'undefined') {
	   			document.getElementById("lastUpd").innerHTML = Data.Info.lastUpd;
   			}
   			if (typeof Data.Info.msg !== 'undefined') {
	   			document.getElementById("msg").innerHTML = Data.Info.msg;
   			}

}

function getList() {
	document.getElementById("msg").innerHTML = "to be done";
}

var initialTimeout = 100;
var currentTimeout;

function beginTimeout() {
	currentTimeout = initialTimeout;
	window.setInterval(printTimeout,1000);
}

function resetTimeout() {
	currentTimeout = initialTimeout;
}
 
function printTimeout() {
	document.getElementById("tout").innerHTML = currentTimeout;	
	if(currentTimeout==0)
		location.replace("close.html");
	currentTimeout--;
	
}