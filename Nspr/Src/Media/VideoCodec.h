#ifndef __NSPR_VIDEO_CODEC_H__
#define __NSPR_VIDEO_CODEC_H__

namespace nspr
{

class VideoCodec
{
public:
    VideoCodec() {}
    virtual ~VideoCodec() {}
    
class Parameter
{
public:
    Parameter() : m_codecId(H264), m_res(RES_1280X720), m_srcFmt(YUV420P) {}
    ~Parameter() {}

    enum CodecId
    {
        H264 = 0,
        H265,
    };
    void SetCodecId(CodecId id) { m_codecId = id; }
    CodecId GetCodecId() { return m_codecId; }

    enum Resolution
    {
        RES_352X288 = 0,
        RES_640X480,
        RES_1280X720,
    };

    void SetResolution(Resolution res) { m_res = res; }
    Resolution GetResolution() { return m_res; }

    int GetBitrate();
    int GetWidth();
    int GetHeight();
    int GetFramerate() { return 25; }
    int GetGopSeconds() { return 1; }

    enum MediaFmt
    {
        YUV420P = 0,
        ARGB,
    };
    void SetSourceFmt(MediaFmt fmt) { m_srcFmt = fmt; }
    MediaFmt GetSourceFmt() { return m_srcFmt; }

private:
    CodecId m_codecId;
    Resolution m_res;
    MediaFmt m_srcFmt;
}; // class Parameter

}; // class VideoCodec

} // namespace nspr


#endif // ifndef __NSPR_VIDEO_CODEC_H__

