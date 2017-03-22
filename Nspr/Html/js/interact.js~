
var InteractClient = {
    createNew: function() {
        var ic = {};
        
        var interactNetwork = InteractNetwork.createNew();
        var connMgmt = ConnMgmt.createNew(ic);

        ic.publish = function(addr, appname, liveid) {
            var interactRequest = "/api/publish?url=";
            interactRequest += "http://" + addr + "/" + appname + "/" + liveid;
            console.log("send publish request " + interactRequest);
            interactNetwork.sendRequest(interactRequest, ic.onPublishResponse);
        };

        ic.play = function(addr, appname, liveid) {
            var interactRequest = "/api/play?url=";
            interactRequest += "http://" + addr + "/" + appname + "/" + liveid;
            console.log("send play request " + interactRequest);
            interactNetwork.sendRequest(interactRequest, ic.onPlayResponse);
        };

		ic.query = function() {
            var interactRequest = "/api/query";
            console.log("send query request " + interactRequest);
            interactNetwork.sendRequest(interactRequest, ic.onQueryResponse);
		};

        ic.onDestroy = function(connId) {
            var interactRequest = "/api/close?connId=" + connId;
            console.log("send close request" + interactRequest);
            interactNetwork.sendRequest(interactRequest, ic.onCloseResponse);
        };

        ic.onPublishResponse = function(response) {
            var obj = JSON.parse(response);
            connMgmt.AddConn(obj.connId);
        };

        ic.onPlayResponse = function(response) {
            var obj = JSON.parse(response);
            connMgmt.AddConn(obj.connId);
        };

        ic.onCloseResponse = function(response) {
            var obj = JSON.parse(response);
            connMgmt.DelDone(obj.connId);
        };
		ic.onQueryResponse = function(response) {
            var arr = JSON.parse(response);
            connMgmt.UpdateStatus(arr);
		}
		setInterval( function() { ic.query(); }, 1000 );
        return ic;
    }, 

};

var InteractNetwork = {
    createNew: function() {
        var itn = {};
        itn.sendRequest = function(request, responseCB) {
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
        };
        return itn;
    }
};

var ConnMgmt = {
    createNew: function(ic) {
        var cm = {};
        var connArray = new Array();
        var connStatus = {};

        cm.getConnIdByIndex = function(index) {
            return connArray[index];
        };

        cm.delConnIdByIndex = function(index) {
            ic.onDestroy(connArray[index]);
            connArray.splice(index, 1);
        };
        cm.GetConnStatusById = function(id) {
            return connStatus[id];
        };

        var connUI = ConnectionUI.createNew(cm);
        cm.AddConn = function(id) {
            connArray.push(id);
            console.log("add " + id);
            connUI.updateConnectionList(connArray);
        };

        cm.DelDone = function(id) {
            console.log("delete id:" + id + " done");
        };

        cm.UpdateStatus = function(remoteArray) {
            var index;
            var change = false;
            if (remoteArray.length != connArray.length) {
                connArray.splice(0, connArray.length);
                for (index in remoteArray) {
                    connArray.push(remoteArray[index].connId);
                    connStatus[remoteArray[index].connId] = remoteArray[index].status;
                }
                change = true;
            }
            for (index in remoteArray) {
                if (remoteArray[index].connId != connArray[index]) {
                    connArray[index] = remoteArray[index].connId;
                    change = true;
                }
                if (connStatus[connArray[index]] != remoteArray[index].status) {
                    connStatus[connArray[index]] = remoteArray[index].status;
                    change = true;
                }
            }
            if (change) {
                console.log("changed");
                connUI.updateConnectionList(connArray);
            }
        }

        cm.GetConnUI = function() {
            return connUI;
        };

        cm.GetConnArray = function() {
            return connArray;
        };
        return cm;
    }
};

$('#conn-id').bind({
    click: function() {
        console.log("click");
    },
    mouseover : function() {
        console.log("over " + this.id);
    }
});

var ConnectionUI = {
    createNew : function(connMgmt) {
        var cui = {};
        ConnectionUI.connMgmt = connMgmt;
        var getConnId = connMgmt.getConnIdByIndex;
        cui.updateConnectionList = function(connArray) {
            $("#conn-id").empty();
            var index = 0;
            for (index in connArray) {
                $("#conn-id").append(cui.getConnectionListHtml(index));
                var stat = connMgmt.GetConnStatusById(connArray[index]);
                console.log(connArray[index] + "'s status is " + stat);
                if (stat && stat == "connecting") {
                    $('#' + connArray[index]).css("color", "#f00");
                }
                else if (stat && stat == "connected") {
                    $('#' + connArray[index]).css("color", "#050");
                }
            }
            ConnectionList.update();
        };

        cui.getConnectionListHtml = function(index) {
            var connId = getConnId(index);
            var listHtml = "<span id='" + connId + "'" + " >" + connId + "</span>";
            listHtml += "<a class='delConn' onclick='ConnectionUI.destroyConn(" + index.toString() + ")'>x</a>";
            listHtml += "<br/>";
            return listHtml;
        };
        return cui;
    },
    destroyConn : function(index) {
        $("#conn-id").empty();
        ConnectionUI.connMgmt.delConnIdByIndex(index);
        console.log(ConnectionUI.connMgmt.GetConnUI());
        ConnectionUI.connMgmt.GetConnUI().updateConnectionList(ConnectionUI.connMgmt.GetConnArray());
    },
    connMgmt : {}
    
};

var ConnectionList = {
    isEditing : false,

    update : function() {
        if (ConnectionList.isEditing) {
            $('.delConn').css("display", "inline");
            $('.editConn').css("width", "3.3em");
            $('#edit-conn-span').text("FINISH");
        }
        else {
            $('.delConn').css("display", "none");
            $('.editConn').css("width", "2.2em");
            $('#edit-conn-span').text("EDIT");
        }
    },

    changeMode : function() {
        ConnectionList.isEditing = !ConnectionList.isEditing;
        ConnectionList.update();
    }
}

$('#edit-conn').bind({
    click: ConnectionList.changeMode
});


