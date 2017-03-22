
var logclient = {};
var interactClient = {};

var initxyz = function() {
    logclient = LogClient.createNew();
    interactClient = InteractClient.createNew();
    
    logclient.connect();
	VideoSource.Init();

	$("#server-ip").val(document.location.hostname + ":1688");
	var t = setTimeout("loadWeatherQueryScript();", 1000);
	var t = setTimeout("loadStockQueryScript();", 2000);
};

var startinteract = function() {
    var addr = $("#server-ip").val();
    var app = $("#app-name").val();
    var liveId = $("#live-id").val();
    var direction = $("#direction").val();
    
    if (direction == "publish") {
        console.log("publish to " + addr);
        interactClient.publish(addr, app, liveId);
    }
    else if (direction == "play") {
        console.log("play from " + addr);
        interactClient.play(addr, app, liveId);
    }
	var t1 = setTimeout("WordChangeEngine.sendRequest('/api/getanotherword', WordChangeEngine.onResponse);", 2000);
};

var WordChangeEngine = {
	sendRequest : function(request, responseCB) {
		var xhr = new XMLHttpRequest();
		xhr.onreadystatechange = function() {
			if (xhr.readyState == 4) {
				if ((this.status >= 200 && this.status < 300) || this.status == 304) {
					console.log(this.responseText);
					var fixedResponse = this.responseText.replace(/\\'/g, "'");
					responseCB(fixedResponse);
					return;
				}
			}
		};
		xhr.open("get", request, true);
		xhr.send();
		console.log("send request");
	},
	onResponse : function(response) {
        var obj = JSON.parse(response);
		$("#live-id").val(obj.result);
	}
};

