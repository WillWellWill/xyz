#include "Nspr.h"

#include "VideoCodec.h"

namespace nspr
{

struct
{
    int width;
    int height;
    int bitrate;
} parameterTable[] = {{352, 288, 256000},
                      {640, 480, 512000},
                      {1280, 720, 1024000}};
int VideoCodec::Parameter::GetBitrate()
{
    return parameterTable[(int)m_res].bitrate;
}

int VideoCodec::Parameter::GetWidth()
{
    return parameterTable[(int)m_res].width;
}

int VideoCodec::Parameter::GetHeight()
{
    return parameterTable[(int)m_res].height;
}
}

