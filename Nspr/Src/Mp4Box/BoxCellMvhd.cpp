#include "Nspr.h"

#include "BoxCellMvhd.h"

#include <ctime>

namespace nspr
{

BoxCellMvhd::BoxCellMvhd()
{
    u_char defaultData[] = {0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x3, 0xe8, 0x0, 0x0, 0x0, 0x0, 0x0, 0x1, 0x0, 0x0, 0x1, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x1, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x1, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x40, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x2};
    memcpy(m_defaultData, defaultData, sizeof(defaultData));
    m_defaultDataLen = sizeof(defaultData);
}

BoxCellMvhd::~BoxCellMvhd()
{
}

int BoxCellMvhd::Divide(u_char *data, size_t len)
{
    u_char  *p = data;
    const u_char v = *p;
    ++p;
    NsprDebug("Box mvhd start divide");
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

    const uint32_t timeScale = ntohl(*((uint32_t *)p));
    p += 4;
    NsprDebug("time scale %d", timeScale);

    const uint32_t duration = ntohl(*((uint32_t *)p));
    p += 4;
    NsprDebug("duration %d", duration);

    const uint16_t rateHigh = ntohs(*((uint16_t *)p));
    p += 2;
    const uint16_t rateLow = ntohs(*((uint16_t *)p));
    p += 2;
    NsprDebug("rate %d.%d", rateHigh, rateLow);

    const uint16_t volHigh = *p;
    p++;
    const uint16_t volLow = *p;
    p++;
    NsprDebug("volume %d.%d", volHigh, volLow);

    p += 70;

    const uint32_t nextTrackId = ntohl(*((uint32_t *)p));
    p += 4;
    NsprDebug("next track id %d", nextTrackId);

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

    assert(p - data == (int)len);
    NsprDebug("Box mvhd end divide");
    
	return NSPR_OK;
}

int BoxCellMvhd::Fuse(u_char *data, size_t &len)
{
    u_char *p = data;
    NsprDebug("Box mvhd start fuse");

    memcpy(p, m_defaultData, m_defaultDataLen);
    p += m_defaultDataLen;
    
    NsprDebug("Box mvhd end fuse, len = %d", p - data);
    len = p - data;
    return NSPR_OK;
}

bool BoxCellMvhd::isComposite()
{
    return false;
}

} // namespace nspr

