#include "Nspr.h"

#include "StreamProcFactory.h"
#include "InteractStreamProc.h"
#include "LogStreamProc.h"
#include "MediaSponsor.h"
#include "VideoEncoder.h"
#include "Mp4BoxPackerProc.h"
#include "RtpPacker.h"
#include "RtpParser.h"
#include "ProtocolSponsor.h"
#include "ProtocolConnection.h"
#include "StreamSender.h"
#include "Utility.h"

namespace nspr
{

StreamProcFactory::StreamProcFactory()
	: m_cameraParameter(nullptr)
{
    m_log[std::string("haha")] = new LogStreamProc();
}

StreamProcFactory::~StreamProcFactory()
{

}

StreamProc *StreamProcFactory::GetStreamProc(char *url)
{
    if (!ngx_strncmp(url, "interact://@camera", 18)) {
        VideoEncoderProc *srcProc = Singleton<MediaSponsor>::GetInstance()->GetSourceProc();
        if (!srcProc) {
            return nullptr;
        }
        
        Mp4BoxPackerProc *boxPacker = new Mp4BoxPackerProc(srcProc->GetParameter());
        InteractStreamProc *interactStream = new InteractStreamProc();

        srcProc->AttachTxChannel(boxPacker);
        boxPacker->AttachTxChannel(interactStream);

		m_cameraParameter = srcProc->GetParameter();
		NsprInfo("I catch the camera parameter %p", m_cameraParameter);
        return interactStream;
    }
	else if (!ngx_strncmp(url, "interact://", 11)) {
		char *connId = url + 11;
		NsprInfo("will play video connId:%s", connId);
		ProtocolConnection *pc = Singleton<ProtocolSponsor>::GetInstance()->SearchConnection((u_char *)connId);
		if (!pc) {
			NsprError("search connection failed by connId:%s", connId);
			return new EmptyStreamProc();
		}
		u_char addr[128] = {0};
		char ip[64] = {0};
		Utility u;
		u.IpFromConnId((u_char *)connId, (u_char *)ip, sizeof(ip));
		
		snprintf((char *)addr, sizeof(addr), "%s:%d", ip, pc->GetMetadata()->v.srcPort);
		NsprInfo("get address to play video: %s", addr);
        
        auto it = m_stream.find(std::string((char *)addr));
        if (it != m_stream.end()) {
            NsprInfo("get exist streamproc of %s", addr);
            Mp4BoxPackerProc *mp = (Mp4BoxPackerProc *)it->second;
            mp->Reset();
            return mp;
        }
        
		StreamSenderProc *peer = new StreamSenderProc(addr);
		int const localPort = peer->GetLocalPort();
		if (localPort >= 0) {
			char portStr[64] = {0};
			snprintf(portStr, sizeof(portStr), "localport-%d", localPort);
			m_src[std::string(portStr)] = peer;
			NsprInfo("set %p with port %d", peer, localPort);
		}
		StreamProc *rtpParser = new RtpParserProc();
		peer->AttachRxChannel(rtpParser);

		if (!m_cameraParameter) {
			NsprError("get no camera parameter");
			return new EmptyStreamProc();
		}
        Mp4BoxPackerProc *boxPacker = new Mp4BoxPackerProc(m_cameraParameter);
		rtpParser->AttachTxChannel(boxPacker);
		peer->OnHandleTx((u_char *)"hello", 5);
        m_stream[std::string((char *)addr)] = boxPacker;
		return boxPacker;
	}
	else if (!ngx_strncmp(url, "rtp://camera", 12)) {
		VideoEncoderProc *srcProc = Singleton<MediaSponsor>::GetInstance()->GetSourceProc();
		if (!srcProc) {
			NsprError("get camera source failed");
			return nullptr;
		}
		m_cameraParameter = srcProc->GetParameter();
		RtpPackerProc *rtpPackerProc = new RtpPackerProc();
		if (srcProc->AttachTxChannel(rtpPackerProc)) {
			NsprError("camera attach rtp packer failed");
			return nullptr;
		}
		return rtpPackerProc;
	}
    else if (!ngx_strncmp(url, "log://", 6)) {
        return m_log[std::string("haha")];
    }
    return new EmptyStreamProc();
}

StreamProc *StreamProcFactory::GetStreamProc(int port, const char *desc)
{
	(void)desc;
    switch (port) {
    case m_logFixedPort:
        return m_log[std::string("haha")];
    default:
        return ProcessPortRange(port);
    }
    return nullptr;
}

StreamProc *StreamProcFactory::ProcessPortRange(int port)
{
	char portStr[64] = {0};
	snprintf(portStr, sizeof(portStr), "localport-%d", port);
	std::map<std::string, StreamProc *>::iterator it = m_src.find(std::string(portStr));
	if (it == m_src.end()) {
		return nullptr;
	}
	//NsprInfo("find proc %p, with port %d", it->second, port);
    return it->second;
}

} // namespace nspr

