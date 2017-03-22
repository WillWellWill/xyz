
var MgmtInitialize = {
	run : function() {
		var iplist = Settings.GetIP();
		$("#server-addr-list").empty();
		for (i in iplist) {
			optionHtml  = "<option value =";
			optionHtml += "'" + iplist[i] + "'";
			optionHtml += ">";
			optionHtml += iplist[i];
			optionHtml += "</option>";
			$("#server-addr-list").append(optionHtml);
		}
	},
	currentIP : function() {
		var ipSelected = $("#server-addr-list").val();
		if (ipSelected) {
		}
		else {
			return "10.1.13.196";
		}
		return ipSelected;
	}
};

var DiagramUI = {
	createNew : function(sd, width, height) {
		var dui = {};
		var topPercent = 7;
		var botPercent = 8;
		var leftPercent = 7;
		var rightPercent = 8;

		var leftTopX = width * leftPercent / 100;
		var leftTopY = height * topPercent / 100;
		
		var leftBotX = width * leftPercent / 100;
		var leftBotY = height - (height * botPercent / 100);

		var rightTopX = width - (width * rightPercent / 100);
		var rightTopY = height * topPercent / 100;

		var rightBotX = width - (width * rightPercent / 100);
		var rightBotY = height - (height * botPercent / 100);
	
		var c = document.getElementById("my-screen");
		var ctx = c.getContext("2d");
		ctx.strokeStyle = "#0000ff";
		dui.drawCoordinate = function() {
			ctx.beginPath();
			ctx.moveTo(leftTopX, leftTopY);
			ctx.lineTo(leftBotX, leftBotY);
			ctx.lineTo(rightBotX, rightBotY);

			var vertCount = sd.getVertCount()
			var vertCoordLen = leftBotY - leftTopY;
			ctx.font = "30px Arial";
			for (var i = 0; i < vertCount; i++) {
				var curX = leftTopX;
				var curY = leftBotY - i * (vertCoordLen / vertCount);

				if (i != 0) {
					ctx.moveTo(curX, curY);
					ctx.lineTo(curX - 10, curY);
					ctx.moveTo(curX, curY);
					ctx.lineTo(curX + 10, curY);
				}

				var name = sd.getVertValueByIndex(i);
				var nameWidth = ctx.measureText(name).width;
				var nameHeight = -15;
				
				var fontLeftTopX = curX - nameWidth - 20;
				var fontLeftTopY = curY - nameHeight / 2;
				ctx.fillText(name, fontLeftTopX, fontLeftTopY);
				console.log(name + " " + fontLeftTopX.toString() + " " + nameWidth.toString());
			}

			var horiCount = sd.getHoriCount();
			var horiCoordLen = rightBotX - leftBotX;
			for (var i = 0; i < horiCount; i++) {
				var curX = leftBotX + i * (horiCoordLen / horiCount);
				var curY = leftBotY;
				if (i != 0) {
					ctx.moveTo(curX, curY);
					ctx.lineTo(curX, curY - 3);
					ctx.moveTo(curX, curY);
					ctx.lineTo(curX, curY + 3);
				}
				if (sd.getHoriName(i)) {
					if (i != 0) {
						ctx.lineTo(curX, curY + 15);
					}
					var name = sd.getHoriName(i);
					var nameWidth = ctx.measureText(name).width;
					var nameHeight = 80;
					
					var fontLeftTopX = curX - nameWidth / 2;
					var fontLeftTopY = curY + nameHeight / 2;
					ctx.fillText(name, fontLeftTopX, fontLeftTopY);
				}
			}
			ctx.stroke();
			ctx.closePath();
		};

		var index = 0;
		dui.linkIn = function() {
			ctx.beginPath();
			var value = sd.getHoriNext();
			var horiCount = sd.getHoriCount();
			var horiCoordLen = rightBotX - leftBotX;
			var vertCount = sd.getVertCount()
			var vertCoordLen = leftBotY - leftTopY;
			while (value != -1) {
				console.log(value);

				var pointX = leftBotX + index * (horiCoordLen / horiCount);
				var pointY = leftBotY - value * (vertCoordLen / vertCount);
				if (index == 0) {
					ctx.moveTo(pointX, pointY);
				}
				else {
					ctx.lineTo(pointX, pointY);
				}
				ctx.arc(pointX, pointY, 3, 0, 2*Math.PI);
				value = sd.getHoriNext();
				index += 1;
			}
			ctx.stroke();
			ctx.closePath();
		};

		dui.clear = function() {
			ctx.clearRect(0, 0, width, height);
			index = 0;
		};

		return dui;
	}
};

