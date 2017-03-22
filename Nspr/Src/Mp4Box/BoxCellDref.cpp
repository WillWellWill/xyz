#include "Nspr.h"

#include "BoxCellDref.h"


namespace nspr
{

BoxCellDref::BoxCellDref()
{
}

BoxCellDref::~BoxCellDref()
{
}

int BoxCellDref::Divide(u_char *data, size_t len)
{
    u_char  *p = data;
    const u_char v = *p;
    ++p;
    NsprDebug("Box dref start divide");
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
    NsprDebug("entry count is %d", entryCount);

    int i = 0;
    for (; i < (int)entryCount; i++) {
        const uint32_t boxLen = ntohl(*((uint32_t *)p));
        (void)boxLen;
        p += 4;

        char boxName[5] = {0};
        memcpy(boxName, p, 4);
        p += 4;

        const u_char boxV = *p;
        ++p;
        if (boxV == 1) {
            NsprWarn("not support version 1");
            return NSPR_OK;
        }
        uint32_t f = 0;
        memcpy((u_char *)&f + 1, p, 3);
        p += 3;
        f = htonl(f);
        if (f == 1) {
            NsprDebug("entry %s is empty", boxName);
        }
    }
    
    NsprDebug("Box dref start divide");
	return NSPR_OK;
}

int BoxCellDref::Fuse(u_char *data, size_t &len)
{
    u_char *p = data;

    u_char data0[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01};
    u_char data1[] = {0x00, 0x00, 0x00, 0x0c, 0x75, 0x72, 0x6c, 0x20, 0x00, 0x00, 0x00, 0x01};

    memcpy(p, data0, 8);
    p += 8;

    memcpy(p, data1, 12);
    p += 12;

    len = p - data;
    return NSPR_OK;
}

bool BoxCellDref::isComposite()
{
	return false;
}

} // namespace nspr

