#include "Nspr.h"

#include "BoxCellTkhd.h"


namespace nspr
{

BoxCellTkhd::BoxCellTkhd()
    : m_width(640)
    , m_height(480)
{
    u_char defaultData[] = {0x0, 0x0, 0x0, 0x3, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x1, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x1, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x1, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x40, 0x0, 0x0, 0x0};
    memcpy(m_defaultData, defaultData, sizeof(defaultData));
    m_defaultDataLen = sizeof(defaultData);
}

BoxCellTkhd::~BoxCellTkhd()
{
}

int BoxCellTkhd::Divide(u_char *data, size_t len)
{
    u_char  *p = data;
    const u_char v = *p;
    ++p;
    NsprDebug("Box tkhd start divide");
    if (v == 1) {
        NsprWarn("not support version 1");
        return NSPR_OK;
    }

    uint32_t flag = 0;
    memcpy((u_char *)&flag + 1, p, 3);
    p += 3;
    NsprDebug("flag: 0x%x", htonl(flag));

    uint32_t createTime = ntohl(*((uint32_t *)p));
    p += 4;
    uint32_t modifyTime = ntohl(*((uint32_t *)p));
    p += 4;

    if (createTime) {
        tm *lt = localtime((const time_t *)&createTime);
        NsprDebug("create time %s", asctime(lt));
    }

    if (modifyTime) {
        tm *lt = localtime((const time_t *)&modifyTime);
        NsprDebug("create time %s", asctime(lt));
    }

    const uint32_t trackId = ntohl(*((uint32_t *)p));
    p += 4;
    NsprDebug("track id %d", trackId);

    // reserved
    p += 4;

    const uint32_t duration = ntohl(*((uint32_t *)p));
    p += 4;
    NsprDebug("duration %d", duration);
    
    // reserved
    p += 8;

    const uint16_t layer = ntohs(*((uint16_t *)p));
    p += 2;
    NsprDebug("layer %d", layer);

    const uint16_t alternateGroup = ntohs(*((uint16_t *)p));
    p += 2;
    NsprDebug("alternate group %d", alternateGroup);

    const uint16_t volHigh = *p;
    p++;
    const uint16_t volLow = *p;
    p++;
    NsprDebug("volume %d.%d", volHigh, volLow);

    // reserved
    p += 38;

    const uint16_t widthHigh = ntohs(*((uint16_t *)p));
    p += 2;
    const uint16_t widthLow = ntohs(*((uint16_t *)p));
    p += 2;
    NsprDebug("width %d.%d", widthHigh, widthLow);

    const uint16_t heightHigh = ntohs(*((uint16_t *)p));
    p += 2;
    const uint16_t heightLow = ntohs(*((uint16_t *)p));
    p += 2;
    NsprDebug("height %d.%d", heightHigh, heightLow);

    assert(p - data == (int)len);

    char printDefaultData[1024] = {0};
    snprintf(printDefaultData, sizeof(printDefaultData), "defaultData[] = {");
    int i = 0;
    for (; i < (int)len; i++) {
        snprintf(printDefaultData + strlen(printDefaultData), sizeof(printDefaultData) - strlen(printDefaultData), 
                "0x%x, ", data[i]);
    }
    snprintf(printDefaultData + strlen(printDefaultData) - 1, sizeof(printDefaultData) - strlen(printDefaultData), 
            "};");
    
    NsprDebug("%s", printDefaultData);

    NsprDebug("Box tkhd end divide");
	return NSPR_OK;
}

int BoxCellTkhd::Fuse(u_char *data, size_t &len)
{
    u_char *p = data;
    NsprDebug("Box tkhd start fuse");

    memcpy(p, m_defaultData, m_defaultDataLen);
    p += m_defaultDataLen;

    uint16_t &widthHigh = *((uint16_t *)p);
    p += 2;
    uint16_t &widthLow = *((uint16_t *)p);
    p += 2;
    uint16_t &heightHigh = *((uint16_t *)p);
    p += 2;
    uint16_t &heightLow = *((uint16_t *)p);
    p += 2;

    widthHigh = htons(m_width);
    widthLow = 0;
    heightHigh = htons(m_height);
    heightLow = 0;
    
    NsprDebug("Box tkhd end fuse, len = %d", p - data);
    len = p - data;
    return NSPR_OK;
}

bool BoxCellTkhd::isComposite()
{
	return false;
}

} // namespace nspr

