#include "Nspr.h"

#include "VideoEncoder.h"
extern "C"
{
#include "libavutil/opt.h"
#include "libavutil/channel_layout.h"
#include "libavutil/common.h"
#include "libavutil/imgutils.h"
#include "libavutil/mathematics.h"
#include "libavutil/samplefmt.h"
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libswscale/swscale.h"
};

namespace nspr
{

VideoEncoder::VideoEncoder()
    : m_avCodecCtx(nullptr)
    , m_frame(nullptr)
    , m_avpkt(new AVPacket)
    , m_count(0)
{

}

VideoEncoder::~VideoEncoder()
{
    if (m_avCodecCtx) {
        avcodec_close(m_avCodecCtx);
        av_free(m_avCodecCtx);
    }
    if (m_frame) {
        av_freep(&m_frame->data[0]);
        av_frame_free(&m_frame);
    }
    delete m_avpkt;
}

int VideoEncoder::StartUp(VideoCodec::Parameter *parameter)
{
    av_register_all();

    AVCodec *codec = nullptr;
    if (parameter->GetCodecId() == VideoCodec::Parameter::H264) {
        NsprDebug("find 264 encoder, AV_CODEC_ID_H264 = %d", AV_CODEC_ID_H264);
        codec = avcodec_find_encoder(AV_CODEC_ID_H264);
    }
    else if (parameter->GetCodecId() == VideoCodec::Parameter::H265) {
        NsprDebug("find 265 encoder");
        codec = avcodec_find_encoder(AV_CODEC_ID_H265);
    }
    else {
        NsprError("code id[:%d] was unsupported", parameter->GetCodecId());
        return NSPR_ERROR;
    }
    if (!codec) {
        NsprError("find encoder failed");
        return NSPR_ERROR;
    }

    m_avCodecCtx = avcodec_alloc_context3(codec);
    if (!m_avCodecCtx) {
        NsprError("alloc context failed");
        return NSPR_ERROR;
    }
    AVCodecContext *c = m_avCodecCtx;
    c->bit_rate = parameter->GetBitrate();
    c->width = parameter->GetWidth();
    c->height = parameter->GetHeight();
    c->time_base = (AVRational){ 1, parameter->GetFramerate()};
    c->gop_size = parameter->GetFramerate() * parameter->GetGopSeconds();
    c->max_b_frames = 0;
    if (parameter->GetSourceFmt() == VideoCodec::Parameter::YUV420P) {
        c->pix_fmt = AV_PIX_FMT_YUV420P;
    }
    else {
        NsprError("encoder source format was unsupported");
        return NSPR_ERROR;
    }
    if (parameter->GetCodecId() == VideoCodec::Parameter::H264) {
        // av_opt_set(c->priv_data, "preset", "slow", 0);
        av_opt_set(c->priv_data, "preset", "superfast", 0);
        av_opt_set(c->priv_data, "tune", "zerolatency", 0);
    }
    NsprInfo("parameter: width: %d, height: %d, "
            "bitrate: %lld, framerate: %d, "
            "pix_fmt = %d, %d"
            , c->width, c->height
            , c->bit_rate, c->time_base.den, c->pix_fmt, codec->pix_fmts[0]);

    int ret;
	if ((ret = avcodec_open2(c, codec, nullptr)) < 0) {
        NsprError("codec open failed, %d", ret);
        return NSPR_ERROR;
    }

    m_frame = av_frame_alloc();
    if (!m_frame) {
        NsprError("frame alloc failed");
        return NSPR_ERROR;
    }
    m_frame->format = c->pix_fmt;
    m_frame->width  = c->width;
    m_frame->height = c->height;

    if (av_image_alloc(m_frame->data, m_frame->linesize, c->width, c->height, c->pix_fmt, 32) < 0) {
        NsprError("image alloc failed");
        return NSPR_ERROR;
    }
    NsprInfo("video encoder start up ok");

    return NSPR_OK;
}

int VideoEncoder::SendFrame(uint8_t *data)
{
    memcpy(m_frame->data[0], data, m_avCodecCtx->width * m_avCodecCtx->height * 3 / 2);
    m_frame->pts = m_count;
    m_count++;
    if (avcodec_send_frame(m_avCodecCtx, m_frame) < 0) {
        return NSPR_ERROR;
    }
    return NSPR_OK;
}

int VideoEncoder::RecvPacket(uint8_t **pkt, size_t *pktLen)
{
    av_init_packet(m_avpkt);
    if (avcodec_receive_packet(m_avCodecCtx, m_avpkt)) {
        return NSPR_ERROR;
    }
    *pkt = m_avpkt->data;
    *pktLen = m_avpkt->size;
	//NsprDebug("pkt data = %p", m_avpkt->data);
    /*
    NsprDebug("pts: %lld dts: %lld, duration = %d, flag = %d, pos = %lld"
            , m_avpkt->pts, m_avpkt->dts, m_avpkt->duration, m_avpkt->flags, m_avpkt->pos);
    */
    return NSPR_OK;
}

void VideoEncoder::ConfirmRecv()
{
    av_packet_unref(m_avpkt);
}

VideoEncoderProc::VideoEncoderProc(VideoCodec::Parameter *parameter)
    : m_ve(new VideoEncoder())
    , m_param(parameter)
{

}

VideoEncoderProc::~VideoEncoderProc()
{
    delete m_ve;
}

int VideoEncoderProc::OnHandleRx(u_char *data, size_t len)
{
    return NSPR_ERROR;
}

int VideoEncoderProc::OnHandleTx(u_char *data, size_t len)
{
    if (m_ve->SendFrame(data)) {
        NsprError("send frame failed");
        return NSPR_ERROR;
    }

    int ret = 0;
    uint8_t *pkt = nullptr;
    size_t pktLen = 0;
    while ((ret = m_ve->RecvPacket(&pkt, &pktLen)) == NSPR_OK) {
        if (RelayTx(pkt, pktLen)) {
            return NSPR_ERROR;
        }
		m_ve->ConfirmRecv();
    }
    return NSPR_OK;
}

int VideoEncoderProc::AttachedTxBy(StreamProc *proc)
{
    return m_ve->StartUp(m_param); 
}

} // namespace nspr

