function set_data(dataset) {
	
	var Request = new XMLHttpRequest();
	
	Request.open('POST', 'clock.json', 1);
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

	
	Request.open('POST', 'clock.json', 1);
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
	   		
 //  			console.log(Data);
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
   			if (typeof Data.Info.timeout !== 'undefined') {
	   			document.getElementById("tout").innerHTML = Data.Info.timeout;
	   			currentTimeout = Data.Info.timeout;
   			}
}

function getList() {
	document.getElementById("msg").innerHTML = "to be done";
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
	else if(currentTimeout<=-10)
	{
		document.getElementById("msg").innerHTML = "Time's up";
		window.clearInterval(myTimer);
	}
		
	currentTimeout--;
	
}


// When the user clicks on the button, open the modal
function scanBtn(dataset) {
  //zapytanie o AP
  	var Request = new XMLHttpRequest();

	
	Request.open('POST', 'clock.json', 1);
	Request.setRequestHeader("Action","get");
	Request.setRequestHeader("DataSet", dataset);
	
	Request.onreadystatechange = function (aEvt) {
		if (Request.readyState == 4) {
   		if(Request.status == 200)
   		{
   			var Data = JSON.parse(Request.responseText);
   			populate_data(Data);	//possible error messages
   			if (typeof Data.APs !== 'undefined') {
   				populate_list(Data);	//
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
			document.getElementById("pwd").disabled 
				= event.currentTarget.childNodes[i].innerHTML == "N"; 

		}	
	}

}

