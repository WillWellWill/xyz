#include "Nspr.h"

#include "BoxCellTrex.h"


namespace nspr
{

BoxCellTrex::BoxCellTrex()
{
}

BoxCellTrex::~BoxCellTrex()
{
}

int BoxCellTrex::Divide(u_char *data, size_t len)
{
    u_char  *p = data;
    const u_char v = *p;
    ++p;
    NsprDebug("Box trex start divide");
    if (v == 1) {
        NsprWarn("not support version 1");
        return NSPR_OK;
    }
    uint32_t flag = 0;
    memcpy((u_char *)&flag + 1, p, 3);
    p += 3;
    NsprDebug("flag: 0x%x", htonl(flag));

    uint32_t trackId = ntohl(*((uint32_t *)p));
    p += 4;

    uint32_t defaultSampleDescriptionIndex = ntohl(*((uint32_t *)p));
    p += 4;
    uint32_t defaultSampleDuration = ntohl(*((uint32_t *)p));
    p += 4;
    uint32_t defaultSampleSize = ntohl(*((uint32_t *)p));
    p += 4;
    uint32_t defaultSampleFlags = ntohl(*((uint32_t *)p));
    p += 4;

    NsprDebug("track id = %d, \n"
              "default sample description index = %d, "
              "default sample duration = %d, "
              "default sample size = %d, "
              "defautl sample flags = %d"
              , trackId
              , defaultSampleDescriptionIndex
              , defaultSampleDuration
              , defaultSampleSize
              , defaultSampleFlags);

    NsprDebug("Box trex end divide");
	return NSPR_OK;
}

int BoxCellTrex::Fuse(u_char *data, size_t &len)
{
    memset(data, 0, len);
    u_char *p = data;
    NsprDebug("Box trex start fuse");
    u_char &v = *p++;
    // 0 flag & version
    p += 3;
    v = 0;

    uint32_t &trackId = *((uint32_t *)p);
    p += 4;
    trackId = htonl(1);

    uint32_t &defaultSampleDescriptionIndex = *((uint32_t *)p);
    p += 4;
    defaultSampleDescriptionIndex = htonl(1);

    uint32_t &defaultSampleDuration = *((uint32_t *)p);
    p += 4;
    defaultSampleDuration = 0;

    uint32_t &defaultSampleSize = *((uint32_t *)p);
    p += 4;
    defaultSampleSize = 0;

    uint32_t &defaultSampleFlags = *((uint32_t *)p);
    p += 4;
    defaultSampleFlags = 0;

    NsprDebug("Box trex end fuse, len = %d", p - data);
    len = p - data;
    return NSPR_OK;
}

bool BoxCellTrex::isComposite()
{
	return false;
}

} // namespace nspr

