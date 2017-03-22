#include "Nspr.h"

#include "BoxCellTfdt.h"


namespace nspr
{

BoxCellTfdt::BoxCellTfdt()
    : m_decodeTime(0)
{
}

BoxCellTfdt::~BoxCellTfdt()
{
}

int BoxCellTfdt::Divide(u_char *data, size_t len)
{
    u_char  *p = data;
    const u_char v = *p;
    ++p;
    NsprDebug("Box tfdt start divide");
    if (v == 1) {
        NsprWarn("version is 1");
        //return NSPR_OK;
    }
    uint32_t flag = 0;
    memcpy((u_char *)&flag + 1, p, 3);
    p += 3;
    NsprDebug("flag: 0x%x", htonl(flag));

    if (v == 1) {
        // decode time occpy 8 bytes
        p += 4;
    }
    const uint32_t decodeTime = ntohl(*((uint32_t *)p));
    p += 4;
    NsprDebug("decode time: %d", decodeTime);

    NsprDebug("Box tfdt end divide");
	return NSPR_OK;
}

int BoxCellTfdt::Fuse(u_char *data, size_t &len)
{
    u_char  *p = data;
    u_char &v = *p++;
    NsprDebug("Box tfdt start fuse");
    memset(data, 0, len);
    uint32_t *flag = (uint32_t *)p;
    p += 3;

    p += 4;
    uint32_t &decodeTime = *((uint32_t *)p);
    p += 4;

    v = 1;
    flag[0] = 0x0;
    decodeTime = htonl(m_decodeTime);
    
    NsprDebug("Box tfdt end fuse, len = %d", p - data);
    len = p - data;
    return NSPR_OK;
}

bool BoxCellTfdt::isComposite()
{
	return false;
}

} // namespace nspr

