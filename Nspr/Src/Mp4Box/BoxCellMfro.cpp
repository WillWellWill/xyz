#include "Nspr.h"

#include "BoxCellMfro.h"


namespace nspr
{

BoxCellMfro::BoxCellMfro()
{
}

BoxCellMfro::~BoxCellMfro()
{
}

int BoxCellMfro::Divide(u_char *data, size_t len)
{
    u_char  *p = data;
    const u_char v = *p;
    ++p;
    NsprDebug("Box mfro start divide");
    if (v == 1) {
        NsprWarn("not support version 1");
        return NSPR_OK;
    }
    uint32_t flag = 0;
    memcpy((u_char *)&flag + 1, p, 3);
    p += 3;
    NsprDebug("flag: 0x%x", htonl(flag));

    const uint32_t size_ = ntohl(*((uint32_t *)p));
    p += 4;
    NsprDebug("size_ = %d", size_);

    NsprDebug("Box mfro end divide");
	return NSPR_OK;
}

bool BoxCellMfro::isComposite()
{
	return false;
}

} // namespace nspr

