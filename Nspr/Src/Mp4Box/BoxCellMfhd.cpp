#include "Nspr.h"

#include "BoxCellMfhd.h"


namespace nspr
{

BoxCellMfhd::BoxCellMfhd()
    : m_sequenceNumber(1)
{
}

BoxCellMfhd::~BoxCellMfhd()
{
}

int BoxCellMfhd::Divide(u_char *data, size_t len)
{
    u_char  *p = data;
    const u_char v = *p;
    ++p;
    NsprDebug("Box mfhd start divide");
    if (v == 1) {
        NsprWarn("not support version 1");
        return NSPR_OK;
    }
    uint32_t flag = 0;
    memcpy((u_char *)&flag + 1, p, 3);
    p += 3;
    NsprDebug("flag: 0x%x", htonl(flag));

    uint32_t sequenceNumber = ntohl(*((uint32_t *)p));
    p += 4;

    NsprDebug("sequence number = %d", sequenceNumber);

    NsprDebug("Box mfhd end divide");
	return NSPR_OK;
}

int BoxCellMfhd::Fuse(u_char *data, size_t &len)
{
    u_char  *p = data;
    u_char &v = *p++;
    NsprDebug("Box mfhd start fuse");
    memset(data, 0, len);
    // let flag be 0
    p += 3;

    uint32_t &sequenceNumber = *((uint32_t *)p);
    p += 4;

    v = 0;
    sequenceNumber = htonl(m_sequenceNumber++);
    
    NsprDebug("Box mfhd end fuse, len = %d", p - data);
    len = p - data;
    return NSPR_OK;
}

bool BoxCellMfhd::isComposite()
{
	return false;
}

} // namespace nspr

