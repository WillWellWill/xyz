#include "Nspr.h"

#include "BoxCellStsd.h"
#include "BoxCellAvcC.h"

namespace nspr
{

BoxCellStsd::BoxCellStsd()
    : m_spsLen(0)
    , m_ppsLen(0)
    , m_width(0)
    , m_height(0)
{
}

BoxCellStsd::~BoxCellStsd()
{
}

int BoxCellStsd::Divide(u_char *data, size_t len)
{
    u_char  *p = data;
    const u_char v = *p;
    ++p;
    NsprDebug("Box stsd start divide");
    if (v == 1) {
        NsprWarn("not support version 1");
        return NSPR_OK;
    }
    uint32_t flag = 0;
    memcpy((u_char *)&flag + 1, p, 3);
    p += 3;
    NsprDebug("flag: 0x%x", htonl(flag));

    uint32_t sampleEntryCount = ntohl(*((uint32_t *)p));
    p += 4;
    NsprDebug("sample entry count: %d", sampleEntryCount);

    int i = 0;
    for (; i < (int)sampleEntryCount; i++) {
        const u_char *entryStart = p;
        uint32_t entryLen = ntohl(*((uint32_t *)p));
        p += 4;
        u_char entryName[5] = {0};
        memcpy(entryName, p, 4);
        p += 4;

        NsprDebug("entry %s, len %d", entryName, entryLen);

        // reserved
        p += 6;

        const uint16_t dataReferenceIndex = ntohs(*((uint16_t *)p));
        p += 2;
        NsprDebug("data reference index %d", dataReferenceIndex);

        // pre-defined & reserved
        p += 16;

        const uint16_t width = ntohs(*((uint16_t *)p));
        p += 2;
        const uint16_t height = ntohs(*((uint16_t *)p));
        p += 2;
        NsprDebug("width = %d, height = %d", width, height);
		const uint32_t horizResolution = ntohl(*((uint32_t *)p));
        p += 4;
		const uint32_t vertResolution = ntohl(*((uint32_t *)p));
        p += 4;
        NsprDebug("horiz resolution = %d, vert resolution = %d, 0x%x", horizResolution, vertResolution, vertResolution);

        // reserved 
        p += 4;

        const uint16_t frameCount = ntohs(*((uint16_t *)p));
        p += 2;
        NsprDebug("frame count: %d", frameCount);

        p += 32;
		//string[32] compressorname;
        const uint16_t depth = ntohs(*((uint16_t *)p));
        p += 2;

        // pre-defined
        p += 2;

        NsprDebug("depth: %d, p - entryStart = %d", depth, p - entryStart);
        BoxCellAvcC avc;
        if (avc.Divide(p, entryLen - (p - entryStart))) {
            return NSPR_ERROR;
        }
    }

    NsprDebug("Box stsd end divide");
	return NSPR_OK;
}

int BoxCellStsd::Fuse(u_char *data, size_t &len)
{
    memset(data, 0, len);
    u_char *p = data;
    NsprDebug("Box stsd start fuse");
    u_char &v = *p++;
    // 0 flag & version
    p += 3;
    v = 0;

    uint32_t &sampleEntryCount = *((uint32_t *)p);
    p += 4;

    const int actualSampleEntryCount = 1;
    sampleEntryCount = ntohl(actualSampleEntryCount);

    int i = 0;
    for (; i < actualSampleEntryCount; i++) {
        u_char *entryStart = p;
        uint32_t &entryLen = *((uint32_t *)p);
        p += 4;

        u_char entryName[] = "avc1";
        memcpy(p, entryName, 4);
        p += 4;

        // reserved
        p += 6;

        uint16_t &dataReferenceIndex = *((uint16_t *)p);
        p += 2;
        dataReferenceIndex = htons(1);

        // pre-defined & reserved
        p += 16;

        uint16_t &width = *((uint16_t *)p);
        p += 2;
        uint16_t &height = *((uint16_t *)p);
        p += 2;

        width = htons(m_width);
        height = htons(m_height);

		uint32_t &horizResolution = *((uint32_t *)p);
        p += 4;
		uint32_t &vertResolution = *((uint32_t *)p);
        p += 4;
        horizResolution = vertResolution = htonl(4718592);

        // reserved 
        p += 4;

        uint16_t &frameCount = *((uint16_t *)p);
        p += 2;
        frameCount = ntohs(1);

        p += 32;
        
        uint16_t &depth = *((uint16_t *)p);
        p += 2;
        depth = ntohs(24);

        // pre-defined
        p[0] = 0xFF;
        p[1] = 0xFF;
        p += 2;

        p = WriteAVCDecoderConfigurationRecord(p);

        entryLen = ntohl(p - entryStart);
    }

    NsprDebug("Box stsd end fuse, len = %d", p - data);
    len = p - data;
    return NSPR_OK;
}

u_char *BoxCellStsd::WriteAVCDecoderConfigurationRecord(u_char *p)
{
    NsprDebug("write AVCDecoderConfigurationRecord");

    u_char *avcStart = p;
    uint32_t &avcLen = *((uint32_t *)p);
    p += 4;

    u_char name[] = "avcC";
    memcpy(p, name, 4);
    p += 4;

    u_char &configurationVersion = *p++;
    u_char &avcProfileIndication = *p++;
    u_char &profileCompatibility = *p++;
    u_char &avcLevelIndication = *p++;
    u_char &lengthSizeMinusOne = *p++;
    u_char &numOfSequenceParameterSets = *p++;
    
    configurationVersion = 1;
    avcProfileIndication = 100;
    profileCompatibility = 0;
    avcLevelIndication = 30;
    lengthSizeMinusOne = 0xFF;
    numOfSequenceParameterSets = 0xE1;

    uint16_t &sequenceParameterSetLength = *((uint16_t *)p);
    p += 2;
    sequenceParameterSetLength = ntohs(m_spsLen);
    
    memcpy(p, m_sps, m_spsLen);
    p += m_spsLen;
    
    u_char &numOfPictureParameterSets = *p++;
    numOfPictureParameterSets = 1;

    uint16_t &pictureParameterSetLength = *((uint16_t *)p);
    p += 2;

    pictureParameterSetLength = ntohs(m_ppsLen);
    memcpy(p, m_pps, m_ppsLen);
    p += m_ppsLen;

    avcLen = htonl(p - avcStart);
    NsprDebug("write AVCDecoderConfigurationRecord end");
    return p;
}

void BoxCellStsd::SetSps(u_char *sps, int spsLen)
{
    memcpy(m_sps, sps, spsLen);
    m_spsLen = spsLen;
}

void BoxCellStsd::SetPps(u_char *pps, int ppsLen)
{
    memcpy(m_pps, pps, ppsLen);
    m_ppsLen = ppsLen;
}

bool BoxCellStsd::isComposite()
{
	return false;
}

} // namespace nspr

