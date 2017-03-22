#include "Nspr.h"

#include "BoxCellStsz.h"


namespace nspr
{

BoxCellStsz::BoxCellStsz()
{
}

BoxCellStsz::~BoxCellStsz()
{
}

int BoxCellStsz::Divide(u_char *data, size_t len)
{
    u_char  *p = data;
    const u_char v = *p;
    ++p;
    NsprDebug("Box stsz start divide");
    if (v == 1) {
        NsprWarn("not support version 1");
        return NSPR_OK;
    }
    uint32_t flag = 0;
    memcpy((u_char *)&flag + 1, p, 3);
    p += 3;
    NsprDebug("flag: 0x%x", htonl(flag));

    uint32_t sampleSize = ntohl(*((uint32_t *)p));
    p += 4;

    uint32_t sampleCount = ntohl(*((uint32_t *)p));
    p += 4;

    NsprDebug("sample size = %d, sample count = %d", sampleSize, sampleCount);
	if (sampleSize == 0){
        int i;
		for (i = 1; i <= (int)sampleCount; i++) {
            const uint32_t entrySize = ntohl(*((uint32_t *)p));
            p += 4;
            NsprDebug("entry size = %d", entrySize);
		}
	}
    NsprDebug("Box stsz end divide");
	return NSPR_OK;
}

int BoxCellStsz::Fuse(u_char *data, size_t &len)
{
    memset(data, 0, len);
    u_char *p = data;
    NsprDebug("Box stsz start fuse");

    p += 4;

    // sample size 0
    p += 4;

    // 0 entries
    p += 4;
    NsprDebug("Box stsz end fuse, len = %d", p - data);
    len = p - data;
    return NSPR_OK;
}

bool BoxCellStsz::isComposite()
{
	return false;
}

} // namespace nspr

