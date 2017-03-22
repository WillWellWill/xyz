#include "Nspr.h"

#include "BoxCellTfhd.h"


namespace nspr
{

BoxCellTfhd::BoxCellTfhd()
    : m_moofOffset(0)
    , m_defaultSampleSize(0)
{
}

BoxCellTfhd::~BoxCellTfhd()
{
}

int BoxCellTfhd::Divide(u_char *data, size_t len)
{
    u_char  *p = data;
    const u_char v = *p;
    ++p;
    NsprDebug("Box tfhd start divide");
    if (v == 1) {
        NsprWarn("not support version 1");
        //return NSPR_OK;
    }
    uint32_t flag = 0;
    memcpy((u_char *)&flag + 1, p, 3);
    p += 3;
    NsprDebug("flag: 0x%x", htonl(flag));

    const uint32_t trackId = ntohl(*((uint32_t *)p));
    p += 4;
    NsprDebug("track id %d", trackId);

    p += 4;
    const uint32_t baseDataOffset = ntohl(*((uint32_t *)p));
    p += 4;
    const uint32_t defaultSampleDuration = ntohl(*((uint32_t *)p));
    p += 4;
    const uint32_t defaultSampleSize = ntohl(*((uint32_t *)p));
    p += 4;
    const uint32_t defaultSampleFlags = ntohl(*((uint32_t *)p));
    p += 4;

    NsprDebug("base data offset: %d, default sample duration: %d, default sample size %d, default sample flags:0x%x"
            , baseDataOffset
            , defaultSampleDuration
            , defaultSampleSize
            , defaultSampleFlags);

    NsprDebug("Box tfhd end divide");
	return NSPR_OK;
}


int BoxCellTfhd::Fuse(u_char *data, size_t &len)
{
    u_char  *p = data;
    u_char &v = *p++;
    NsprDebug("Box tfhd start fuse");
    memset(data, 0, len);
    uint32_t *flag = (uint32_t *)p;
    p += 3;

    uint32_t &trackId = *((uint32_t *)p);
    p += 4;

    uint32_t &defaultSampleDuration = *((uint32_t *)p);
    p += 4;
    uint32_t &defaultSampleSize = *((uint32_t *)p);
    p += 4;
    uint32_t &defaultSampleFlags = *((uint32_t *)p);
    p += 4;

    v = 0;
    *flag = ntohl(0x02003800);
    trackId = htonl(1);
    defaultSampleDuration = htonl(40000);
    defaultSampleSize = htonl(m_defaultSampleSize);
    defaultSampleFlags = htonl(0x1010000);
    
    NsprDebug("Box tfhd end fuse, len = %d, m_moofOffset = %d", p - data, m_moofOffset);
    len = p - data;
    return NSPR_OK;
}

bool BoxCellTfhd::isComposite()
{
	return false;
}

} // namespace nspr

