#ifndef __NSPR_MEDIA_SPONSOR_H_INCLUDED__
#define __NSPR_MEDIA_SPONSOR_H_INCLUDED__

namespace nspr
{

class VideoSourceProc;
class VideoEncoderProc;
class StreamProc;
class MediaSponsor
{
public:
    MediaSponsor();
    ~MediaSponsor();

    int OpenSource(const u_char *desc);
    int RelayStream(const u_char *addr, const u_char *streamId);
    int ReleaseStream(const u_char *streamId);

    VideoEncoderProc *GetSourceProc();

    void LoopRun();
    void CloseSource();
    void Exit();

    StreamProc *SearchProcByDesc(const u_char *desc);

private:
    pthread_t           m_capturePid;
    VideoSourceProc    *m_vsProc;
    VideoEncoderProc   *m_veProc;

}; // class MediaSponsor

void *source_loop_run(void *param);

} // namespace nspr


#endif // ifndef __NSPR_MEDIA_SPONSOR_H_INCLUDED__