var SmartDiagram = {
	createNew : function() {
		var sd = {};
		var slf = {};
		slf.verts = [];
		slf.horiBegin = 0;
		slf.horiEnd = 0;
		sd.addVert = function(value, isShow) {
			var vert = {};
			vert.value = value;
			vert.show = isShow;
			slf.verts.push(vert);
		};
		sd.addHori = function(idx, name) {
			slf.horis[idx].name = name;
		};
		
		sd.addHoriCount = function(count) {
			if (slf.horis && slf.horis.length > 0) {
				slf.horis.splice(0, slf.horis.length);
			}
			slf.horis = new Array(count);
			for (var i = 0; i < count; i++) {
				slf.horis[i] = {};
			}
			slf.horiBegin = 0;
			slf.horiEnd = 0;
		};

		sd.getVertCount = function() {
			return slf.verts.length;
		};

		sd.getVertValueByIndex = function(index) {
			return slf.verts[index].value;
		};

		sd.getHoriCount = function() {
			return slf.horis.length;
		};

		sd.getHoriNext = function() {
			if (slf.horiBegin != slf.horiEnd) {
				var res = slf.horis[slf.horiBegin].value;
				slf.horiBegin += 1;
				return res;
			}
			return -1;
		};
		sd.getHoriName = function(idx) {
			return slf.horis[idx].name;
		};
		sd.addPoint = function(v) {
			slf.horis[slf.horiEnd].value = v;
			slf.horiEnd += 1;
		};
		return sd;
	}
};

var PortMgmt = {
	portDesc : {},
	Update : function(pd) {
		if (!pd.windowGap) {
			pd.completeShow = true;
			pd.allstatus = [];
			pd.showPointer = 0;
			pd.windowGap = 200;
			pd.sd = SmartDiagram.createNew();
			pd.sd.addVert("", true);
			pd.sd.addVert("idle", true);
			pd.sd.addVert("busy", true);
			pd.sd.addHoriCount(pd.windowGap);
			pd.dui = DiagramUI.createNew(pd.sd, 1600, 900);
		}
		if (pd.completeShow) {
			ApiProcNetwork.sendRequest("http://" + MgmtInitialize.currentIP() + ":7777/api/port/query?type=publish", PortMgmt.OnUpdate);
		}
		else {
			PortMgmt.ShowMore(pd);
		}

	},
	ShowMore : function(pd) {
		console.log("not yet complete");
		pd.sd.addHoriCount(pd.windowGap);
		pd.sd.addHori(0, pd.allstatus[pd.showPointer].port);
		pd.sd.addHori(49, pd.allstatus[pd.showPointer + 49].port);
		pd.sd.addHori(99, pd.allstatus[pd.showPointer + 99].port);
		pd.sd.addHori(149, pd.allstatus[pd.showPointer + 149].port);
		pd.sd.addHori(199, pd.allstatus[pd.showPointer + 199].port);
		var i, j = 0;
		for (i = pd.showPointer; i < pd.allstatus.length; i++, j++) {
			if (j == pd.windowGap) {
				pd.showPointer = i;
				console.log(pd.showPointer);
				break;
			}
			pd.sd.addPoint(pd.allstatus[i].stat);
		}
		if (i >= pd.allstatus.length) {
			pd.completeShow = true;
		}
		else {
			pd.completeShow = false;
		}
		pd.dui.clear();
		pd.dui.drawCoordinate();
		pd.dui.linkIn();
	},
	OnUpdate : function(response, pd) {
        var obj = JSON.parse(response);
		if (obj.result) {
			alert(obj.result);
			return;
		}
		var portArray = obj;
		pd.allstatus.splice(0, pd.allstatus.length);
		var index;
		for (index in portArray) {
			var value = {};
			value.port = portArray[index].port;
			value.stat = 0;
			if (portArray[index].status == 0) {
				value.stat = 1.0;
			}
			else if (portArray[index].status == 1) {
				value.stat = 2.0;
			}
			pd.allstatus.push(value);
		}
		pd.showPointer = 0;
		pd.completeShow = false;
		PortMgmt.ShowMore(pd);
		return;
	}
};

var ApiProcNetwork = {
	sendRequest : function(request, responseCB) {
		var xhr = new XMLHttpRequest();
		xhr.onreadystatechange = function() {
			if (xhr.readyState == 4) {
				if ((this.status >= 200 && this.status < 300) || this.status == 304) {
					console.log(this.responseText);
					var fixedResponse = this.responseText.replace(/\\'/g, "'");
					responseCB(fixedResponse, PortMgmt.portDesc);
					return;
				}
			}
		};
		xhr.open("get", request, true);
		xhr.send();
		console.log("send request");
	}
};

