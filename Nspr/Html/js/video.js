
if (!window.MediaSource) {
  alert('The MediaSource API is not available on this platform');
}

var VideoSource = {
	local: {},
	remote: {},
	createLocalBuffer: function() {
		VideoSource.local.ms = new MediaSource();
		VideoSource.local.v = document.getElementById("localvideo");
		VideoSource.local.v.src = window.URL.createObjectURL(VideoSource.local.ms);
		VideoSource.local.ms.addEventListener('sourceopen', function() {
			VideoSource.local.buffer = this.addSourceBuffer('video/mp4; codecs="avc1.42e01e"');
		}, false);

		VideoSource.local.ms.addEventListener('sourceended', function() {
		}, false);

		VideoSource.local.ms.addEventListener('error', function() {
		}, false);
	},
	createRemoteBuffer: function() {
		VideoSource.remote.ms = new MediaSource();
		VideoSource.remote.v = document.getElementById("remotevideo");
		VideoSource.remote.v.src = window.URL.createObjectURL(VideoSource.remote.ms);
		VideoSource.remote.ms.addEventListener('sourceopen', function() {
			VideoSource.remote.buffer = this.addSourceBuffer('video/mp4; codecs="avc1.42e01e"');
		}, false);

		VideoSource.remote.ms.addEventListener('sourceended', function() {
		}, false);

		VideoSource.remote.ms.addEventListener('error', function() {
		}, false);
	},
	Init: function() {
		VideoSource.createLocalBuffer();
		VideoSource.createRemoteBuffer();
	}
};

var VideoPlayer = {
	urlPrefix: "ws://" + document.location.hostname + ":7788" + "/interact",
	createLocalVideo: function(url) {
		var ws = new WebSocket(url);
		ws.binaryType = 'arraybuffer';
		ws.onopen = function(evt) {
			var s = new ArrayBuffer(5);
			s[0] = 's';
			s[1] = 't';
			s[2] = 'a';
			s[3] = 'r';
			s[4] = 't';
			ws.send(s);
		};
		ws.onclose = function(evt) {
		};
		ws.onerror = function(evt) {
		};
		ws.onmessage = function(evt) {
			if (VideoSource.local.buffer.updating) {
				return;
			}
			var buffer = new Uint8Array(evt.data);
			//console.log("append buffer " + buffer.length);
			/*
			if (sourceBuffer.buffered.length >= 1) {
				console.log(sourceBuffer.buffered.start(0) + ":" + sourceBuffer.buffered.end(sourceBuffer.buffered.length - 1));
			}
			*/
			VideoSource.local.buffer.appendBuffer(buffer);
			return;
		};
	},
	createRemoteVideo: function(url) {
		var vp = {};
		var ws = new WebSocket(url);
		ws.binaryType = 'arraybuffer';
		ws.onopen = function(evt) {
			var s = new ArrayBuffer(5);
			s[0] = 's';
			s[1] = 't';
			s[2] = 'a';
			s[3] = 'r';
			s[4] = 't';
			ws.send(s);
		};
		ws.onclose = function(evt) {
		};
		ws.onerror = function(evt) {
		};
		ws.onmessage = function(evt) {
			//console.log("append buffer " + buffer.length);
			var sourceBuffer = VideoSource.remote.buffer;
			var buffer = new Uint8Array(evt.data);
			if (sourceBuffer.updating) {
				return;
			}
			if (sourceBuffer.buffered.length >= 1) {
				console.log(sourceBuffer.buffered.start(0) + ":" + sourceBuffer.buffered.end(sourceBuffer.buffered.length - 1));
			}
			try {
				sourceBuffer.appendBuffer(buffer);
			} catch (e) {
				console.log("append buffer error");
				return;
			}
			return;
		};
	},
	playLocal: function() {
		VideoPlayer.createLocalVideo(VideoPlayer.urlPrefix + "/@camera");
	},
	playRemote: function() {
		var connId = $("#remoteplayconn").val();
		if (connId) {
			VideoPlayer.createRemoteVideo(VideoPlayer.urlPrefix + "/" + connId);
		}
		else {
			alert("no playable connection");
		}
	},
	updateRemoteConnId: function(idList) {
		$("#remoteplayconn").empty();
		for (i in idList) {
			if (idList[i].slice(0, 2) == "pl") {
				optionHtml  = "<option value =";
				optionHtml += "'" + idList[i] + "'";
				optionHtml += ">";
				optionHtml += idList[i];
				optionHtml += "</option>";
				$("#remoteplayconn").append(optionHtml);
			}
		}
	}
};

