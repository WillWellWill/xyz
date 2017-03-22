#ifndef __NSPR_VIDEO_ENCODER_H__
#define __NSPR_VIDEO_ENCODER_H__

#include "VideoCodec.h"
#include "StreamProc.h"

struct AVCodecContext;
struct AVFrame;
struct AVPacket;

namespace nspr
{

class VideoEncoder : VideoCodec
{
public:
    VideoEncoder();
    virtual ~VideoEncoder();
    
    int StartUp(VideoCodec::Parameter *parameter);
    int SendFrame(uint8_t *data);
    int RecvPacket(uint8_t **pkt, size_t *pktLen);
    void ConfirmRecv();
    
private:
    AVCodecContext *m_avCodecCtx;
    AVFrame *m_frame;
    AVPacket *m_avpkt;
    int m_count;
    

}; // class VideoEncoder

class VideoEncoderProc : public StreamProc
{
public:
    VideoEncoderProc(VideoCodec::Parameter *parameter);
    ~VideoEncoderProc();

    virtual int OnHandleTx(u_char *data, size_t len);
    virtual int OnHandleRx(u_char *data, size_t len);

    virtual int AttachedTxBy(StreamProc *proc);

    VideoCodec::Parameter *GetParameter() { return m_param; }

private:
    VideoEncoder *m_ve;
    VideoCodec::Parameter *m_param;
}; // class VideoEncoderProc

} // namespace nspr


#endif // ifndef __NSPR_VIDEO_ENCODER_H__

