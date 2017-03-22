
function startPush() {
    var addr = $("#server-ip").val();
    var app = $("#app-name").val();
    var liveId = $("#live-id").val();
    publishWithUrl("http://" + addr + "/" + app + "/" + liveId);
    initLogClient();
}

var publishWithUrl = function(url) {
    var xhr = new XMLHttpRequest();
    xhr.onreadystatechange = function() {
        if (xhr.readyState == 4)
        {
            if ((this.status >= 200 && this.status < 300) || this.status == 304)
            {
                console.log(this.responseText);
                var fixedResponse = this.responseText.replace(/\\'/g, "'");
                return;
            }
            alert('api failed, ' + this.status + ', ' + this.responseText);
        }
    };
    xhr.open("get", "/api/publish?url=" + url, true);
    xhr.send();
};

