#include "Nspr.h"

#include "BoxCellTfra.h"


namespace nspr
{

BoxCellTfra::BoxCellTfra()
{
}

BoxCellTfra::~BoxCellTfra()
{
}

int BoxCellTfra::Divide(u_char *data, size_t len)
{
    u_char  *p = data;
    const u_char v = *p;
    ++p;
    NsprDebug("Box tfra start divide");
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

    // reserved
    p += 3;

    const u_char length = *p++;
    u_char lengthSizeOfTrafNum = (length >> 4) & 0x3;
    u_char lengthSizeOfTrunNum = (length >> 2) & 0x3;
    u_char lengthSizeOfSampleNum = length & 0x3;
    
    NsprDebug("length size of traf & trun & sample number is %d %d %d"
                , lengthSizeOfTrafNum
                , lengthSizeOfTrunNum
                , lengthSizeOfSampleNum);
    
    uint32_t entryCount = ntohl(*((uint32_t *)p));
    p += 4;

    NsprDebug("entry count = %d", entryCount);
    int i = 0;
    for (; i < (int)entryCount; i++) {
        p += 4;
        const uint32_t time = ntohl(*((uint32_t *)p));
        p += 4;
        p += 4;
        const uint32_t moofOffset = ntohl(*((uint32_t *)p));
        p += 4;
        NsprDebug("time = %u, moof offset = %u"
                , time, moofOffset);
        NsprDebug("p - data = %d, len = %d", p - data, len);
        NsprDebug("p[0] = %d, p[1] = %d, p[2] = %d"
                , p[0], p[1], p[2]);
        p += 3;
    }

    NsprDebug("Box tfra end divide");
	return NSPR_OK;
}

bool BoxCellTfra::isComposite()
{
	return false;
}

} // namespace nspr

