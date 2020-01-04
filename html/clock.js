function set_data(dataset) {
	
	var Request = new XMLHttpRequest();
	
	Request.open('POST', 'clock.json', 1);
	Request.setRequestHeader("DataSet",dataset);

	switch(dataset)
	{
	case 'ap':
		Request.setRequestHeader("ssid",document.getElementById("ssid").value);
		if(document.getElementById("pwd").disabled == false)
			Request.setRequestHeader("pwd",document.getElementById("pwd").value);
		else 
				Request.setRequestHeader("pwd","");
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
   		else if(Request.status == 408)	//timeout request
   		{
   			document.getElementsByTagName("BODY")[0].innerHTML = "The end";
				window.clearInterval(myTimer);
   		}
     		else
	     		document.getElementById("msg").innerHTML = "Can't set config"; 

		}
	};	
	Request.send(null);
}
	


function get_data(dataset) {

	var Request = new XMLHttpRequest();
	
	Request.open('GET', 'clock.json', 1);
	Request.setRequestHeader("DataSet", dataset);
	
	Request.onreadystatechange = function (aEvt) {
		if (Request.readyState == 4) {
   		if(Request.status == 200)
   		{
   			var Data = JSON.parse(Request.responseText);
   			populate_data(Data);
   		}
     		else
	     		document.getElementById("msg").innerHTML = "Can't fetch config"; 

		}
	};	
	Request.send(null);
}

function populate_data(Data) {
	   		
	   		if( typeof Data.Config !== 'undefined')
	   		{
   				if (typeof Data.Config.ssid !== 'undefined') {
   					document.getElementById("ssid").value = Data.Config.ssid;
   					document.getElementById("pwd").value = ""; 
   				}
   				if (typeof Data.Config.ntp !== 'undefined') {
   					document.getElementById("ntp_serv").value = Data.Config.ntp; 
   				}
   				if (typeof Data.Config.auth !== 'undefined') {
	   				document.getElementById("pwd").disabled = Data.Config.auth==='false';
   				}
   			}
   			if( typeof Data.Info !== 'undefined' )
   			{
   				if (typeof Data.Info.currentTime !== 'undefined') {
	   				document.getElementById("currentTime").innerHTML = Data.Info.currentTime;
   				}
   				if (typeof Data.Info.lastUpd !== 'undefined') {
	   				document.getElementById("lastUpd").innerHTML = Data.Info.lastUpd;
   				}
   				if (typeof Data.Info.msg !== 'undefined') {
	   				document.getElementById("msg").innerHTML = Data.Info.msg;
   				}
   				else {
	   				document.getElementById("msg").innerHTML = "";   				
   				}	
   				if (typeof Data.Info.timeout !== 'undefined') {
	   				document.getElementById("tout").innerHTML = Data.Info.timeout;
	   				currentTimeout = Data.Info.timeout;
   				}
   				if (typeof Data.Info.sdkVer !== 'undefined') {
	   				document.getElementById("sdkVer").innerHTML = Data.Info.sdkVer;
   				}
   				if (typeof Data.Info.compDate !== 'undefined') {
	   				document.getElementById("compDate").innerHTML = Data.Info.compDate;
   				}
   			}
   			else {
   				document.getElementById("msg").innerHTML = "";
   			}
}


var initialTimeout = 100;
var currentTimeout;
var myTimer;

function beginTimeout() {
	currentTimeout = initialTimeout;
	myTimer = window.setInterval(printTimeout,1000);
}

 
function printTimeout() {

	if(currentTimeout>=0)
	{
		document.getElementById("tout").innerHTML = currentTimeout;
	}			
	else //if(currentTimeout<=-10)
	{
		document.getElementsByTagName("BODY")[0].innerHTML = "Timeout. Connection closed";
		window.clearInterval(myTimer);
	}
		
	currentTimeout--;
	
}

// ---- Modal ------
function scanBtn(dataset) {

  	var Request = new XMLHttpRequest();

	Request.open('GET', 'clock.json', 1);
	Request.setRequestHeader("DataSet", dataset);
	
	Request.onreadystatechange = function (aEvt) {
		if (Request.readyState == 4) {
   		if(Request.status == 200)
   		{
   			var Data = JSON.parse(Request.responseText);
   			populate_data(Data);	//possible error messages
   			if (typeof Data.APs !== 'undefined') {
   				populate_list(Data);	
   				document.getElementById("mId").style.display = "block";	
   			}
   		}
     		else
	     		document.getElementById("msg").innerHTML = "Can't fetch AP list"; 

		}
	};	
	Request.send(null);

}

function populate_list(Data) {
	
	var listCont = document.getElementById("mContent");
	listCont.innerHTML = "";
	for(i in Data.APs) {
		listCont.innerHTML += "<div class='mItem' onclick='mChosenEvt()'>"+
			"<span class='mMAC'>" + Data.APs[i].MAC + "</span>" +
			"<span class='mSSID'>" + Data.APs[i].SSID + "</span>" +
			"<span class='mAuth'>" + Data.APs[i].Auth + "</span>" +
			"<span class='mCh'>" + Data.APs[i].Ch + "</span>" +
			"<span class='mRSSI'>" + Data.APs[i].RSSI + "</span>" +
			"<span class='mPhy'>" + Data.APs[i].Phy + "</span>" +
			"</div>";
	}
}

function mCloseEvt(e) {
  document.getElementById("mId").style.display = "none";
}

function mChosenEvt() {

	for(i in event.currentTarget.childNodes)
	{
		if(event.currentTarget.childNodes[i].className == "mSSID")
		{
			document.getElementById("ssid").value 
				= event.currentTarget.childNodes[i].innerHTML;
		}
		if(event.currentTarget.childNodes[i].className == "mAuth")
		{
			document.getElementById("pwd").value = "";
			document.getElementById("pwd").disabled 
				= event.currentTarget.childNodes[i].innerHTML == "N"; 

		}	
	}

}

