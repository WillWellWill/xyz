#ifndef __MP4_MEDIA_PACKER_H_INCLUDED__
#define __MP4_MEDIA_PACKER_H_INCLUDED__

#include "VideoCodec.h"
#include "StreamProc.h"

struct AVFormatContext;
struct AVStream;
namespace nspr
{

class Mp4Packer
{
public:
    Mp4Packer(StreamProc *proc, VideoCodec::Parameter *parameter);
    virtual ~Mp4Packer();
    
    int StartUp();

    int DataIn(uint8_t *data, size_t len);
    int PackBegin();
    int PackEnd();

private:
    AVFormatContext *m_formatCtx;
    AVStream *m_vStream;
    uint8_t *m_outputBuffer;
    const static int m_maxOutputBufferSize = 4096;
    StreamProc *m_proc;

    int m_frameCnt;

    VideoCodec::Parameter *m_parameter;
private:
    int AddStream();
    int RemoveStream();

}; // class Mp4Packer

class Mp4PackerProc : public StreamProc
{
public:
    Mp4PackerProc(VideoCodec::Parameter *parameter);
    virtual ~Mp4PackerProc();

    virtual int OnHandleTx(u_char *data, size_t len);
    virtual int OnHandleRx(u_char *data, size_t len);

    virtual int AttachedTxBy(StreamProc *proc);

    int WriteBuffer(uint8_t *buf, int bufSize);
    int SeekBuffer(int offset, int whence);

private:
    Mp4Packer *m_mp;
    u_char *m_buffer;
    size_t m_bufferLen;
    int m_bufferPointer;
    bool m_hasStarted;
    static const int m_maxBufferKBLen = 1024;

}; // class Mp4PackerProc

} // namespace nspr


#endif // ifndef __MP4_MEDIA_PACKER_H_INCLUDED__

