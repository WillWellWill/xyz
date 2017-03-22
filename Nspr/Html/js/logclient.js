
var LogClient = {
    addr: "ws://" + document.location.hostname + ":7788/log",
    createNew: function() {
        var lc = {};
        
        var cstatus = "disconnected";
        var requestType = {};
        requestType.gsn = "get_server_name";
        requestType.glm = "get_log_stream";
        
        lc.connect = function() {
            var wsfd = new WebSocket(LogClient.addr);
            wsfd.onopen = function(evt) {
                console.log("connect " + LogClient.addr + " successfully");
                cstatus = "connected";
                
                console.log("sending " + requestType.gsn);
                wsfd.send(requestType.gsn);
            };

            wsfd.onclose = function(evt) {
                console.log("disconnect from " + LogClient.addr);
                cstatus = "disconnected";
            };

            wsfd.onmessage = function(evt) {
                console.log("message from " + LogClient.addr + ": ");
                console.log("\t" + evt.data);

                var messageType = evt.data.slice(0, 4);
                var message = evt.data.slice(4);
                switch (messageType) {
                    case "gsn:": // response of get_server_name
                        lc.changeServerName(message);
                        wsfd.send(requestType.glm);
                        break;
                    case "log:":
                        lc.logMessage(message);
                        break;
                }
            };

            wsfd.onerror = function(evt) {
                console.log("error happened with " + LogClient.addr);
                cstatus = "error";
                wsfd.close();
            };
        };

        lc.changeServerName = function(serverName) {
            var logHeader = {};
            logHeader["disconnected"] = "No Log Message";
            logHeader["connected"] = "Log From " + serverName;
            logHeader["error"] = "Error Connection From " + serverName;
            $("#log-header").text(logHeader[cstatus]);
        };

        lc.logMessage = function(message) {
            var lines = message.split('\n');
            console.log("line number = " + lines.length);
            var lineNum = lines.length - 1;
            var content = lines[0];
            if (lineNum > 1) {
                content += "<span class='lineExtension' ";
                content += "id='lineExtensionCmd_" + LogParser.extensionCount + "' ";
                content += "onclick='LogParser.lineExtend(" + LogParser.extensionCount + ")'";
                content += " >";
                content += "+";
                content += "</span>";

                var extension = "";
                extension += "<div id='lineExtensionContent_" + LogParser.extensionCount + "'><br/>";
                for (var i = 1; i < lineNum; i++) {
                    extension += lines[i] + "<br/>";
                }
                extension += "</div>";
                LogParser.AddExtension(extension);
                content += extension;
            }
            $("#log-prograph").append(content + "<br/>");

            for (var i = 0; i < LogParser.extensionCount; i++) {
                $("#lineExtensionContent_" + i.toString()).css("display", "none");
            }
            $("#logtext").scrollTop($("#logtext")[0].scrollHeight);
        };

        return lc;
    }
};


var LogParser = {
    AddExtension : function(content) {
        LogParser.extensionContent[LogParser.extensionCount] = {};
        LogParser.extensionContent[LogParser.extensionCount].data = content;
        LogParser.extensionContent[LogParser.extensionCount].isShow = false;
        LogParser.extensionCount++;
    },
    lineExtend : function(index) {
        var shouldShow = !LogParser.extensionContent[index].isShow;
        if (shouldShow) {
            $("#lineExtensionContent_" + index.toString()).css("display", "inline");
            $("#lineExtensionCmd_" + index.toString()).text("-");
        }
        else {
            $("#lineExtensionContent_" + index.toString()).css("display", "none");
            $("#lineExtensionCmd_" + index.toString()).text("+");
        }
        LogParser.extensionContent[index].isShow = shouldShow;
    },
    extensionContent : new Array(),
    extensionCount : 0
};


