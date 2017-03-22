#include "Nspr.h"

#include "BoxCellStsc.h"


namespace nspr
{

BoxCellStsc::BoxCellStsc()
{
}

BoxCellStsc::~BoxCellStsc()
{
}

int BoxCellStsc::Divide(u_char *data, size_t len)
{
    u_char  *p = data;
    const u_char v = *p;
    ++p;
    NsprDebug("Box stsc start divide");
    if (v == 1) {
        NsprWarn("not support version 1");
        return NSPR_OK;
    }
    uint32_t flag = 0;
    memcpy((u_char *)&flag + 1, p, 3);
    p += 3;
    NsprDebug("flag: 0x%x", htonl(flag));

    uint32_t entryCount = ntohl(*((uint32_t *)p));
    p += 4;

    NsprDebug("entry count = %d", entryCount);
    int i = 0;
    for (; i < (int)entryCount; i++) {
        const uint32_t firstChunk = ntohl(*((uint32_t *)p));
        p += 4;
        const uint32_t samplesPerChunk = ntohl(*((uint32_t *)p));
        p += 4;
        const uint32_t sampleDescriptionIndex = ntohl(*((uint32_t *)p));
        p += 4;
        NsprDebug("first chunk = %d, sample per chunk = %d, sample description index = %d"
                , firstChunk, samplesPerChunk, sampleDescriptionIndex);
    }
    NsprDebug("Box stsc end divide");
	return NSPR_OK;
}

int BoxCellStsc::Fuse(u_char *data, size_t &len)
{
    memset(data, 0, len);
    u_char *p = data;
    NsprDebug("Box stsc start fuse");

    p += 4;

    // 0 entries
    p += 4;
    NsprDebug("Box stsc end fuse, len = %d", p - data);
    len = p - data;
    return NSPR_OK;
}

bool BoxCellStsc::isComposite()
{
	return false;
}

} // namespace nspr

