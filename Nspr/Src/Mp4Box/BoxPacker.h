#ifndef __NSPR_BOX_PACKER_H_INCLUDED__
#define __NSPR_BOX_PACKER_H_INCLUDED__

#include "VideoCodec.h"

namespace nspr
{

class BoxFactory;
class BoxPacker
{
public:
	BoxPacker(VideoCodec::Parameter *parameter);
	~BoxPacker();

	int Pack(u_char *data, size_t& len);
    void InjectData(u_char *data, size_t len);
    int InjectFrame(u_char *data, size_t len);
    VideoCodec::Parameter *GetParameter() { return m_param; }

private:
    BoxFactory *m_factory;
    const static int m_maxFrameCount = 1024;
    int m_frameSize[m_maxFrameCount];
    int m_frameCount;

    u_char *m_mediaData;
    const int m_maxMediaDataLen;
    size_t m_mediaLen;

    VideoCodec::Parameter *m_param;

    bool m_prepared;
    bool m_headerPacker;

    int m_totalPackerSize;

    int m_timestamp;
    int m_decodeTime;

private:
    int WriteBox(u_char *type, u_char *data, size_t &len);
    void PreparePacker(u_char *data, size_t len);
    int ProcessH264Binary(uint8_t *data, size_t len);
}; // class BoxPacker

} // namespace nspr

#endif // ifndef __NSPR_BOX_PACKER_H_INCLUDED__

