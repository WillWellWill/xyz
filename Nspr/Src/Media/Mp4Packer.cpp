#include "Nspr.h"

#include "Mp4Packer.h"

extern "C" {
#include <libavutil/opt.h>
#include <libavutil/mathematics.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libswresample/swresample.h>
}

namespace nspr
{

Mp4Packer::Mp4Packer(StreamProc *proc, VideoCodec::Parameter *parameter)
    : m_formatCtx(nullptr)
    , m_vStream(nullptr)
    , m_outputBuffer(nullptr)
    , m_proc(proc)
    , m_frameCnt(0)
    , m_parameter(parameter)
{
    av_register_all();
    m_outputBuffer = (uint8_t *)av_malloc(m_maxOutputBufferSize);
}

Mp4Packer::~Mp4Packer()
{

    if (m_outputBuffer) {
        av_free(m_outputBuffer);
    }
}

static void RemoveStartCode(uint8_t *data, size_t len, uint8_t **out, size_t *outLen)
{
    *out = new uint8_t[len * 2 + 1];
    int zeroContinueCount = 0, naluCount = 0, i = 0, lastIndex = 0;
    
    uint8_t *outPtr = *out;
    uint32_t *lastNaluLen = nullptr;
    for (i = 0; i < len; i++) {
        if (data[i] == 0) {
            *outPtr++ = *(data + i);
            ++zeroContinueCount;
        }
        else if ((data[i] == 1) && (zeroContinueCount == 2 || zeroContinueCount == 3)) {
            if (lastNaluLen) {
                *lastNaluLen = htonl(i - zeroContinueCount - lastIndex - 1);
            }
            outPtr -= zeroContinueCount;

            lastNaluLen = (uint32_t *)outPtr;
            outPtr += 4;
            ++naluCount;
            lastIndex = i;
            continue;
        }
        else {
            *outPtr++ = *(data + i);
            zeroContinueCount = 0;
        }
    }
    *lastNaluLen = htonl(i - lastIndex - 1);
    *outLen = outPtr - *out;
    NsprDebug("nalu count = %d, last nalu size = %d, out len = %d", naluCount, ntohl(*lastNaluLen), *outLen);
}

static int iframe = -1;
int Mp4Packer::DataIn(uint8_t *data, size_t len)
{
    if (m_frameCnt == 0) {
        iframe++;
    }
    char fileName[128];
    snprintf(fileName, sizeof(fileName), "./mp4files/%d.264", iframe);
    FILE *fp = fopen(fileName, "ab+");
    if (!fp) {
        NsprError("open file %s failed, %s", strerror(errno));
        return NSPR_ERROR;
    }
    fwrite(data, 1, len, fp);
    fclose(fp);

	AVPacket pkt;
	av_init_packet(&pkt);
	pkt.stream_index  = m_vStream->index;

    uint8_t *interleavdPkt = nullptr;
    size_t interleavedLen = 0;
    RemoveStartCode(data, len, &interleavdPkt, &interleavedLen);
	pkt.data          = interleavdPkt;
	pkt.size          = interleavedLen;
    pkt.dts = pkt.pts = (m_frameCnt++) * m_vStream->time_base.den / m_vStream->time_base.num / 25;
    if (data[4] == 0x67) {
        pkt.flags |= AV_PKT_FLAG_KEY;
    }
    NsprDebug("pts = %lld", pkt.pts);
	int ret = av_interleaved_write_frame(m_formatCtx, &pkt);
    delete []interleavdPkt;
    return ret;
}

int Mp4Packer::AddStream()
{
    AVCodecID codec_id = AV_CODEC_ID_H264;
    if (m_parameter->GetCodecId() == VideoCodec::Parameter::H265) {
        codec_id = AV_CODEC_ID_H265;
    }
    AVCodec *codec = avcodec_find_encoder(codec_id);
    m_vStream = avformat_new_stream(m_formatCtx, nullptr);
    if (!m_vStream) {
        NsprError("could not allocate stream");
        return NSPR_ERROR;
    }
    m_vStream->id = m_formatCtx->nb_streams - 1;
    AVCodecContext *c = avcodec_alloc_context3(codec);
    
    c->codec_id = codec_id;
    c->bit_rate = m_parameter->GetBitrate();
    c->width    = m_parameter->GetWidth();
    c->height   = m_parameter->GetHeight();

    m_vStream->time_base = (AVRational){1, 1200000};
    c->gop_size = m_parameter->GetFramerate() * m_parameter->GetGopSeconds();
    c->pix_fmt = AV_PIX_FMT_YUV420P;
    if (m_formatCtx->oformat->flags & AVFMT_GLOBALHEADER) {
        NsprError("hello global");
        c->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
    }
    if (avcodec_parameters_from_context(m_vStream->codecpar, c)) {
        return NSPR_ERROR;
    }
    avcodec_free_context(&c);
    m_frameCnt = 0;
    return NSPR_OK;
}

int Mp4Packer::RemoveStream()
{
    if (m_formatCtx) {
        avformat_free_context(m_formatCtx);
    }
    return NSPR_OK;
}

static int WriteMp4Packet(void *opaque, uint8_t *buf, int buf_size)
{
    NsprDebug("buf size = %d", buf_size);
    Mp4PackerProc *mp = (Mp4PackerProc *)opaque;
    mp->WriteBuffer(buf, buf_size);
    return buf_size;
}

static int64_t SeekMp4Buffer(void *opaque, int64_t offset, int whence)
{
    NsprDebug("whence = %d, offset = %lld", whence, offset);
    Mp4PackerProc *mp = (Mp4PackerProc *)opaque;
    mp->SeekBuffer(offset, whence);
    return offset;
}

int Mp4Packer::StartUp()
{
    m_formatCtx = avformat_alloc_context();
    if (!m_formatCtx) {
        NsprError("allocate context failed");
        return NSPR_ERROR;
    }
    m_formatCtx->oformat = av_guess_format("mp4", 0, 0);
    if (!m_formatCtx->oformat) {
        NsprError("guess format of mp4 failed");
        return NSPR_ERROR;
    }
    
    m_formatCtx->pb = avio_alloc_context(m_outputBuffer, m_maxOutputBufferSize, 1, m_proc, NULL, WriteMp4Packet, SeekMp4Buffer);
    if (!m_formatCtx->pb) {
        NsprError("avio_alloc_context failed");
        return NSPR_ERROR;
    }

    AVOutputFormat *fmt = m_formatCtx->oformat;
    if (fmt->video_codec == AV_CODEC_ID_NONE) {
        NsprError("video codec id is AV_CODEC_ID_NONE");
        return NSPR_ERROR;
    }
    if (AddStream()) {
        return NSPR_ERROR;
    }
    
    return NSPR_OK;
}

int Mp4Packer::PackBegin()
{
    if (StartUp()) {
        return NSPR_ERROR;
    }
    AVDictionary *opts = NULL;
    av_dict_set(&opts, "movflags", "frag_keyframe+empty_moov", 0);
    int ret;
    if ((ret = avformat_write_header(m_formatCtx,  &opts))) {
        NsprError("avformat_write_header failed %s", av_err2str(ret));
        return NSPR_ERROR;
    }
    return NSPR_OK;
}

int Mp4Packer::PackEnd()
{
    av_write_trailer(m_formatCtx);
    if (RemoveStream()) {
        return NSPR_ERROR;
    }
    return NSPR_OK;
}

Mp4PackerProc::Mp4PackerProc(VideoCodec::Parameter *parameter)
    : m_mp(new Mp4Packer(this, parameter))
    , m_buffer(new u_char[m_maxBufferKBLen * 1024])
    , m_bufferLen(0)
    , m_bufferPointer(0)
    , m_hasStarted(false)
{
}

Mp4PackerProc::~Mp4PackerProc()
{
    delete []m_buffer;
    delete m_mp;
}

int Mp4PackerProc::OnHandleTx(u_char *data, size_t len)
{
    if (data[4] == 0x67) {
        if (m_hasStarted) {
            NsprDebug("Pack end");
            m_mp->PackEnd();
            RelayTx(m_buffer, m_bufferLen);
            m_bufferPointer = 0;
            m_bufferLen = 0;
        }
        m_mp->PackBegin();
        NsprDebug("Pack begin");
        m_hasStarted = true;
    }
    return m_mp->DataIn(data, len);
}

int Mp4PackerProc::OnHandleRx(u_char *data, size_t len)
{
    return NSPR_ERROR;
}

int Mp4PackerProc::WriteBuffer(uint8_t *buf, int bufSize)
{
    if (m_bufferPointer + bufSize > m_maxBufferKBLen * 1024) {
        NsprError("size of current gop overflow, m_bufferPointer = %d, bufSize = %d", m_bufferPointer, bufSize);
        return NSPR_ERROR;
    }
    if (bufSize == 4) {
        NsprDebug("4 is %x %x %x %x, bufferPointer = %d", buf[0], buf[1], buf[2], buf[3], m_bufferPointer);
    }
    memcpy(m_buffer + m_bufferPointer, buf, bufSize);
    m_bufferPointer += bufSize;
    if (m_bufferPointer > m_bufferLen) {
        m_bufferLen = m_bufferPointer;
    }
    return NSPR_OK;
}

int Mp4PackerProc::SeekBuffer(int offset, int whence)
{
    if (whence == 0) {
        m_bufferPointer = offset;
        NsprDebug("from start offset = %d", offset);
    }
    else if (whence == 1) {
        m_bufferPointer += offset;
        NsprDebug("from current %d offset = %d", m_bufferPointer, offset);
    }
    else if (whence == 2) {
        m_bufferPointer = m_bufferLen + offset;
        NsprDebug("from end %d offset = %d", m_bufferLen, offset);
    }
    else {
        NsprError("error whence");
        return NSPR_ERROR;
    }
    if (offset == m_bufferLen) {
        NsprDebug("seek to end, finish the pack progress");
    }
    return NSPR_OK;
}

int Mp4PackerProc::AttachedTxBy(StreamProc *proc)
{
    return NSPR_OK; 
}


} // namespace nspr

