#include "Nspr.h"

#include "VideoSource.h"
#include "opencv2/opencv.hpp"
#include "StreamProc.h"

using namespace cv;

namespace nspr
{

VideoSource::VideoSource(StreamProc *proc)
    : m_capture(new VideoCapture(0))
    , m_parameter(new VideoCodec::Parameter())
    , m_proc(proc)
    , m_isStopped(false)
{

}

VideoSource::~VideoSource()
{
    delete m_parameter;
    delete m_capture;
}

int VideoSource::StartUp(Resolution resolution)
{
    if(!m_capture->isOpened()) {
        return NSPR_ERROR;
    }
    NsprInfo("open capture successfully");
    
    double capture_width = 640.0, capture_height = 480.0;
    switch (resolution)
    {
        case RESOLUTION_VGA:  capture_width = 640.0;  capture_height = 480.0; break;
        case RESOLUTION_720P: capture_width = 1280.0; capture_height = 720.0; break;
        default: break;
    }
    m_capture->set(CV_CAP_PROP_FRAME_WIDTH, capture_width);
    m_capture->set(CV_CAP_PROP_FRAME_HEIGHT, capture_height);
    NsprInfo("set capture parameter[H264, YUV420P] successfully");
    
    m_parameter->SetCodecId(VideoCodec::Parameter::H264);
    m_parameter->SetSourceFmt(VideoCodec::Parameter::YUV420P);
    int w = m_capture->get(CV_CAP_PROP_FRAME_WIDTH);  
    int h = m_capture->get(CV_CAP_PROP_FRAME_HEIGHT);
    int cc = static_cast<int>(m_capture->get(CV_CAP_PROP_FOURCC));
    int fps = static_cast<int>(m_capture->get(CV_CAP_PROP_FPS));
    int speed = static_cast<int>(m_capture->get(CV_CAP_PROP_ISO_SPEED));
    int format = static_cast<int>(m_capture->get(CV_CAP_PROP_FORMAT));
    int bri = static_cast<int>(m_capture->get(CV_CAP_PROP_BRIGHTNESS));
    
    NsprInfo("FOURCC : %d, fps = %d, speed = %d, format = %d, bri = %d"
            , cc, fps, speed, format, bri);
    NsprInfo("capture get width = %d, height = %d", w, h);  
    if (w == 1280 && h == 720) {
        m_parameter->SetResolution(VideoCodec::Parameter::RES_1280X720);
    }
    else if (w == 640 && h == 480) {
        m_parameter->SetResolution(VideoCodec::Parameter::RES_640X480);
    }
    else {
        NsprError("the resolution was not expected");
        return NSPR_ERROR;
    }
    NsprInfo("capture start up ok");
    return NSPR_OK;
}

int VideoSource::Stop()
{
    m_isStopped = true;
    return NSPR_OK;
}

void VideoSource::Run()
{
    for (;;) {
        Mat frame;
        *m_capture >> frame;
        if (!frame.data) {
            NsprError("empty frame");
            usleep(10000);
            continue;
        }
        cv::Mat yuvImg;
        const int w = m_parameter->GetWidth()
            , h = m_parameter->GetHeight();
        yuvImg.create(h * 3 / 2, w, CV_8UC1);
        cv::cvtColor(frame, yuvImg, CV_BGR2YUV_I420);
        if (m_isStopped) {
            NsprError("stopped, jump out of the loop");
            break;
        }
        if (m_proc->RelayTx(yuvImg.data, w * h * 3 / 2)) {
            NsprError("process error");
            break;
        }
        waitKey(10);
    }
}


VideoSourceProc::VideoSourceProc(VideoSource::Resolution resolution)
    : m_vs(new VideoSource(this))
    , m_resolution(resolution)
{

}
VideoSourceProc::~VideoSourceProc()
{
    delete m_vs;
}

int VideoSourceProc::OnHandleTx(u_char *data, size_t len)
{
    return NSPR_ERROR;
}

int VideoSourceProc::OnHandleRx(u_char *data, size_t len)
{
    return NSPR_ERROR;
}

int VideoSourceProc::StreamStart()
{
    return m_vs->StartUp(m_resolution);
}

int VideoSourceProc::StreamStop()
{
    return m_vs->Stop();
}
    
void VideoSourceProc::LoopInput()
{
    m_vs->Run();
}

} // namespace nspr

