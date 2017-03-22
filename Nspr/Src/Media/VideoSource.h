#ifndef __NSPR_VIDEO_SOURCE_H__
#define __NSPR_VIDEO_SOURCE_H__

#include "VideoCodec.h"
#include "StreamProc.h"

namespace cv
{
class VideoCapture;
}


namespace nspr
{

class VideoSource
{
public:
    VideoSource(StreamProc *proc);
    ~VideoSource();

    enum Resolution
    {
        RESOLUTION_VGA = 0,
        RESOLUTION_720P,
    };

    int StartUp(Resolution resolution);
    int Stop();

    void Run();

    VideoCodec::Parameter *GetParameter() { return m_parameter; }

private:
    cv::VideoCapture *m_capture;
    VideoCodec::Parameter *m_parameter;
    StreamProc *m_proc;
    bool m_isStopped;

}; // class VideoSource

class VideoSourceProc : public StreamProc
{
public:
    VideoSourceProc(VideoSource::Resolution resolution);
    virtual ~VideoSourceProc();
    
    virtual int OnHandleTx(u_char *data, size_t len);
    virtual int OnHandleRx(u_char *data, size_t len);

    int StreamStart();
    int StreamStop();

    void LoopInput();
    VideoCodec::Parameter *GetParameter() { return m_vs->GetParameter(); }

private:
    VideoSource *m_vs;
    VideoSource::Resolution m_resolution;

}; // class VideoSourceProc

} // namespace nspr


#endif // ifndef __NSPR_VIDEO_SOURCE_H__

