#include "Nspr.h"

#include "MediaSponsor.h"
#include "VideoSource.h"
#include "VideoEncoder.h"
#include "RtpPacker.h"
#include "StreamSender.h"
#include "Mp4Packer.h"

#include <pthread.h>

namespace nspr
{

MediaSponsor::MediaSponsor()
    : m_capturePid(nullptr)
    , m_vsProc(nullptr)
    , m_veProc(nullptr)
{
}

MediaSponsor::~MediaSponsor()
{
    if (m_veProc) {
        delete m_veProc;
    }
    if (m_vsProc) {
        delete m_vsProc;
    }
}

void *source_loop_run(void *param)
{
    MediaSponsor *mediaSponsor = (MediaSponsor *)param;
    mediaSponsor->LoopRun();
    mediaSponsor->Exit();
    return nullptr;
}


int MediaSponsor::OpenSource(const u_char *desc)
{
    if (!m_capturePid) {
        m_vsProc = new VideoSourceProc(VideoSource::RESOLUTION_VGA);
        if (m_vsProc->StreamStart()) {
            return NSPR_ERROR;
        }
        
        m_veProc = new VideoEncoderProc(m_vsProc->GetParameter());
        
        if (m_vsProc->AttachTxChannel(m_veProc)) {
            return NSPR_ERROR;
        }

#if 0
        Mp4PackerProc *mpProc = new Mp4PackerProc(m_vsProc->GetParameter());
        m_veProc->AttachTxChannel(mpProc);
        mpProc->AttachTxChannel(new EmptyStreamProc());
#endif

        if (pthread_create(&m_capturePid, NULL, source_loop_run, (void *)this)) {
            NsprError("capture thread start failed");
            return NSPR_ERROR;
        }
    }
    return NSPR_OK;
}
    
VideoEncoderProc *MediaSponsor::GetSourceProc()
{
    if (!m_veProc && OpenSource((u_char *)"vga")) {
        NsprError("open source error");
        return nullptr;
    }
    return m_veProc;
}

int MediaSponsor::RelayStream(const u_char *addr, const u_char *streamId)
{
    return NSPR_ERROR;
}

int MediaSponsor::ReleaseStream(const u_char *streamId)
{
    return NSPR_ERROR;
}

void MediaSponsor::CloseSource()
{
    if (m_vsProc) {
        m_vsProc->StreamStop();
    }
}
    
void MediaSponsor::LoopRun()
{
    m_vsProc->LoopInput();
}

void MediaSponsor::Exit()
{
    delete m_veProc;
    delete m_vsProc;
    m_veProc = nullptr;
    m_vsProc = nullptr;
    m_capturePid = nullptr;
}

StreamProc *MediaSponsor::SearchProcByDesc(const u_char *desc)
{
    return m_veProc;
}


} // namespace nspr


