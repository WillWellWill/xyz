#include "Nspr.h"

#include "LogStreamProc.h"

namespace nspr
{

LogStreamProc::LogStreamProc()
{
    NsprInfo("Im log stream process");
}

LogStreamProc::~LogStreamProc()
{
}


int LogStreamProc::OnHandleTx(u_char *data, size_t len)
{
    return NSPR_OK;
}

int LogStreamProc::OnHandleRx(u_char *data, size_t len)
{
    if (!strncmp((char *)data, "get_server_name", 15)) {
        u_char *response = (u_char *)"gsn:HAHA";
        RelayTx(response, strlen((char *)response));
    }
    else if (!strncmp((char *)data, "get_log_stream", 14)) {
        //u_char *response = (u_char *)"log:first log message";
        //RelayTx(response, strlen((char *)response));
    }
    else {
        NsprDebug("seq = %d", *(uint16_t *)data);
        data += 2;
        u_char response[1500] = {0};
        if (len > sizeof(response) - 4) {
            len = sizeof(response) - 4;
        }
        memcpy(response, "log:", 4);
        memcpy(response + 4, data, len - 2);
        RelayTx(response, len - 2 + 4);
        NsprDebug("%s", (char *)response);
    }
    return NSPR_OK;
}


} // namespace nspr

