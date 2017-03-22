#include "Nspr.h"

#include "InteractStreamProc.h"
#include "MediaSponsor.h"

namespace nspr
{

InteractStreamProc::InteractStreamProc()
{
    NsprInfo("Im interact stream process");
}

InteractStreamProc::~InteractStreamProc()
{
}


int InteractStreamProc::OnHandleTx(u_char *data, size_t len)
{
    if (RelayTx(data, len)) {
        return NSPR_ERROR;
    }
    return NSPR_OK;
}

int InteractStreamProc::OnHandleRx(u_char *data, size_t len)
{
    NsprDebug("websocket recv %d data", len);
    //Singleton<MediaSponsor>::GetInstance()->Launch();
#if 0
    FILE *fp = fopen("mp4files/n.mp4", "rb");
    if (!fp) {
        NsprError("open file fail");
        return NSPR_ERROR;
    }
    u_char buf[32 * 1024];
    size_t bytesRead = 0;
    do {
        bytesRead = fread(buf, 1, sizeof(buf), fp);
        if (bytesRead <= 0) {
            break;
        }
        NsprDebug("read out bytes %d", bytesRead);
        RelayTx(buf, bytesRead);
    } while (bytesRead == sizeof(buf));
    fclose(fp);
#endif
    return NSPR_OK;
}


} // namespace nspr

