
function openNav() {
    $("#mySidenav").css("width", "12%");
}

function closeNav() {
    $("#mySidenav").css("width", "0");
}

function hideAllContainer() {
    $(".start-container").css("display", "none");
    $(".mgmt-container").css("display", "none");
    $(".video-container").css("display", "none");
    $(".settings-container").css("display", "none");
    $(".tool-container").css("display", "none");
}

var currentId = 'navStart';
function selectContainer(id) {
    if (currentId == id) {
        return;
    }

    hideAllContainer();
    if (id == 'navStart') {
        $(".start-container").css("display", "block");
    }
    else if (id == 'navMgmt') {
		MgmtInitialize.run();
        $(".mgmt-container").css("display", "block");
    }
    else if (id == 'navVideo') {
		VideoPlayer.updateRemoteConnId(ConnectionUI.connMgmt.GetConnArray());
		console.log(ConnectionUI.connMgmt.GetConnArray());
        $(".video-container").css("display", "block");
    }
    else if (id == 'navSettings') {
        $(".settings-container").css("display", "block");
    }
    else if (id == 'navTool') {
        $(".tool-container").css("display", "block");
    }
    currentId = id;
    closeNav();
}


