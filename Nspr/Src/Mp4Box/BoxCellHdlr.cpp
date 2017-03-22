#include "Nspr.h"

#include "BoxCellHdlr.h"


namespace nspr
{

BoxCellHdlr::BoxCellHdlr()
{
    u_char defaultData[] = {0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x76, 0x69, 0x64, 0x65, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x56, 0x69, 0x64, 0x65, 0x6f, 0x48, 0x61, 0x6e, 0x64, 0x6c, 0x65, 0x72, 0x00};
    memcpy(m_defaultData, defaultData, sizeof(defaultData));
    m_defaultDataLen = sizeof(defaultData);
}

BoxCellHdlr::~BoxCellHdlr()
{
}

int BoxCellHdlr::Divide(u_char *data, size_t len)
{
    u_char  *p = data;
    const u_char v = *p;
    ++p;
    NsprDebug("Box hdlr start divide");
    if (v == 1) {
        NsprWarn("not support version 1");
        return NSPR_OK;
    }

    uint32_t flag = 0;
    memcpy((u_char *)&flag + 1, p, 3);
    p += 3;
    NsprDebug("flag: 0x%x", htonl(flag));

    // pre-defined
    p += 4;

    u_char handlerType[5] = {0};
    memcpy(handlerType, p, 4);
    p += 4;
    NsprDebug("handler type %s", handlerType);

    // reserved
    p += 12;

    const int strLen = len - (p - data);
    char *trackName = new char[strLen];
    memcpy(trackName, p, strLen);
    p += strLen;
    NsprDebug("track name is %s", trackName);
    delete []trackName;
    
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
    NsprDebug("Box hdlr end divide");
	return NSPR_OK;
}

int BoxCellHdlr::Fuse(u_char *data, size_t &len)
{
    u_char *p = data;
    NsprDebug("Box hdlr start fuse");

    memcpy(p, m_defaultData, m_defaultDataLen);
    p += m_defaultDataLen;
    
    NsprDebug("Box hdlr end fuse, len = %d", p - data);
    len = p - data;
    return NSPR_OK;
}

bool BoxCellHdlr::isComposite()
{
	return false;
}

} // namespace nspr

