#include "Nspr.h"

#include "BoxCellVmhd.h"


namespace nspr
{

BoxCellVmhd::BoxCellVmhd()
{
}

BoxCellVmhd::~BoxCellVmhd()
{
}

int BoxCellVmhd::Divide(u_char *data, size_t len)
{
    u_char  *p = data;
    const u_char v = *p;
    ++p;
    NsprDebug("Box vmhd start divide");
    if (v == 1) {
        NsprWarn("not support version 1");
        return NSPR_OK;
    }
    uint32_t flag = 0;
    memcpy((u_char *)&flag + 1, p, 3);
    p += 3;
    NsprDebug("flag: 0x%x", htonl(flag));

    const uint16_t graphicsMode = ntohs(*((uint16_t *)p));
    p += 2;
    NsprDebug("graphics mode is %d", graphicsMode);
    
    const uint16_t red = ntohs(*((uint16_t *)p));
    p += 2;
    const uint16_t green = ntohs(*((uint16_t *)p));
    p += 2;
    const uint16_t blue = ntohs(*((uint16_t *)p));
    p += 2;
    NsprDebug("rgb: %d, %d, %d", red, green, blue);

    assert(p - data == (int)len);
    NsprDebug("Box vmhd end divide");
	return NSPR_OK;
}

int BoxCellVmhd::Fuse(u_char *data, size_t &len)
{
    memset(data, 0, len);
    u_char *p = data;
    NsprDebug("Box vmhd start fuse");

    p[3] = 0x1;
    p += 4;

    // graphic
    p += 8;

    NsprDebug("Box vmhd end fuse, len = %d", p - data);
    len = p - data;
    return NSPR_OK;
}

bool BoxCellVmhd::isComposite()
{
	return false;
}

} // namespace nspr

