#ifndef __NSPR_MP4_BOX_PACKER_PROC_H_INCLUDED__
#define __NSPR_MP4_BOX_PACKER_PROC_H_INCLUDED__

#include "VideoCodec.h"
#include "StreamProc.h"

namespace nspr
{

class BoxPacker;
class Mp4BoxPackerProc : public StreamProc
{
public:
	Mp4BoxPackerProc(VideoCodec::Parameter *parameter);
	~Mp4BoxPackerProc();

    void Reset();

	virtual int OnHandleTx(u_char *data, size_t len);
	virtual int OnHandleRx(u_char *data, size_t len);
	virtual int AttachedTxBy(StreamProc *proc);

private:
    BoxPacker *m_boxPacker;
    const int m_maxBoxLen;
    bool m_hasInjectFrame;
    u_char *m_box;
    size_t m_boxLen;

private:
    bool IsPackingTime(u_char *data, size_t len);
}; // class Mp4BoxPackerProc

} // namespace nspr

#endif // ifndef __NSPR_MP4_BOX_PACKER_PROC_H_INCLUDED__

