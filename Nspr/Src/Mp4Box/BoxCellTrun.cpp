#include "Nspr.h"

#include "BoxCellTrun.h"


namespace nspr
{

BoxCellTrun::BoxCellTrun()
    :  m_dataOffset(0)
    , m_sampleCount(0)
    , m_dataOffsetPtr(nullptr)
{
    int i = 0;
    for (; i < m_maxSampleCount; i++) {
        m_sampleLen[i] = 0;
    }
}

BoxCellTrun::~BoxCellTrun()
{
}

int BoxCellTrun::Divide(u_char *data, size_t len)
{
    u_char  *p = data;
    const u_char v = *p;
    ++p;
    NsprDebug("Box trun start divide");
    if (v == 1) {
        NsprWarn("not support version 1");
        return NSPR_OK;
    }
    uint32_t flag = 0;
    memcpy((u_char *)&flag + 1, p, 3);
    p += 3;
    NsprDebug("flag: 0x%x", ntohl(flag));

    const uint32_t sampleCount = ntohl(*((uint32_t *)p));
    p += 4;
    NsprDebug("sample count %d", sampleCount);

    const uint32_t dataOffset = ntohl(*((uint32_t *)p));
    p += 4;
    NsprDebug("data offset: %d", dataOffset);

    const uint32_t firstSampleFlags = ntohl(*((uint32_t *)p));
    p += 4;
    NsprDebug("first sample flags 0x%x", firstSampleFlags);

    int i = 0;
    int totalSize = 0;
    for (; i < (int)sampleCount; i++) {
        const uint32_t sampleSize = ntohl(*((uint32_t *)p));
        p += 4;
        NsprDebug("sample %d, len: %d", i, sampleSize);
        totalSize += sampleSize;
    }

    NsprDebug("total size = %d", totalSize);

    NsprDebug("Box trun end divide");
	return NSPR_OK;
}

int BoxCellTrun::Fuse(u_char *data, size_t &len)
{
    u_char  *p = data;
    u_char &v = *p++;
    NsprDebug("Box trun start fuse");
    memset(data, 0, len);
    uint32_t *flag = (uint32_t *)p;
    p += 3;

    uint32_t &sampleCount = *((uint32_t *)p);
    p += 4;
    uint32_t &dataOffset = *((uint32_t *)p);
    m_dataOffsetPtr = (uint32_t *)p;
    p += 4;
    uint32_t &firstSampleFlags = *((uint32_t *)p);
    p += 4;
    int i = 0;
    for (; i < m_sampleCount; i++) {
        uint32_t &sampleLen = *((uint32_t *)p);
        p += 4;
        sampleLen = ntohl(m_sampleLen[i]);
    }

    v = 0;
    flag[0] = htonl(0x020500);
    sampleCount = ntohl(m_sampleCount);
    dataOffset = ntohl(m_dataOffset);
    firstSampleFlags = htonl(0x02000000);
    
    NsprDebug("Box trun end fuse, len = %d", p - data);
    len = p - data;
    return NSPR_OK;
}


bool BoxCellTrun::isComposite()
{
	return false;
}

} // namespace nspr

