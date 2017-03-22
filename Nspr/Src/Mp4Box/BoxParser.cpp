#include "Nspr.h"

#include "BoxParser.h"
#include "InfoSummary.h"
#include "BoxFactory.h"

#include <stdio.h>

namespace nspr
{

BoxParser::BoxParser(const u_char *file)
    : m_fp(nullptr)
    , m_infoSummary(new InfoSummary())
    , m_boxFactory(new BoxFactory())
{
    snprintf((char *)m_filePath, sizeof(m_filePath), "%s", file);
}

BoxParser::~BoxParser()
{
    delete m_boxFactory;
    delete m_infoSummary;
}

int BoxParser::CheckBox(u_char *type, size_t boxLen, bool &isComposite)
{
    BoxCell *boxcell = m_boxFactory->GetBoxCell(type);
    if (!boxcell) {
        return NSPR_ERROR;
    }

    if (!boxcell->isComposite()) {
        u_char *data = new u_char[boxLen - 8];
        const size_t dataLen = boxLen - 8;
        int ret = 0;
        if ((ret = fread((char *)&data[0], 1, dataLen, m_fp)) < dataLen) {
            NsprWarn("reach file end, ret = %d", ret);
            delete boxcell;
            return NSPR_ERROR;
        }

        ret = boxcell->Divide(data, dataLen);
        if (ret == NSPR_ERROR) {
            delete []data;
            return NSPR_ERROR;
        }
        if (ret > 0) {
            int relPos = ret - dataLen;
            NsprDebug("relative position: %d", relPos);
            fseek(m_fp, relPos, SEEK_CUR);
        }

        delete []data;
    }

    return NSPR_OK;
}

int BoxParser::ReadBox()
{
    int boxLen = 0;
    int ret = 0;
    if ((ret = fread((char *)&boxLen, 1, 4, m_fp)) < 4) {
        NsprWarn("reach file end, ret = %d", ret);
        return NSPR_ERROR;
    }
    boxLen = ntohl(boxLen);
    if (boxLen == 1) {
        printf("large size, not supported, exit\n");
        return NSPR_ERROR;
    }

    u_char type[5] = {0};
    if (fread((char *)&type[0], 1, 4, m_fp) < 4) {
        printf("reach file end\n");
        return NSPR_ERROR;
    }
    printf("%s box, size %d\n", (char *)type, boxLen);

    bool isComposite = false;
    if (CheckBox(type, boxLen, isComposite)) {
        printf("check box failed\n");
        return NSPR_ERROR;
    }
    if (isComposite) {
        return NSPR_OK;
    }

    return NSPR_OK;
}

int BoxParser::ParseBox()
{
    m_fp = fopen((const char *)m_filePath, "rb");
    if (!m_fp) {
        printf("open file error\n");
        return NSPR_ERROR;
    }

    do {
        if (ReadBox()) {
            break;
        }
    } while (true);

    fclose(m_fp);
    return NSPR_OK;
}

} // namespace nspr

int main(int argc, char *argv[])
{
    if (argc < 2) {
        printf("%s mp4FileName\n", argv[0]);
        return -1;
    }
    nspr::BoxParser *bp = new nspr::BoxParser((const u_char *)argv[1]);
    bp->ParseBox();
    delete bp;
    return 0;
}

