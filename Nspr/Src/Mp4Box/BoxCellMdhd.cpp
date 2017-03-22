#include "Nspr.h"

#include "BoxCellMdhd.h"


namespace nspr
{

BoxCellMdhd::BoxCellMdhd()
{
    u_char defaultData[] = {0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x12, 0x4f, 0x80, 0x0, 0x0, 0x0, 0x0, 0x55, 0xc4, 0x0, 0x0};
    memcpy(m_defaultData, defaultData, sizeof(defaultData));
    m_defaultDataLen = sizeof(defaultData);
}

BoxCellMdhd::~BoxCellMdhd()
{
}

int BoxCellMdhd::Divide(u_char *data, size_t len)
{
    u_char  *p = data;
    const u_char v = *p;
    ++p;
    NsprDebug("Box mdhd start divide");
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

    const uint16_t language = ntohs(*((uint16_t *)p));
    p += 2;
    NsprDebug("language 0x%x", language);

    // pre-defined
    p += 2;

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
    NsprDebug("Box mdhd end divide");
	return NSPR_OK;
}

int BoxCellMdhd::Fuse(u_char *data, size_t &len)
{
    u_char *p = data;
    NsprDebug("Box mdhd start fuse");

    memcpy(p, m_defaultData, m_defaultDataLen);
    p += m_defaultDataLen;
    
    NsprDebug("Box mdhd end fuse, len = %d", p - data);
    len = p - data;
    return NSPR_OK;
}

bool BoxCellMdhd::isComposite()
{
	return false;
}

} // namespace nspr

