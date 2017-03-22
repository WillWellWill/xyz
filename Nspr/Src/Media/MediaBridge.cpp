#include "Nspr.h"

#include "MediaBridge.h"
#include "StreamSender.h"
#include "StreamProcFactory.h"
#include "StreamProc.h"

namespace nspr
{

MediaBridge::MediaBridge(const char *ip)
{
	memcpy(m_ip, ip, strlen(ip) + 1);
}

MediaBridge::~MediaBridge()
{
}

int MediaBridge::BuildBridge(Metadata *metadata)
{
	if (metadata->mode == IC_MODE_PUBLISH) {
		return BuildPublishBridge(metadata);
	}
	else if (metadata->mode == IC_MODE_PLAY) {
		return BuildPlayBridge(metadata);
	}
	return NSPR_ERROR;
}

int MediaBridge::BuildPublishBridge(Metadata *metadata)
{
	const int &vport = metadata->v.dstPort;
	const int &aport = metadata->a.dstPort;
	NsprDebug("video dest port: %d, audio dest port: %d", vport, aport);
	
	u_char addr[128]= {0};
	snprintf((char *)addr, sizeof(addr), "%s:%d", m_ip, vport);
	StreamProc *videoSender = new StreamSenderProc(addr);

	StreamProc *sourceProc = Singleton<StreamProcFactory>::GetInstance()->GetStreamProc((char *)"rtp://camera");
    if (!sourceProc) {
        return NSPR_ERROR;
    }
	sourceProc->AttachTxChannel(videoSender);
	return NSPR_OK;
}

int MediaBridge::BuildPlayBridge(Metadata *metadata)
{
	const int &vport = metadata->v.dstPort;
	const int &aport = metadata->a.dstPort;
	NsprDebug("video dest port: %d, audio dest port: %d", vport, aport);
	return NSPR_OK;
}

} // namespace nspr

