

var Settings = {
	AddIP : function() {
		var nim = "";
		if (localStorage.nspr_ip_manager) {
			nim = localStorage.nspr_ip_manager;
		}
			
		var newip = $("#manage-ip").val();
		if (nim == "") {
			nim += newip;
		}
		else {
			nim += "$" + newip;
		}
		localStorage.nspr_ip_manager = nim;
		$("#manage-ip-hint").text(newip + " added successfully");
		$("#manage-ip").val("");
	},
	GetIP : function() {
		var nim = localStorage.nspr_ip_manager;
		if (nim) {
			return nim.split("$");
		}
		return [];
	}
};

